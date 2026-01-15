#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

typedef struct conteiner {
    char *codigo;
    char *CNPJ;
    char *F;
    int peso, oi, diff,ih;
} conteiner;

uint32_t H1(const char *string, uint32_t N) {
    uint32_t hash = 17;
    for (size_t i = 0; string[i]; i++) {
        if (string[i] != ' ')
            hash = (hash * 100003 + (unsigned char)string[i]) % N;
    }
    return hash;
}
uint32_t H2(const char *string, uint32_t N) {
    uint32_t hash = 19;
    for (size_t i = 0; string[i]; i++) {
        if (string[i] != ' ')
            hash = (hash * 100019 + (unsigned char)string[i]) % N;
    }
    return hash;
}


conteiner *inicializarregistro(uint32_t N) {
    conteiner *registro = calloc(N, sizeof(conteiner));
    if (!registro) return NULL;
    return registro;
}

void copiaconteiner(conteiner *destino, conteiner *origem) {
    if (!destino || !origem) return;
    destino->codigo = origem->codigo ? strdup(origem->codigo) : NULL;
    destino->CNPJ = origem->CNPJ ? strdup(origem->CNPJ) : NULL;
    destino->F = origem->F ? strdup(origem->F) : NULL;
    destino->peso = origem->peso;
    destino->oi = origem->oi;
    destino->diff = origem->diff;
}

void addhash(conteiner *registro, conteiner c, int N) {
    if (!c.codigo || !c.CNPJ) return;
    uint32_t h1 = H1(c.codigo, N);
    uint32_t h2 = H2(c.codigo, N);
    for (int i = 0; i < N; i++) {
        uint32_t idx = (h1 + i*h2) % N;
        
        if (registro[idx].codigo == NULL) {
            copiaconteiner(&registro[idx], &c);
            return;
        }
    }
}

void addvetor(conteiner *lista, conteiner c, int pos) {
    lista[pos] = c;
}

void verificacaoselecionado(conteiner *registro, conteiner *doc, conteiner *pesado,
                             const char *nome, const char *cnpjoto, const int pe,
                             int N, int *pc, int *pp) {
    if (!nome || !cnpjoto) return;
        uint32_t h1 = H1(nome, N);
        uint32_t h2 = H2(nome, N);
        for (int i = 0; i < N; i++) {
        uint32_t idx = (h1 + i * h2) % N;
        if (registro[idx].codigo) {
            if (strcmp(nome, registro[idx].codigo) == 0) {
                if (strcmp(registro[idx].CNPJ, cnpjoto) != 0) {
                    free(registro[idx].F);
                    registro[idx].F = strdup(cnpjoto);
                    addvetor(doc, registro[idx], (*pc)++);
                    return;
                }
                if(pe > registro[idx].peso * 1.1 || pe < registro[idx].peso * 0.9) {
                    registro[idx].diff = abs(pe - registro[idx].peso);
                    addvetor(pesado, registro[idx], (*pp)++);
                    return;
                }
                return;
            }
        }
    }
}

void copiar(conteiner *final, conteiner *inicial, int n) {
    if (n <= 0) return;
    for (int k = 0; k < n; k++) final[k] = inicial[k];
}

void intercalarcnpj(conteiner *aux, conteiner *doc, int i, int m, int j) {
    int i1 = i, i2 = m + 1, k = i;
    while (i1 <= m && i2 <= j)
        aux[k++] = (doc[i1].oi <= doc[i2].oi) ? doc[i1++] : doc[i2++];
    if (i1 <= m) copiar(aux + k, doc + i1, m - i1 + 1);
    if (i2 <= j) copiar(aux + k, doc + i2, j - i2 + 1);
    copiar(doc + i, aux + i, j - i + 1);
}

void mergesortcnpj(conteiner *aux, conteiner *doc, int i, int j) {
    if (i < j) {
        int m = i + (j - i) / 2;
        mergesortcnpj(aux, doc, i, m);
        mergesortcnpj(aux, doc, m + 1, j);
        intercalarcnpj(aux, doc, i, m, j);
    }
}

