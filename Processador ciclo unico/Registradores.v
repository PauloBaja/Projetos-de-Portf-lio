module Registradores(
    input wire clock,
    input wire reset,
    input wire [4:0] ReadRegister1,  // Endereço do registrador para leitura 1
    input wire [4:0] ReadRegister2,  // Endereço do registrador para leitura 2
    input wire [4:0] WriteRegister,  // Endereço do registrador para escrita
    input wire [31:0] WriteData,     // Dados a serem escritos
    input wire RegWrite,             // Habilitação de escrita
    output wire [31:0] ReadData1,    // Dados lidos do registrador 1
    output wire [31:0] ReadData2     // Dados lidos do registrador 2
);

    // Banco de registradores: 32 registradores de 32 bits
    reg [31:0] registers [31:0];

    // Inicialização dos registradores (opcional, apenas para simulação)
    integer i;
    initial begin
        for (i = 0; i < 32; i = i + 1) begin
            registers[i] = 32'b0; // Inicializa todos com zero
        end
    end

    // Leitura combinacional
    assign ReadData1 = (ReadRegister1 == 5'b0) ? 32'b0 : registers[ReadRegister1];
    assign ReadData2 = (ReadRegister2 == 5'b0) ? 32'b0 : registers[ReadRegister2];

    // Escrita sincrona
    always @(posedge clock or posedge reset) begin
        if (reset) begin
            for (i = 0; i < 32; i = i + 1) registers[i] <= 32'b0;
        end
        else begin
            // Só escreve se RegWrite estiver ativado E não for o registrador 0
            if (RegWrite && WriteRegister != 5'b0) begin
                registers[WriteRegister] <= WriteData;
            end
        end
    end

endmodule
