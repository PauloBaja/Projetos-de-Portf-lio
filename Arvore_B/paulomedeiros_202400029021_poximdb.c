#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct No {
    int m; 
    struct No *pont_pai;

    char **hash;   
    char **nome;    
    uint32_t *tamanho;

    struct No **p;   
} TNo;

TNo* criarNo(uint32_t ordem) {
    TNo *no = malloc(sizeof(TNo));
    no->m = 0;
    no->pont_pai = NULL;

    no->hash = malloc(sizeof(char*) * (ordem - 1));
    no->nome = malloc(sizeof(char*) * (ordem - 1));
    no->tamanho = malloc(sizeof(uint32_t) * (ordem - 1));

    for (uint32_t i = 0; i < ordem - 1; i++) {
        no->hash[i] = calloc(33, sizeof(char));
        no->nome[i] = calloc(31, sizeof(char));
        no->tamanho[i] = 0;
    }

    no->p = calloc(ordem, sizeof(TNo*));

    return no;
}

TNo *busca(FILE* saida, TNo *no, const char *ch) {
    if (no != NULL) {
        int i = 0;
        while (i < no->m && strcmp(ch, no->hash[i]) > 0) i++;
        if (i < no->m && strcmp(ch, no->hash[i]) == 0) {
                fprintf(saida,"[%s]\n",ch);
            for (uint32_t k = 0; k < no->m; k++) {
                fprintf(saida, "%s:size=%u,hash=%s\n",  no->nome[k],no->tamanho[k],no->hash[k]);
            }
            return no;
        } else if (no->p[i] != NULL) {
            return busca(saida, no->p[i], ch);
        } else {
            fprintf(saida,"[%s]\n",ch);
            fprintf(saida, "-\n");
            return NULL;
        }
    }
    fprintf(saida, "-\n");
    return NULL;
}

void inserirOrdenadoNo(TNo *no, const char *ch, const char *nome, uint32_t tam) {
    int i = no->m - 1;
    while (i >= 0 && strcmp(ch, no->hash[i]) < 0) {
        strcpy(no->hash[i + 1], no->hash[i]);
        strcpy(no->nome[i + 1], no->nome[i]);
        no->tamanho[i + 1] = no->tamanho[i];
        i--;
    }
    strcpy(no->hash[i + 1], ch);
    strcpy(no->nome[i + 1], nome);
    no->tamanho[i + 1] = tam;
    no->m++;
}