void intercalarpeso(conteiner *aux, conteiner *pesado, int i, int m, int j) {
    int i1 = i, i2 = m + 1, k = i;
    
    while (i1 <= m && i2 <= j) {
        int percent1 = 0, percent2 = 0;
        
        if (pesado[i1].peso > 0) {
            float calc1 = (pesado[i1].diff * 100.0f) / pesado[i1].peso;
            percent1 = (int)(calc1 + 0.5f);  
        }
        
        if (pesado[i2].peso > 0) {
            float calc2 = (pesado[i2].diff * 100.0f) / pesado[i2].peso;
            percent2 = (int)(calc2 + 0.5f);  
        }
        
        if (percent1 > percent2) {
            aux[k++] = pesado[i1++];  
        } else if (percent1 < percent2) {
            aux[k++] = pesado[i2++];    
        } else {
            aux[k++] = (pesado[i1].oi < pesado[i2].oi) ? pesado[i1++] : pesado[i2++];
        }
    }
    
    if (i1 <= m) copiar(aux + k, pesado + i1, m - i1 + 1);
    if (i2 <= j) copiar(aux + k, pesado + i2, j - i2 + 1);
    copiar(pesado + i, aux + i, j - i + 1);
}

void mergesortpeso(conteiner *aux, conteiner *pesado, int i, int j) {
    if (i < j) {
        int m = i + (j - i) / 2;
        mergesortpeso(aux, pesado, i, m);
        mergesortpeso(aux, pesado, m + 1, j);
        intercalarpeso(aux, pesado, i, m, j);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) { printf("Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]); return 1; }
    FILE *entrada = fopen(argv[1], "r");
    if (!entrada) { perror("Erro ao abrir arquivo de entrada"); return 1; }
    FILE *saida = fopen(argv[2], "w");
    if (!saida) { perror("Erro ao abrir arquivo de saída"); fclose(entrada); return 1; }

    int N, I;
    if (fscanf(entrada, "%d", &N) != 1) { fprintf(stderr, "Erro ao ler N.\n"); return 1; }

    int M = 2*N;
    conteiner *lista = inicializarregistro(M);
    conteiner *pesado = calloc(N, sizeof(conteiner));
    conteiner *doc = calloc(N, sizeof(conteiner));
    conteiner *AUX = calloc(N, sizeof(conteiner));

    for (int i = 0; i < N; i++) {
        conteiner aux;
        aux.codigo = malloc(13);
        aux.CNPJ = malloc(20);
        aux.F = NULL;
        aux.oi = i;
        if (fscanf(entrada, "%12s %19s %d", aux.codigo, aux.CNPJ, &aux.peso) != 3) {
            free(aux.codigo); free(aux.CNPJ); continue;
        }
        addhash(lista, aux, M);
    }

    if (fscanf(entrada, "%d", &I) != 1) I = 0;
    int pc = 0, pp = 0;

    for (int i = 0; i < I; i++) {
        char cb[13], cnpjb[20]; int pb;
        if (fscanf(entrada, "%12s %19s %d", cb, cnpjb, &pb) != 3) continue;
        verificacaoselecionado(lista, doc, pesado, cb, cnpjb, pb, M, &pc, &pp);
    }

    if (pc > 1) mergesortcnpj(AUX, doc, 0, pc - 1);
    for (int acc = 0; acc < pc; acc++)
        fprintf(saida, "%s:%s<->%s\n", doc[acc].codigo, doc[acc].CNPJ, doc[acc].F ? doc[acc].F : "");

    if (pp > 1) mergesortpeso(AUX, pesado, 0, pp - 1);
    for (int acc = 0; acc < pp; acc++) {
        int percent = 0;
        if (pesado[acc].peso > 0) {
            float calc = (pesado[acc].diff * 100.0f) / pesado[acc].peso;
            percent = (int)(calc + 0.5f);
        }
        
        if (percent >= 11) {
            fprintf(saida, "%s:%dkg(%d%%)\n", pesado[acc].codigo, pesado[acc].diff, percent);
        }
    }

    fclose(entrada); fclose(saida);

    for (int i = 0; i < M; i++) {
        free(lista[i].codigo); free(lista[i].CNPJ); free(lista[i].F);
    }
    free(lista); free(pesado); free(doc); free(AUX);
    return 0;
}