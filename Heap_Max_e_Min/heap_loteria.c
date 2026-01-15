#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct no {
    uint8_t bilhete[15];
    char hex128[33];
    uint8_t acertos;
} No;

typedef struct {
    char** hexs;
    size_t tamanho;
    size_t capacidade;
} ListaHex;

void inicializarLista(ListaHex* lista) {
    lista->tamanho = 0;
    lista->capacidade = 16;
    lista->hexs = malloc(lista->capacidade * sizeof(char*));
    if (!lista->hexs) {
        fprintf(stderr, "Erro ao alocar memória para lista\n");
        exit(1);
    }
}

void adicionarHex(ListaHex* lista, char* hex_ptr) {
    if (lista->tamanho >= lista->capacidade) {
        lista->capacidade *= 2;
        lista->hexs = realloc(lista->hexs, lista->capacidade * sizeof(char*));
        if (!lista->hexs) {
            fprintf(stderr, "Erro ao realocar memória para lista\n");
            exit(1);
        }
    }
    lista->hexs[lista->tamanho++] = hex_ptr;
}

void resetarLista(ListaHex* lista) {
    lista->tamanho = 0;
}

void imprimirListaHex(ListaHex* lista, FILE* saida) {
    for (size_t i = 0; i < lista->tamanho; i++) {
        fprintf(saida, "%s\n", lista->hexs[i]);
    }
}

void liberarLista(ListaHex* lista) {
    free(lista->hexs);
}

void contarAcertosMerge(uint8_t sorteio[10], No* x) {
    int i = 0, j = 0, acc = 0;
    while (i < 10 && j < 15) {
        if (sorteio[i] < x->bilhete[j]) {
            i++;
        } else if (sorteio[i] > x->bilhete[j]) {
            j++;
        } else {
            acc++;
            i++;
            j++;
        }
    }
    x->acertos = acc;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Uso: %s <entrada.txt> <saida.txt>\n", argv[0]);
        return 1;
    }

    FILE* entrada = fopen(argv[1], "r");
    if (!entrada) {
        printf("Erro ao abrir arquivo de entrada.\n");
        return 1;
    }

    FILE* saida = fopen(argv[2], "w");
    if (!saida) {
        fclose(entrada);
        printf("Erro ao abrir arquivo de saída.\n");
        return 1;
    }

    int dinheiro;
    uint32_t na;
    uint8_t sorteio[10];

    if (fscanf(entrada, "%d %u", &dinheiro, &na) != 2) {
        fprintf(stderr, "Erro na leitura dos dados iniciais\n");
        fclose(entrada);
        fclose(saida);
        return 1;
    }

    No* dados = malloc(na * sizeof(No));
    if (!dados) {
        fprintf(stderr, "Erro de alocação de memória\n");
        fclose(entrada);
        fclose(saida);
        return 1;
    }

    for (int i = 0; i < 10; i++) {
        uint32_t temp;
        if (fscanf(entrada, "%u", &temp) != 1 || temp > 255) {
            fprintf(stderr, "Erro na leitura do sorteio[%d]\n", i);
            goto cleanup;
        }
        sorteio[i] = (uint8_t)temp;
    }

    for (uint32_t i = 0; i < na; i++) {
        if (fscanf(entrada, "%32s", dados[i].hex128) != 1) {
            fprintf(stderr, "Erro na leitura do hex128 da aposta %u\n", i);
            goto cleanup;
        }

        for (uint32_t k = 0; k < 15; k++) {
            uint32_t valor;
            if (fscanf(entrada, "%u", &valor) != 1 || valor > 255) {
                fprintf(stderr, "Erro no bilhete[%u] da aposta %u\n", k, i);
                goto cleanup;
            }
            dados[i].bilhete[k] = (uint8_t)valor;
        }

        contarAcertosMerge(sorteio, &dados[i]);
    }

    ListaHex gMAX, gMIN;
    inicializarLista(&gMAX);
    inicializarLista(&gMIN);

    uint8_t maior_acertos = 0, menor_acertos = 10;

    for (uint32_t i = 0; i < na; i++) {
        uint8_t a = dados[i].acertos;

        if (a > maior_acertos) {
            maior_acertos = a;
            resetarLista(&gMAX);
            adicionarHex(&gMAX, dados[i].hex128);
        } else if (a == maior_acertos) {
            adicionarHex(&gMAX, dados[i].hex128);
        }

        if (a < menor_acertos) {
            menor_acertos = a;
            resetarLista(&gMIN);
            adicionarHex(&gMIN, dados[i].hex128);
        } else if (a == menor_acertos) {
            adicionarHex(&gMIN, dados[i].hex128);
        }
    }

    uint32_t premio_metade = dinheiro / 2;
    uint32_t premio_max = premio_metade / (gMAX.tamanho ? gMAX.tamanho : 1);
    uint32_t premio_min = premio_metade / (gMIN.tamanho ? gMIN.tamanho : 1);

    fprintf(saida, "[%zu:%u:%u]\n", gMAX.tamanho, maior_acertos, premio_max);
    imprimirListaHex(&gMAX, saida);

    fprintf(saida, "[%zu:%u:%u]\n", gMIN.tamanho, menor_acertos, premio_min);
    imprimirListaHex(&gMIN, saida);

    liberarLista(&gMAX);
    liberarLista(&gMIN);

cleanup:
    free(dados);
    fclose(entrada);
    fclose(saida);
    return 0;
}
