module Fetch_Unit(
    input wire [31:0] addr,      // Endereço da instrução a ser buscada
    output wire [31:0] instrucao // Instrução lida a ser decodificada e futuramente executada
);

    // Memória de instruções (69 palavras de 32 bits), o suficiente para ler as 69 linhas do quicksort.hex
    reg [31:0] memoria [0:68];

    // Inicialização da memória com algumas instruções
    integer i;
    initial begin
        for (i = 0; i < 256; i = i + 1) memoria[i] = 32'b0; // Inicializa tudo com 0
        $readmemh("Busca_Binaria_Recursiva.hex", memoria); //Instruções definidas em palavras de 8 bytes por intrução correspondente a busca binária adaptada
    end

    // Leitura combinacional
    assign instrucao = memoria[addr[9:2]];
    // Usa os bits 9:2 para indexar (alinhado em palavras)

endmodule
