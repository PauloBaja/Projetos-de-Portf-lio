# Caminho para os executáveis
IVERILOG = iverilog
VVP     = vvp
GTK     = gtkwave

# Arquivos de código fonte
SRCP = main_positivo.v ALU.v ALU_Control.v Data_Memory_Positivo.v Fetch_Unit.v Registradores.v UCL.v 
SRCN = main_negativo.v ALU.v ALU_Control.v Data_Memory_Negativo.v Fetch_Unit.v Registradores.v UCL.v
SRCA = main_positivo.v main_negativo.v ALU.v ALU_Control.v Data_Memory_Negativo.v Data_Memory_Positivo.v Fetch_Unit.v Registradores.v UCL.v 

# Arquivos de testbench
TBP  = tb_main_positivo.v
TBN  = tb_main_negativo.v

# Executável de saída
OUT = asm.out

# Regra para a simulação positiva
positivo:
	$(IVERILOG) -o $(OUT) $(SRCP) $(TBP)
	$(VVP) $(OUT)

# Regra para a simulação negativa
negativo:
	$(IVERILOG) -o $(OUT) $(SRCN) $(TBN)
	$(VVP) $(OUT)

# Regra para a simulação de ambos, executando primeiro o positivo e depois o negativo
all: positivo negativo


