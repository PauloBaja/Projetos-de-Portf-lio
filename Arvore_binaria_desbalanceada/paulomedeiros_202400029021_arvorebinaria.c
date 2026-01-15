#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct A {
    char nome[51];
    char permissao[3]; 
    uint32_t len,ordem;
    struct A *e;
    struct A *d;
} arvore;

arvore* add(arvore *raiz, char nn[50], char p[3], uint32_t tamanho, uint32_t posicao) {
    arvore *novo = (arvore*) malloc(sizeof(arvore));
    if (novo == NULL) {
        printf("Erro de alocação\n");
        exit(1);
    }

    strcpy(novo->nome, nn);
    strcpy(novo->permissao, p);
    novo->len = tamanho;
    novo->ordem = posicao;
    novo->e = NULL;
    novo->d = NULL;

    if (raiz == NULL)
        return novo;

    arvore *atual = raiz;
    arvore *pai = NULL;

    while (atual != NULL) {
        pai = atual;
        int cmp = strcmp(nn, atual->nome);
        if (cmp < 0) {
            atual = atual->e;
        } else if (cmp > 0) {
            atual = atual->d;
        } else { 
            if (strcmp(atual->permissao, "rw") == 0) {
                strcpy(atual->permissao, p);
                strcpy(atual->nome, nn);
                atual->len = tamanho;
                atual->ordem = posicao;
            }
            free(novo); 
            return raiz;
        }
    }

    int cmp = strcmp(nn, pai->nome);
    if (cmp < 0)
        pai->e = novo;
    else
        pai->d = novo;

    return raiz;
}


void emordem(arvore *raiz, uint32_t qnt,FILE *out) {
    fprintf(out,"[EPD]\n");
    arvore **pilha = malloc(sizeof(arvore*) * qnt);
    if (!pilha) {
        printf("Erro de alocação da pilha\n");
        exit(1);
    }
    int topo = -1;
    arvore *atual = raiz;

    while (atual != NULL || topo != -1) {
        while (atual != NULL) {
            if (topo + 1 >= (int)qnt) {
                printf("Estouro da pilha em emordem\n");
                free(pilha);
                exit(1);
            }
            pilha[++topo] = atual;
            atual = atual->e;
        }

        atual = pilha[topo--];
        fprintf(out,"%u:%s|%s|%u_bytes\n", atual->ordem,atual->nome,atual->permissao,atual->len);
        atual = atual->d;
    }
    free(pilha);
}

void preordem(arvore *raiz, uint32_t qnt,FILE *out) {
        fprintf(out,"[PED]\n");
    arvore **pilha = malloc(sizeof(arvore*) * qnt);
    if (!pilha) {
        printf("Erro de alocacao da pilha\n");
        exit(1);
    }
    int topo = -1;
    arvore *atual = raiz;

    while (atual != NULL || topo != -1) {
        while (atual != NULL) {
        fprintf(out,"%u:%s|%s|%u_bytes\n", atual->ordem,atual->nome,atual->permissao,atual->len);
            if (topo + 1 >= (int)qnt) {
                printf("Estouro da pilha em preordem\n");
                free(pilha);
                exit(1);
            }
            pilha[++topo] = atual;
            atual = atual->e;
        }

        atual = pilha[topo--];
        atual = atual->d;
    }

    free(pilha);
}

void posordem(arvore *raiz, uint32_t qnt, FILE *out) {
        fprintf(out,"[EDP]\n");

    if (raiz == NULL) return;

    arvore **pilha1 = malloc(sizeof(arvore*) * qnt);
    arvore **pilha2 = malloc(sizeof(arvore*) * qnt);
    if (!pilha1 || !pilha2) {
        printf("Erro de alocação das pilhas em posordem\n");
        free(pilha1);
        free(pilha2);
        exit(1);
    }

    int topo1 = -1, topo2 = -1;
    pilha1[++topo1] = raiz;

    while (topo1 != -1) {
        arvore *atual = pilha1[topo1--];
        pilha2[++topo2] = atual;

        if (atual->e)
            pilha1[++topo1] = atual->e;
        if (atual->d)
            pilha1[++topo1] = atual->d;
    }

    while (topo2 != -1) {
    arvore *no = pilha2[topo2--];
    fprintf(out,"%u:%s|%s|%u_bytes\n", no->ordem, no->nome, no->permissao, no->len);
}

    free(pilha1);
    free(pilha2);
}


void liberar(arvore *raiz) {
    if (raiz == NULL) return;
    liberar(raiz->e);
    liberar(raiz->d);
    free(raiz);
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

    uint32_t nd;
    fscanf(entrada, "%u", &nd);

    arvore *arv = NULL;
    char n[51], per[3];
    uint32_t tam, acc = 0;

    while (fscanf(entrada, "%s %s %u", n, per, &tam) == 3) {
        arv = add(arv, n, per, tam, acc);
        acc++;
    }

    emordem(arv, nd, saida);
    preordem(arv, nd, saida);
    posordem(arv, nd, saida);

    liberar(arv);
    fclose(entrada);
    fclose(saida);

    return 0;
}

