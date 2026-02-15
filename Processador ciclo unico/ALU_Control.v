module ALU_Control (
    input wire [1:0] ALUOp,     //A partir do OPCODE, auxilia a ALU a selecionar entre os diferentes tipos de operação
    input wire [5:0] funct,     //Trecho seletor de operações do tipo I, seleciona entre as funções desse tipo
    input wire [5:0] opcode,    //OPCODE recebido do UCL
    output reg [3:0] ALU_Ctrl   //Seleciona a operação a ser utilizada futuramente na ALU
);

    always @(*) begin
        case(ALUOp)
            2'b00: ALU_Ctrl = 4'b0010; // lw, sw, addi, addiu
            2'b01: ALU_Ctrl = 4'b0110; // sub, conveniente para BEQ e BNE
            2'b10: begin // R-type ou I-type especial
                if (opcode == 6'b000000) begin // R-type
                    case(funct)
                        6'h20, 6'h21: ALU_Ctrl = 4'b0010; // add/addu
                        6'h2A: ALU_Ctrl = 4'b0111;        // slt
                        6'h00: ALU_Ctrl = 4'b0011;        // sll
                        6'h02: ALU_Ctrl = 4'b0100;        // srl
                        default: ALU_Ctrl = 4'b0010;      // ADD default
                    endcase
                end
                else if (opcode == 6'b001111) ALU_Ctrl = 4'b1000; // lui
                else ALU_Ctrl = 4'b0010; // default ADD
            end
            2'b11: ALU_Ctrl = 4'b0001; // ori
            default: ALU_Ctrl = 4'b0010; // ADD padrão
        endcase
    end
endmodule
