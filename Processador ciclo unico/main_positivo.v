module main_positivo(
    input wire clk,
    input wire reset
);

    // ================= PC =================//
    reg [31:0] pc;                                      //Definição do Program Counter
    wire [31:0] pc_plus_4, branch_target, jump_target;  //Objetos que definem a próximo PC a ser considerado

    assign pc_plus_4 = pc + 32'd4;

    // ================= BANCO DE REGISTRADORES =================//
    wire [31:0] ReadData1, ReadData2;                  //Valores lidos dos registradores de origem
    wire [31:0] WriteData;                             //O dado que será escrito no registrador destino
    wire [4:0] WriteRegister;                          //Número do registrador destino no banco de Registradores

    // ================= FETCH =================//
    wire [31:0] instrucao;
    Fetch_Unit fetch_unit (
        .addr(pc),
        .instrucao(instrucao)
    );

    // ================= DECODE =================
    // Decodificação dos campos da instrução
    wire [5:0] opcode = instrucao[31:26];  // Campo opcode (6 bits) que define o tipo da instrução (R, I ou J)
    wire [4:0] rs = instrucao[25:21];      // Registrador fonte 1 (rs), os 5 bits do campo rs
    wire [4:0] rt = instrucao[20:16];      // Registrador fonte 2 (rt), os 5 bits do campo rt
    wire [4:0] rd = instrucao[15:11];      // Registrador destino (rd), os 5 bits do campo rd (para instruções R-type)
    wire [4:0] shamt = instrucao[10:6];    // Campo shamt, utilizado em operações de deslocamento 
    wire [5:0] funct = instrucao[5:0];     // Função, os 6 bits finais que detalham a operação dentro de uma instrução R-type
    wire [15:0] imm = instrucao[15:0];     // Imediato, valor constante em instruções do tipo I 

    wire [31:0] imm_ext = {{16{imm[15]}}, imm};  // sign-extended imediato

    // ================= UNIDADE DE CONTROLE =================//
    // Sinais de controle gerados pela unidade de controle (Control Unit)
    wire RegDst;      // Seleciona qual registrador será escrito: rd (R-type) ou rt (I-type)
    wire Jump;        // Sinal de jump: indica se a instrução é um salto (J ou JAL)
    wire BranchEQ;    // Sinal de branch if equal (BEQ): indica se deve pular quando rs == rt
    wire BranchNE;    // Sinal de branch if not equal (BNE): indica se deve pular quando rs != rt
    wire MemRead;     // Habilita leitura da memória de dados 
    wire MemtoReg;    // Seleciona se o dado a ser escrito no registrador vem da memória (1) ou do ALU (0)
    wire MemWrite;    // Habilita escrita na memória de dados
    wire ALUSrc;      // Seleciona a segunda entrada da ALU: registrador (0) ou imediato (1)
    wire RegWrite;    // Habilita escrita no banco de registradores

    // ==============Sinais de controle da ALU=========//
    wire [1:0] ALUOp; // Sinal de controle intermediário da ALU: indica o tipo de operação (R-type, BEQ, etc.)
    wire [3:0] ALU_Ctrl; // Sinal final de controle da ALU, definindo a operação específica (ADD, SUB, AND, OR, etc.)

    UCL ucl(                  //Instanciação da unidade lógica de controle
        .opcode(opcode),
        .RegDst(RegDst),
        .Jump(Jump),
        .BranchEQ(BranchEQ),
        .BranchNE(BranchNE),
        .MemRead(MemRead),
        .MemtoReg(MemtoReg),
        .ALUOp(ALUOp),
        .MemWrite(MemWrite),
        .ALUSrc(ALUSrc),
        .RegWrite(RegWrite)
    );

    ALU_Control alucontrol(     //Instanciação do Controle da ALU
        .ALUOp(ALUOp),
        .funct(funct),
        .opcode(opcode),
        .ALU_Ctrl(ALU_Ctrl)
    );

    // ================= BANCO DE REGISTRADORES =================//
    // MUX de destino de escrita: escolhe rd para R-type, R31 para jal(definição do MIPS), ou rt para I-type
    assign WriteRegister = (opcode == 6'b000000) ? rd :       //R-type     
                           (opcode == 6'b000011) ? 5'd31 :    // jal
                           rt;                               // I-type

    Registradores banco_regs (   //Instanciação do Banco de Registradores
        .clock(clk),
        .reset(reset),
        .ReadRegister1(rs),
        .ReadRegister2(rt),
        .WriteRegister(WriteRegister),
        .WriteData(WriteData),
        .RegWrite(RegWrite),
        .ReadData1(ReadData1),
        .ReadData2(ReadData2)
    );

    // ================= EXECUTE =================//

    // Registradores temporários para entrada da ALU
    reg [31:0] A, B;

    // Saída da ALU
    wire [31:0] ALUResult;  // Resultado da operação
    wire Zero;               // Indicador de resultado zero (usado em Branch EQ/NE)

    // ================ Operandos ================//
    always @(*) begin
        // A sempre recebe o valor do registrador rs
        A = ReadData1;

        // B é selecionado por um "MUX lógico":
        // 1. LUI: imediato deslocado 16 bits para os 16 MSBs
        // 2. ALUSrc = 1: imediato estendido (I-type aritmético ou load/store)
        // 3. ALUSrc = 0: registrador rt (R-type)
        if (opcode == 6'b001111)          // LUI
            B = imm_ext << 16;
        else if (ALUSrc)
            B = imm_ext;
        else
            B = ReadData2;
    end

    ALU alu(          //Instanciação da ALU
        .A(A),
        .B(B),
        .ALU_Ctrl(ALU_Ctrl),
        .shamt(shamt),
        .Result(ALUResult),
        .Zero(Zero)
    );

    // ================= MEMÓRIA DE DADOS =================
wire [31:0] MemData;     // Dados lidos da memória de dados (resultado de LW)

    Data_Memory_Positivo DM ( //intanciação da memória de dados a serem lidos e processados
        .clk(clk),
        .MemRead(MemRead),
        .MemWrite(MemWrite),
        .address(ALUResult),
        .writeData(ReadData2),
        .readData(MemData)
    );

    // ================= WRITEBACK =================
    assign WriteData = (opcode == 6'b000011) ?  // MUX de Writeback: seleciona o dado a ser escrito no registrador destino:
                        pc_plus_4 :             // 1. `pc_plus_4` para `jal` (salva o endereço de retorno)
                    (MemtoReg) ? MemData :   // 2. `MemData` para `lw` (dado lido da memória)
                        ALUResult;              // 3. `ALUResult` para R-type, I-type ou LUI (resultado da ALU)                
                                              


    // ================= PC UPDATE =================
    wire [31:0] branch_target_calc = pc_plus_4 + (imm_ext << 2);  // Calcula o endereço de destino para branches, desloca o imediato de 16 bits para 32 
    wire [31:0] jump_target_calc   = {pc_plus_4[31:28], instrucao[25:0], 2'b00};  // Calcula o endereço de salto, combina o PC atual com a instrução de salto (instrucao[25:0]) e ajusta os 2 bits finais

    always @(posedge clk or posedge reset) begin
        if (reset)
            pc <= 32'b0;  // Se houver reset, o PC é reiniciado para 0
        else if (opcode == 6'b000000 && funct == 6'b001000) // jr: Se a instrução for `jr`, o PC é atualizado com o valor de ReadData1 (endereço de salto)
            pc <= ReadData1;
        else if (Jump)
            pc <= jump_target_calc;  // Se Jump for ativo, o PC é atualizado para o endereço de salto (Jump/JAL)
        else if (BranchEQ && Zero)
            pc <= branch_target_calc;  // Se BranchEQ for ativo e Zero for 1, o PC é atualizado para o destino do branch (Branch EQ)
        else if (BranchNE && !Zero)
            pc <= branch_target_calc;  // Se BranchNE for ativo e Zero for 0, o PC é atualizado para o destino do branch (Branch NE)
        else
            pc <= pc_plus_4;  // Caso padrão: o PC avança para o próximo endereço (pc + 4)
    end

endmodule
