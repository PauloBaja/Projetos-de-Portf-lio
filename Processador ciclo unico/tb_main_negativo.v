`timescale 1ns/1ns


module tb_main_negativo;

    reg clk;
    reg reset;
    integer i;

    //Instancia o Processador
    main_negativo uut (
        .clk(clk),
        .reset(reset)
    );

    //Gera o Clock (Inverte a cada 10ns = Ciclo de 20ns)
    always #10 clk = ~clk;

    initial begin
        $dumpfile("tb_main_negativo.vcd");
        $dumpvars(0, tb_main_negativo);

        clk = 0;
        reset = 1; //Zera o processador
        #20;     
        reset = 0; // Dá a partida
        
        #1000000;

        $display("=== RESULTADO DA MEMÓRIA DE DADOS BUSCA NEGATIVA===");

        for (i = 0; i < 32; i = i + 1) begin
            if (i < 8) begin
                $display("índice de Memoria %0d: %d  <-- elemento de posição %0d", 
                        i, $signed(uut.DM.memory[i]),i);
            end
             if(i == 8) begin
                $display("índice de Memoria %0d: %d  <-- elemento a ser buscado:%d", 
                        i, $signed(uut.DM.memory[i]),$signed(uut.DM.memory[i]));
            end
            if(i == 9) begin
                $display("índice de Memoria %0d: %d  <-- posição relativa ao item buscado: %d(caso seja -1 não pertence ao vetor)", 
                        i, $signed(uut.DM.memory[i]),$signed(uut.DM.memory[i-1]));
            end
            
             else begin
                $display("índice de Memoria %0d: %d", 
                        i, $signed(uut.DM.memory[i]));
            end

        end

        $display("=======================================");

      // ================= REGISTRADORES FINAIS BUSCA (TABELA DETALHADA) =================
        $display("\n=== REGISTRADORES DEPOIS DA BUSCA NEGATIVA ===");
        $display("| R# | Nome  | Valor      | Interpretação |");
        $display("|----|-------|-----------|---------------|");
        $display("| 0  | $zero | %08h | Reservado para  Register$zero |", uut.banco_regs.registers[0]);
        $display("| 1  | $at   | %08h | Pseudo/assembler |", uut.banco_regs.registers[1]);
        $display("| 2  | $v0   | %08h | Resultado / retorno da função|", uut.banco_regs.registers[2]);
        $display("| 3  | $v1   | %08h | Não usado         |", uut.banco_regs.registers[3]);
        $display("| 4  | $a0   | %08h | Chave buscada     |", uut.banco_regs.registers[4]);
        $display("| 5  | $a1   | %08h | Low da busca      |", uut.banco_regs.registers[5]);
        $display("| 6  | $a2   | %08h | High da busca     |", uut.banco_regs.registers[6]);
        $display("| 7  | $a3   | %08h | Não usado         |", uut.banco_regs.registers[7]);
        $display("| 8  | $t0   | %08h | Mid / temporário  |", uut.banco_regs.registers[8]);
        $display("| 9  | $t1   | %08h | Temporário        |", uut.banco_regs.registers[9]);
        $display("| 10 | $t2   | %08h | Temporário        |", uut.banco_regs.registers[10]);
        $display("| 11 | $t3   | %08h | Valor A[mid]      |", uut.banco_regs.registers[11]);
        $display("| 12 | $t4   | %08h | Não usado         |", uut.banco_regs.registers[12]);
        $display("| 13 | $t5   | %08h | Não usado         |", uut.banco_regs.registers[13]);
        $display("| 14 | $t6   | %08h | Não usado         |", uut.banco_regs.registers[14]);
        $display("| 15 | $t7   | %08h | Não usado         |", uut.banco_regs.registers[15]);
        $display("| 16 | $s0   | %08h | Não usado         |", uut.banco_regs.registers[16]);
        $display("| 17 | $s1   | %08h | Não usado         |", uut.banco_regs.registers[17]);
        $display("| 18 | $s2   | %08h | Não usado         |", uut.banco_regs.registers[18]);
        $display("| 19 | $s3   | %08h | Não usado         |", uut.banco_regs.registers[19]);
        $display("| 20 | $s4   | %08h | Não usado         |", uut.banco_regs.registers[20]);
        $display("| 21 | $s5   | %08h | Não usado         |", uut.banco_regs.registers[21]);
        $display("| 22 | $s6   | %08h | Não usado         |", uut.banco_regs.registers[22]);
        $display("| 23 | $s7   | %08h | Não usado         |", uut.banco_regs.registers[23]);
        $display("| 24 | $t8   | %08h | Não usado         |", uut.banco_regs.registers[24]);
        $display("| 25 | $t9   | %08h | Não usado         |", uut.banco_regs.registers[25]);
        $display("| 26 | $k0   | %08h | Reservado kernel  |", uut.banco_regs.registers[26]);
        $display("| 27 | $k1   | %08h | Reservado kernel  |", uut.banco_regs.registers[27]);
        $display("| 28 | $gp   | %08h | Ponteiro global   |", uut.banco_regs.registers[28]);
        $display("| 29 | $sp   | %08h | Stack pointer (pilha) <--guarda nesse caso as 3 recursões(16 bytes cada=>48 bytes que armazenam o valor ffffffd0)|", uut.banco_regs.registers[29]);
        $display("| 30 | $fp   | %08h | Frame pointer      |", uut.banco_regs.registers[30]);
        $display("| 31 | $ra   | %08h | Retorno da função |", uut.banco_regs.registers[31]);

        $finish;
    end

endmodule