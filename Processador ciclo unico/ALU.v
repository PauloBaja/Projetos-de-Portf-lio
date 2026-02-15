module ALU (                    //Definição da ALU conveniente com a necessária para a simulação do código em assembly.
    input wire [31:0] A,        //Primeiro operando da ALU
    input wire [31:0] B,        //Segundo operando da ALU
    input wire [3:0] ALU_Ctrl,  //Controlador de opreações da ALU, 4 bits, possibilita até 16 operações diferentes.
    input wire [4:0] shamt,     //Índice usado para realizar operações do tipo shift
    output reg [31:0] Result,   //Comporta o resultado final da ALU
    output wire Zero            //Flag ativa em zero, conveninete para operações do tipo Branch
);

    always @(*) begin
        case(ALU_Ctrl)
            4'b0001: Result = A | B;          // OR
            4'b0010: Result = A + B;          // ADD
            4'b0011: Result = B << shamt;     // SLL
            4'b0100: Result = B >> shamt;     // SRL
            4'b0110: Result = A - B;          // SUB (beq, bne)
            4'b0111: Result = ($signed(A) < $signed(B)) ? 32'd1 : 32'd0; // SLT
            4'b1000: Result = {B[15:0], 16'b0}; // LUI
            default: Result = 32'b0;
        endcase
    end
    assign Zero = (Result == 32'b0);

endmodule
