#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct no {
    int F;
    char hex[3];
    struct no *E;
    struct no *D;
    struct no *prox;
    char *codigo;
} no;

typedef struct {
    no **dados;
    int tamanho;
    int capacidade;
} heap_min;


heap_min *criar_heap(int capacidade) {
    heap_min *h = malloc(sizeof(heap_min));
    h->dados = malloc(capacidade * sizeof(no *));
    h->tamanho = 0;
    h->capacidade = capacidade;
    return h;
}

void trocar(no **a, no **b) {
    no *tmp = *a;
    *a = *b;
    *b = tmp;
}

void subir(heap_min *h, int i) {
    while (i > 0) {
        int p = (i - 1) / 2;
        if (h->dados[p]->F <= h->dados[i]->F) break;
        trocar(&h->dados[p], &h->dados[i]);
        i = p;
    }
}

void descer(heap_min *h, int i) {
    while (1) {
        int e = 2 * i + 1;
        int d = 2 * i + 2;
        int menor = i;

        if (e < h->tamanho && h->dados[e]->F < h->dados[menor]->F)
            menor = e;
        if (d < h->tamanho && h->dados[d]->F < h->dados[menor]->F)
            menor = d;

        if (menor == i) break;
        trocar(&h->dados[i], &h->dados[menor]);
        i = menor;
    }
}

void inserir_heap(heap_min *h, no *n) {
    h->dados[h->tamanho] = n;
    subir(h, h->tamanho);
    h->tamanho++;
}

no *extrair_min_heap(heap_min *h) {
    if (h->tamanho == 0) return NULL;
    no *min = h->dados[0];
    h->dados[0] = h->dados[--h->tamanho];
    descer(h, 0);
    return min;
}

no *construir_arvore(no *vetor, int n) {
    heap_min *heap = criar_heap(n);

    for (int i = 0; i < n; i++) {
        if (vetor[i].F > 0) {
            no *folha = malloc(sizeof(no));
            *folha = vetor[i];
            folha->E = folha->D = folha->prox = NULL;
            inserir_heap(heap, folha);
        }
    }

    while (heap->tamanho > 1) {
        no *x = extrair_min_heap(heap);
        no *y = extrair_min_heap(heap);

        no *pai = malloc(sizeof(no));
        pai->F = x->F + y->F;
        pai->hex[0] = '\0';
        pai->E = x;
        pai->D = y;
        pai->prox = NULL;
        pai->codigo = NULL;

        inserir_heap(heap, pai);
    }

    no *raiz = extrair_min_heap(heap);
    free(heap->dados);
    free(heap);
    return raiz;
}

void gerar_codigos_huffman(no *raiz, char *codigo, int prof, char **tabela) {
    if (!raiz) return;

    if (!raiz->E && !raiz->D) {
        if (prof == 0) {
            codigo[0] = '0';
            codigo[1] = '\0';
        } else {
            codigo[prof] = '\0';
        }

        int idx = (unsigned char)strtol(raiz->hex, NULL, 16);
        tabela[idx] = strdup(codigo);
        raiz->codigo = strdup(codigo);
        return;
    }

    codigo[prof] = '0';
    gerar_codigos_huffman(raiz->E, codigo, prof + 1, tabela);

    codigo[prof] = '1';
    gerar_codigos_huffman(raiz->D, codigo, prof + 1, tabela);
}

void imprimir_codigos(FILE *saida, char **tabela) {
    for (int i = 0; i < 256; i++) {
        if (tabela[i]) {
            fprintf(saida, "%02X -> %s\n", i, tabela[i]);
        }
    }
}

void compactar(char *C, unsigned char *E, int tamanhoE, char **tabela) {
    C[0] = '\0';
    for (int i = 0; i < tamanhoE; i++) {
        unsigned char idx = E[i];
        if (tabela[idx]) {
            strcat(C, tabela[idx]);
        }
    }
}

int bits_para_bytes(const char *bits, unsigned char *bytes) {
    int len = strlen(bits);
    int byte_index = 0;
    unsigned char byte = 0;
    int bit_count = 0;

    for (int i = 0; i < len; i++) {
        byte = (byte << 1) | (bits[i] == '1' ? 1 : 0);
        bit_count++;

        if (bit_count == 8) {
            bytes[byte_index++] = byte;
            byte = 0;
            bit_count = 0;
        }
    }

    if (bit_count > 0) {
        byte = byte << (8 - bit_count);
        bytes[byte_index++] = byte;
    }

    return byte_index;
}

void liberar_arvore(no *raiz) {
    if (!raiz) return;
    liberar_arvore(raiz->E);
    liberar_arvore(raiz->D);
    free(raiz->codigo);
    free(raiz);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <entrada> <saida>\n", argv[0]);
        return 1;
    }

    FILE *entrada = fopen(argv[1], "r");
    FILE *saida   = fopen(argv[2], "w");
    if (!entrada || !saida) {
        printf("Erro ao abrir arquivos.\n");
        return 1;
    }

    int N;
    fscanf(entrada, "%d", &N);

    for (int i = 0; i < N; i++) {
        int qtd;
        fscanf(entrada, "%d", &qtd);

        no *vetor = calloc(256, sizeof(no));
        unsigned char *entrada_bytes = malloc(qtd * sizeof(unsigned char));

        for (int j = 0; j < qtd; j++) {
            char hex[3];
            int idx;

            fscanf(entrada, "%2s", hex);
            sscanf(hex, "%x", &idx);

            strcpy(vetor[idx].hex, hex);
            vetor[idx].F++;

            entrada_bytes[j] = (unsigned char)idx;
        }

        no *raiz = construir_arvore(vetor, 256);

        char **tabela = calloc(256, sizeof(char*));
        char codigo[256];

        gerar_codigos_huffman(raiz, codigo, 0, tabela);

        fprintf(saida, "Linha %d:\n", i + 1);
        imprimir_codigos(saida, tabela);

        char compactado_bits[100000];
        compactar(compactado_bits, entrada_bytes, qtd, tabela);

        unsigned char compactado_bytes[12500];
        int n_bytes = bits_para_bytes(compactado_bits, compactado_bytes);

        fprintf(saida, "\nCompactado (hex): ");
        for (int k = 0; k < n_bytes; k++) {
            fprintf(saida, "%02X ", compactado_bytes[k]);
        }
        fprintf(saida, "\n\n");

        for (int k = 0; k < 256; k++)
            free(tabela[k]);

        free(tabela);
        liberar_arvore(raiz);
        free(vetor);
        free(entrada_bytes);
    }

    fclose(entrada);
    fclose(saida);

    return 0;
}
