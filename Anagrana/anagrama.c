#include <stdio.h>
#include <string.h>

typedef struct {
    char string[100];
    int letras[26];
} palavra;

void conta(palavra *p, long *aux) {
    memset(p->letras, 0, sizeof(p->letras));

    for (int i = 0; p->string[i] != '\0'; i++) {
        (*aux)++;  // conta a iteração
        char c = p->string[i];
        p->letras[c - 'a']++;
    }
}

int anagrama(palavra *a, palavra *b, long *aux) {
    for (int i = 0; i < 26; i++) {
        (*aux)++;  // conta a iteração
        if (a->letras[i] != b->letras[i]) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s entrada saida\n", argv[0]);
        return 1;
    }

    FILE *entrada = fopen(argv[1], "r");
    FILE *saida   = fopen(argv[2], "w");

    if (!entrada || !saida) {
        printf("Erro ao abrir arquivos\n");
        return 1;
    }

    int N;
    fscanf(entrada, "%d", &N);

    palavra a, b;
    long tt=0;
    for (int i = 0; i < N; i++) {

        long aux = 0;  // zera contador a cada par de palavras

        fscanf(entrada, "%s %s", a.string, b.string);

     

        conta(&a, &aux);
        conta(&b, &aux);

        if (anagrama(&a, &b, &aux)) {
            fprintf(saida, "%s e %s sao anagramas (iteracoes=%ld)\n", a.string, b.string, aux);
        } else {
            fprintf(saida, "%s e %s nao sao anagramas (iteracoes=%ld)\n", a.string, b.string, aux);
        }
            tt+=aux;
    }
    fprintf(saida, "TOTAL DE ITERACOES %ld\n",  tt);

    fclose(entrada);
    fclose(saida);
    return 0;
}