TNo* dividirNoSimples(TNo *no, const char ch[33], const char nome[31], uint32_t tam, TNo *novoFilho, uint32_t ordem, TNo **nova_raiz) {
    int i, j;

    char **temp_hash = malloc(sizeof(char*) * ordem);
    char **temp_nome = malloc(sizeof(char*) * ordem);
    uint32_t *temp_tam = malloc(sizeof(uint32_t) * ordem);
    TNo *temp_p[ordem + 1];
    for (i = 0; i <= ordem; i++) temp_p[i] = NULL;

    for (i = 0; i < ordem; i++) {
        temp_hash[i] = malloc(33);
        temp_nome[i] = malloc(31);
    }

    for (i = 0; i < no->m; i++) {
        strcpy(temp_hash[i], no->hash[i]);
        strcpy(temp_nome[i], no->nome[i]);
        temp_tam[i] = no->tamanho[i];
    }
    for (i = 0; i <= no->m; i++) {
        temp_p[i] = no->p[i];
    }

    i = 0;
    while (i < no->m && strcmp(ch, temp_hash[i]) > 0) i++;
    for (j = no->m; j > i; j--) {
        strcpy(temp_hash[j], temp_hash[j - 1]);
        strcpy(temp_nome[j], temp_nome[j - 1]);
        temp_tam[j] = temp_tam[j - 1];
        temp_p[j + 1] = temp_p[j];
    }

    strcpy(temp_hash[i], ch);
    strcpy(temp_nome[i], nome);
    temp_tam[i] = tam;
    temp_p[i + 1] = novoFilho;

    int meio = ordem / 2;
    char chave_promovida[33], nome_promovido[31];
    uint32_t tam_promovido;
    strcpy(chave_promovida, temp_hash[meio]);
    strcpy(nome_promovido, temp_nome[meio]);
    tam_promovido = temp_tam[meio];

    no->m = 0;
    for (i = 0; i < meio; i++) {
        strcpy(no->hash[i], temp_hash[i]);
        strcpy(no->nome[i], temp_nome[i]);
        no->tamanho[i] = temp_tam[i];
        no->p[i] = temp_p[i];
        no->m++;
    }
    no->p[i] = temp_p[i];

    TNo *dir = criarNo(ordem);
    dir->pont_pai = no->pont_pai;
    for (i = meio + 1, j = 0; i < ordem; i++, j++) {
        strcpy(dir->hash[j], temp_hash[i]);
        strcpy(dir->nome[j], temp_nome[i]);
        dir->tamanho[j] = temp_tam[i];
        dir->p[j] = temp_p[i];
        dir->m++;
    }
    dir->p[j] = temp_p[ordem];

    for (i = 0; i <= dir->m; i++) {
        if (dir->p[i]) dir->p[i]->pont_pai = dir;
    }
    for (i = 0; i <= no->m; i++) {
        if (no->p[i]) no->p[i]->pont_pai = no;
    }

    for (i = 0; i < ordem; i++) {
        free(temp_hash[i]);
        free(temp_nome[i]);
    }
    free(temp_hash);
    free(temp_nome);
    free(temp_tam);

    if (no->pont_pai == NULL) {
        TNo *nova = criarNo(ordem);
        strcpy(nova->hash[0], chave_promovida);
        strcpy(nova->nome[0], nome_promovido);
        nova->tamanho[0] = tam_promovido;
        nova->m = 1;
        nova->p[0] = no;
        nova->p[1] = dir;
        no->pont_pai = nova;
        dir->pont_pai = nova;
        *nova_raiz = nova;
    } else {
        TNo *pai = no->pont_pai;
        if (pai->m < ordem - 1) {
            i = pai->m - 1;
            while (i >= 0 && strcmp(chave_promovida, pai->hash[i]) < 0) {
                strcpy(pai->hash[i + 1], pai->hash[i]);
                strcpy(pai->nome[i + 1], pai->nome[i]);
                pai->tamanho[i + 1] = pai->tamanho[i];
                pai->p[i + 2] = pai->p[i + 1];
                i--;
            }
            strcpy(pai->hash[i + 1], chave_promovida);
            strcpy(pai->nome[i + 1], nome_promovido);
            pai->tamanho[i + 1] = tam_promovido;
            pai->p[i + 2] = dir;
            dir->pont_pai = pai;
            pai->m++;
            *nova_raiz = NULL;
        } else {
            TNo *nova_aux = NULL;
            dividirNoSimples(pai, chave_promovida, nome_promovido, tam_promovido, dir, ordem, &nova_aux);
            if (nova_aux) *nova_raiz = nova_aux;
        }
    }

    return *nova_raiz ? *nova_raiz : no;
}

TNo* addNo(TNo *raiz, const char ch[33], const char nome[31], uint32_t tam, uint32_t ordem) {
    if (!raiz) return criarNo(ordem);
    TNo *no = raiz;

    while (no->p[0] != NULL) {
        int i = 0;
        while (i < no->m && strcmp(ch, no->hash[i]) > 0) i++;
        no = no->p[i];
    }

    if (no->m < ordem - 1) {
        inserirOrdenadoNo(no, ch, nome, tam);
        return raiz;
    }

    TNo *nova_raiz = NULL;
    dividirNoSimples(no, ch, nome, tam, NULL, ordem, &nova_raiz);
    return nova_raiz ? nova_raiz : raiz;
}

void liberarNo(TNo* no, uint32_t ordem) {
    if (!no) return;
    for (uint32_t i = 0; i <= no->m; i++) {
        liberarNo(no->p[i], ordem);
    }
    for (uint32_t i = 0; i < ordem - 1; i++) {
        free(no->hash[i]);
        free(no->nome[i]);
    }
    free(no->hash);
    free(no->nome);
    free(no->tamanho);
    free(no->p);
    free(no);
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

    TNo *arvore = NULL;
    uint32_t ordem, n, nc, size;
    char name[31], codigo[33], comando[7];

    fscanf(entrada, "%u", &ordem);
    if (ordem < 3) {
        fprintf(stderr, "Ordem inválida.\n");
        return 1;
    }

    fscanf(entrada, "%u", &n);
    arvore = criarNo(ordem);
    for (uint32_t i = 0; i < n; i++) {
        if (fscanf(entrada, "%s %u %s", name, &size, codigo) != 3) break;
        arvore = addNo(arvore, codigo, name, size, ordem);
    }

    fscanf(entrada, "%u", &nc);
    for (uint32_t i = 0; i < nc; i++) {
        if (fscanf(entrada, "%s", comando) != 1) break;
        if (strcmp("INSERT", comando) == 0) {
            if (fscanf(entrada, "%s %u %s", name, &size, codigo) != 3) break;
            arvore = addNo(arvore, codigo, name, size, ordem);
        } else {
            if (fscanf(entrada, "%s", codigo) != 1) break;
            busca(saida, arvore, codigo);
        }
    }

    liberarNo(arvore, ordem);
    fclose(entrada);
    fclose(saida);
    return 0;
}
