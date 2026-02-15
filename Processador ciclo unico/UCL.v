module UCL (        //Control Logic Unit 
    input wire [5:0] opcode,    //OPCODE para distinção entre os tipos de instrução a ser realizada
    output reg RegDst,          //Flag da unidade, representa o endereço do registrador a ser armazenado
    output reg Jump,            //Flag que determina a ocorrencia de jump ou não
    output reg BranchEQ,        //Flag que determina a ocorrencia de BEQ ou não
    output reg BranchNE,        //Flag que determina a ocorrencia de BNE ou não
    output reg MemRead,         //Flag que determina leitura de memória
    output reg MemtoReg,
    output reg [1:0] ALUOp,    // Reduzido para 2 bits
    output reg MemWrite,
    output reg ALUSrc,
    output reg RegWrite
);

    always @(*) begin
        // Valores padrão
        RegDst = 0;
        Jump = 0;
        BranchEQ = 0;
        BranchNE = 0;
        MemRead = 0;
        MemtoReg = 0;
        ALUOp = 2'b00;    // 00: add, 01: sub, 10: R-type, 11: or
        MemWrite = 0;
        ALUSrc = 0;
        RegWrite = 0;

        case(opcode)
            // R-type
            6'b000000: begin
                RegDst = 1;
                RegWrite = 1;
                ALUSrc = 0;
                ALUOp = 2'b10;    // R-type: decodificado por funct
            end
            
            // addi e adiu
            6'b001000, 6'b001001: begin
                RegWrite = 1;
                ALUSrc = 1;
                ALUOp = 2'b00;    // ADD
            end

            // ori
            6'b001101: begin
                RegWrite = 1;
                ALUSrc = 1;
                ALUOp = 2'b11;    // OR
            end

            // slti
            6'b001010: begin
                RegWrite = 1;
                ALUSrc = 1;
                ALUOp = 2'b10;    // Usa ALUControl para SLT
            end

            // lui
            6'b001111: begin
                RegWrite = 1;
                ALUSrc = 1;
                ALUOp = 2'b10;    // Usa ALUControl para LUI
            end

            // lw
            6'b100011: begin
                RegWrite = 1;
                MemRead = 1;
                MemtoReg = 1;
                ALUSrc = 1;
                ALUOp = 2'b00;    // ADD (cálculo de endereço)
            end

            // sw
            6'b101011: begin
                MemWrite = 1;
                ALUSrc = 1;
                ALUOp = 2'b00;    // ADD (cálculo de endereço)
            end

            // beq
            6'b000100: begin
                BranchEQ = 1;
                ALUSrc = 0;
                ALUOp = 2'b01;    // SUB (para comparação)
            end

            // bne
            6'b000101: begin
                BranchNE = 1;
                ALUSrc = 0;
                ALUOp = 2'b01;    // SUB (para comparação)
            end

            // j
            6'b000010: begin
                Jump = 1;
            end

            6'b000011: begin  // jal
                Jump = 1;
                RegWrite = 1;        // Escreve no $ra (registrador de retorno)
                MemtoReg = 0;        // Não precisa ler da memória
                ALUSrc = 0;          // Não há necessidade de usar o imediato
                ALUOp = 2'b00;       // Add, para calcular o endereço de salto
            end

            default: begin
                // Instrução não reconhecida - manter defaults
            end
        endcase
    end
endmodule
