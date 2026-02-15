module Data_Memory_Positivo(         // Carrega o vetor com elemento buscado presente
    input wire clk,                  // Sinal de clock
    input wire MemRead,              // Sinal de leitura
    input wire MemWrite,             // Sinal de escrita
    input wire [31:0] address,       // Endereço de memória
    input wire [31:0] writeData,     // Dados a serem escritos
    output wire [31:0] readData      // Dados lidos
);

    // Memória de dados (256 palavras de 32 bits)
    reg [31:0] memory [255:0];

    // Inicializa a memória para simulação (opcional)
    integer i;
    initial begin
        // Inicializa toda a memória com zeros
        for(i = 0; i < 256; i = i + 1)
            memory[i] = 32'b0;
        
        // Inicialização de dados 
        memory[0] = 32'd9;      // elemento de posição 0
        memory[1] = 32'd10;     // elemento de posição 1
        memory[2] = 32'd11;     // elemento de posição 2
        memory[3] = 32'd12;     // elemento de posição 3
        memory[4] = 32'd13;     // elemento de posição 4
        memory[5] = 32'd14;     // elemento de posição 5
        memory[6] = 32'd15;     // elemento de posição 6
        memory[7] = 32'd16;     // elemento de posição 7
        memory[8] = 32'd11;     // elemento a ser buscado 
        memory[9] = 32'd0;      //Armazana o índice do elemento na busca(-1 para não presente)
    end

    // Leitura combinacional
    assign readData = (MemRead) ? memory[address[9:2]] : 32'b0;

    // Escrita síncrona
    always @(posedge clk) begin
        // Escreve na memória na borda de subida do clock
        if(MemWrite) begin
            memory[address[9:2]] <= writeData;
        end
    end

endmodule
