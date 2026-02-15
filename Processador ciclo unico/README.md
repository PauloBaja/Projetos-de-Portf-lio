# Simulação de Processador MIPS com Busca Binária Recursiva #
## Autor
Paulo Medeiros de Araújo

## Objetivo

Este projeto tem como objetivo simular um **Processador MIPS de ciclo único**, com ênfase na implementação de **Busca Binária Recursiva** em **Assembly**. O código de busca binária pode ser encontrado no arquivo **BuscaBinaria.asm**.

## Testes

Os testes podem ser realizados por meio de comandos `make`, permitindo simular dois cenários distintos: um com o elemento presente no vetor e outro com o elemento ausente. A seguir estão os comandos `make` disponíveis para rodar as simulações:

### Comandos Make

- **`make positivo`**  
  Simula o caso onde a busca binária encontra o elemento dentro do vetor. Esse teste requer **exatamente 3 recursões** para localizar o elemento.

- **`make negativo`**  
  Simula o caso onde o elemento **não pertence ao vetor**. Este teste também exige **exatamente 3 recursões** para determinar que o item não está presente.

- **`make all`**  
  Executa ambos os testes, **positivo** e **negativo**, de forma sequencial, na ordem apresentada acima.

## Breve descrição

Esse código foi pensado de forma que **Data_Memory_positivo.v** ou **Data_Memory_negativo.v** carrega os dados para a memória principal de dados e **Fetch_Unit.v** carrega memória de intruções a serem realizadas. Ao ser carregada no **main_positivo.v** ou **main_negativo.v**, realiza-se o **datapath** por meio de módulos que representam unidades de controle e de lógica-aritmética, assim como a presença de estruturas lógicas e condicionais próprias da linguagem de descrição que permite a multiplexação (seleção entre os caminhos distintos), o que permite a alta gama de instruções, tanto as mais simples do tipo **I** e **R**,as **lógico-aritméticas**:**ADD** **ORI** **STL**, até intruções mais complexas, como as de **jump**: **JUMP**, **JAL** e **JR**, assim como as de **branch**: **BEQ** e **BNE**.