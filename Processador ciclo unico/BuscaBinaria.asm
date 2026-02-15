.data
vetor:    .word 9,10,11,12,13,14,15,16 
buscado:  .word 11 #valor a ser buscado
posicao:  .word 0  #posição a ser retornada

.text
.globl main
main:
    # Chamada da busca binária recursiva
    lw   $a0, buscado   # valor a buscar
    li   $a1, 0          # i = 0
    li   $a2, 7          # f = tamanho-1 // VALOR FIXO para 8 elementos
    jal  bb              # Inicia a recursão de busca binária

    # Armazena o retorno da recursão em 'posicao'
    sw   $v0, posicao    

    # Fim do programa
    li   $v0, 10         # Código de saída
    syscall

# Função bb (busca binária recursiva)
# a0 = valor buscado
# a1 = i (inicio)
# a2 = f (fim)
# v0 = retorno (posição ou -1)
.globl bb
bb:
    addiu $sp, $sp, -16  # Criação de espaço na pilha para a recursão
    sw    $ra, 0($sp)
    sw    $a1, 4($sp)
    sw    $a2, 8($sp)

    # if i > f return -1 // Caso base
    bgt   $a1, $a2, nao_achou  

    # meio = (i + f) / 2
    addu  $t0, $a1, $a2      # i + f
    srl   $t0, $t0, 1         # meio = (i + f) / 2

    # vetor[meio]
    la    $t1, vetor
    sll   $t2, $t0, 2         # meio * 4 (deslocamento para índice de word)
    addu  $t1, $t1, $t2
    lw    $t3, 0($t1)         # t3 = vetor[meio]

    beq   $t3, $a0, achou     # Se vetor[meio] == buscado, achou
    blt   $a0, $t3, esquerda  # Se buscado < vetor[meio], vai para a esquerda
    bgt   $a0, $t3, direita   # Se buscado > vetor[meio], vai para a direita

achou:    
    move  $v0, $t0           # Retorna a posição encontrada
    j     fim_bb             # Fim da busca binária

esquerda:
    addiu $a2, $t0, -1       # Atualiza fim para meio-1
    jal   bb                  # Recursão para a esquerda
    j     fim_bb              # Fim da busca binária

direita:
    addiu $a1, $t0, 1        # Atualiza início para meio+1
    jal   bb                  # Recursão para a direita
    j     fim_bb              # Fim da busca binária

nao_achou:
    li    $v0, -1             # Retorna -1, indicando que não encontrou

fim_bb:                     
    lw    $ra, 0($sp)         # Restaura o valor de retorno
    lw    $a1, 4($sp)         # Restaura o valor de a1
    lw    $a2, 8($sp)         # Restaura o valor de a2
    addiu $sp, $sp, 16        # Desfaz o espaço alocado na pilha
    jr    $ra                 # Retorna ao chamador
