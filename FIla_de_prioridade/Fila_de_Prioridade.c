#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
void formataString(char *str) {
    int i = 0, j = 0;
    int espaco = 0;

    while (str[i]) {
        if (str[i] != ' ') {
            if (espaco && j > 0) {
                str[j++] = '_';
            }
            str[j++] = str[i];
            espaco = 0;
        } else {
            espaco = 1;
        }
        i++;
    }

    str[j] = '\0';
}


typedef struct no {
    uint8_t prioridade;
    char nome[50];
    struct no *prox;
} cliente;

typedef struct orgao {
    uint32_t capacidade, tam1, tam0;
    char nome[50];
    cliente *fila0; 
    cliente *fim0;
    cliente *fila1;
    cliente *fim1;
} org;

cliente *criacliente(char *nome, uint8_t prioridade,org *inst) {
    cliente *novo = malloc(sizeof(cliente));
    if (!novo) {
        fprintf(stderr,"Erro: falha na alocação de memória\n");
        return NULL;
    }
    strncpy(novo->nome, nome, sizeof(novo->nome) - 1);

    novo->nome[sizeof(novo->nome) - 1] = '\0';

    novo->prioridade = prioridade;
    novo->prox = NULL;
    return novo;
}
void preencherorgaoArquivo(FILE *entrada, org *inst) {
    fscanf(entrada, "%s %u", inst->nome, &inst->capacidade);
}

void addfila(cliente **fila, cliente **fim, cliente *novo,uint32_t *tam) {
    if (*fim == NULL) {
        *fila = *fim = novo;
    } else {
        (*fim)->prox = novo;
        *fim = novo;
    }
    novo->prox = NULL;
    (*tam)++;
}

cliente* desfila(cliente **fila, cliente **fim, uint32_t *tam) {
    if (!*fila) return NULL;

    cliente *removido = *fila;
    *fila = removido->prox;

    if (!*fila) *fim = NULL;

    removido->prox = NULL;
    (*tam)--;

    return removido;
}


void addprioritario(cliente *x,org * inst) {
    if (x->prioridade) {
        addfila(&inst->fila1, &inst->fim1, x, &inst->tam1); 
    } else {
        addfila(&inst->fila0,&inst->fim0, x,&inst->tam0);
    }
}

void imprimirAtendimentoTodos(FILE *saida, org *orgaos, uint32_t norgaos) {
    while (1) {
        int houveAtendimento = 0;

        for (uint32_t i = 0; i < norgaos; i++) {
            org *o = &orgaos[i];
            int houveAtendimentoOrgao = 0;

            char linha[512] = {0};
            char buffer[60];
            int primeiro = 1;

            for (uint32_t j = 0; j < o->capacidade; j++) {
                cliente *c = NULL;

                if (o->fila1) {
                    c = desfila(&o->fila1, &o->fim1, &o->tam1);
                } else if (o->fila0) {
                    c = desfila(&o->fila0, &o->fim0, &o->tam0);
                }

                if (c) {
                    if (!primeiro) {
                        strcat(linha, ",");
                    }
                    snprintf(buffer, sizeof(buffer), "%s", c->nome);
                    strcat(linha, buffer);
                    houveAtendimentoOrgao = 1;
                    houveAtendimento = 1;
                    free(c);
                    primeiro = 0;
                }
            }

            if (houveAtendimentoOrgao) {
                fprintf(saida, "%s:%s\n", o->nome, linha);
            }
        }

        if (!houveAtendimento) break;
    }
}




org *buscarOrgaoPorNome(org *lista, uint32_t n, const char *nome) {
    for (uint32_t i = 0; i < n; i++) {
        if (strcmp(lista[i].nome, nome) == 0) {
            return &lista[i];
        }
    }
    return NULL;
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

    uint32_t  norgaos=0;
    

    fscanf(entrada,"%u",&norgaos);

    org *orgaos=malloc(sizeof(org)*norgaos);

    for (uint32_t i = 0; i < norgaos; i++) {
        orgaos[i].fim0=NULL;
        orgaos[i].fim1=NULL;
        orgaos[i].fila0=NULL;
        orgaos[i].fila1=NULL;
        preencherorgaoArquivo(entrada, &orgaos[i]);
    }
    
   uint32_t nc;
fscanf(entrada, "%u", &nc);

char linha[256];
for (uint32_t i = 0; i < nc; i++) {
    if (!fgets(linha, sizeof(linha), entrada)) break;

    linha[strcspn(linha, "\n")] = '\0';

    char *orgaoStr = strtok(linha, "|");
    char *nomeStr = strtok(NULL, "|");
    char *idadeStr = strtok(NULL, "|");

    if (!orgaoStr || !nomeStr || !idadeStr) continue;

    formataString(nomeStr);
    uint32_t idade = atoi(idadeStr);

    cliente *novo = malloc(sizeof(cliente));
    if (!novo) {
        perror("Erro de alocação");
        exit(1);
    }

    strcpy(novo->nome, nomeStr);
    novo->prioridade = (idade >= 60) ? 1 : 0;
    novo->prox = NULL;

    org *destino = buscarOrgaoPorNome(orgaos, norgaos, orgaoStr);
    if (destino == NULL) {
        fprintf(stderr, "Órgão '%s' não encontrado. Ignorando cliente %s.\n", orgaoStr, nomeStr);
        free(novo);
        continue;
    }

    addprioritario(novo, destino);
}

    imprimirAtendimentoTodos(saida,orgaos,norgaos);

    return 0;
}
