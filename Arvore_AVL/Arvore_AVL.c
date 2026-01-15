#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
//Árove binária de decisao com fator balanceamento

typedef struct no {
    uint32_t H;
    struct no *D;
    struct no *E;
    char sinonimos[10][31]; 
    char senha[31];          
} No;


uint32_t maior(uint32_t a, uint32_t b) {
    return (a > b) ? a : b;
}

uint32_t altura(No* no) {
    return (no == NULL) ? 0 : no->H;
}

int fatorbalanceamento(No* no) {
    return (no == NULL) ? 0 : (int)altura(no->E) - (int)altura(no->D);
}

No* copia(char chave[31], char ls[10][31]) {
    No* novo = (No*)malloc(sizeof(No));
    if (!novo) {
        printf("Erro de alocação!\n");
        exit(1);
    }

    for (int i = 0; i < 10; i++) {
        strcpy(novo->sinonimos[i], ls[i]);
    }

    strcpy(novo->senha, chave);
    novo->H = 1;
    novo->D = NULL;
    novo->E = NULL;

    return novo;
}


No* rotacao_D(No* raiz) {
    No* eixo = raiz->E;
    raiz->E = eixo->D;
    eixo->D = raiz;

    raiz->H = maior(altura(raiz->E), altura(raiz->D)) + 1;
    eixo->H = maior(altura(eixo->E), altura(eixo->D)) + 1;

    return eixo;
}

No* rotacao_E(No* raiz) {
    No* eixo = raiz->D;
    raiz->D = eixo->E;
    eixo->E = raiz;

    raiz->H = maior(altura(raiz->E), altura(raiz->D)) + 1;
    eixo->H = maior(altura(eixo->E), altura(eixo->D)) + 1;

    return eixo;
}

No* rotacao_E_D(No* raiz) {
    raiz->E = rotacao_E(raiz->E);
    return rotacao_D(raiz);
}

No* rotacao_D_E(No* raiz) {
    raiz->D = rotacao_D(raiz->D);
    return rotacao_E(raiz);
}


No* balancear(No* raiz) {
    int fb = fatorbalanceamento(raiz);

    if (fb > 1 && fatorbalanceamento(raiz->E) >= 0)
        return rotacao_D(raiz);
    if (fb < -1 && fatorbalanceamento(raiz->D) <= 0)
        return rotacao_E(raiz);
    if (fb > 1 && fatorbalanceamento(raiz->E) < 0)
        return rotacao_E_D(raiz);
    if (fb < -1 && fatorbalanceamento(raiz->D) > 0)
        return rotacao_D_E(raiz);

    return raiz;
}

No* addno(No* raiz, char chave[31], char ls[10][31]) {
    if (raiz == NULL) return copia(chave, ls);

    if (strcmp(chave, raiz->senha) <= 0)
        raiz->E = addno(raiz->E, chave, ls);
    else
        raiz->D = addno(raiz->D, chave, ls);

    raiz->H = maior(altura(raiz->E), altura(raiz->D)) + 1;
    return balancear(raiz);
}

void busca(No* raiz, char buscado[30], FILE* saida) {
    if (raiz == NULL) {
        fprintf(saida, "?]\n");
        fprintf(saida, "-\n");
        return;
    }
    int cmp = strcmp(buscado, raiz->senha);
if (cmp == 0) {
    fprintf(saida, "%s]\n", raiz->senha);
    int printed = 0; 

    for (int i = 0; i < 10; i++) {
        if (strlen(raiz->sinonimos[i]) > 0) {
            if (printed > 0) {
                fprintf(saida, ",");
            }
            fprintf(saida, "%s", raiz->sinonimos[i]);
            printed++;
        }
    }

    fprintf(saida, "\n");
    return;
}


    if (cmp < 0) {
        fprintf(saida, "%s->", raiz->senha);
        busca(raiz->E, buscado, saida);
    } else {
        fprintf(saida, "%s->", raiz->senha);
        busca(raiz->D, buscado, saida);
    }
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

    No* dicionario = NULL;
    uint32_t np, ns,nb;
    char senha[31],buscado[31];
    char lista[10][31];

    fscanf(entrada, "%u", &np);

    for (uint32_t i = 0; i < np; i++) {
        fscanf(entrada, "%s %u", senha, &ns);

        for (uint32_t j = 0; j < ns && j < 10; j++) {
            fscanf(entrada, "%s", lista[j]);
        }

        for (uint32_t j = ns; j < 10; j++) {
            strcpy(lista[j], "");
        }

        dicionario = addno(dicionario, senha, lista);
    }


    fscanf(entrada,"%u", &nb);
    for(uint32_t i =0; i<nb;i++){
        fscanf(entrada,"%s",buscado);
        fprintf(saida, "[");
        busca(dicionario,buscado,saida);
    }
    fclose(entrada);
    fclose(saida);
    return 0;
}
