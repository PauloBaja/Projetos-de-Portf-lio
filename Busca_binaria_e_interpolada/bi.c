#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <stdint.h>

typedef struct {
    uint64_t ISBN;
    char obra[100], autor[100];
} livro;

char* formatastring(char str[]) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == ' ')
            str[i] = '_';
    }
    return str;
}

void merge(livro acervo[], livro* left, uint32_t n1, livro* right, uint32_t n2) {
    uint32_t i = 0, j = 0, k = 0;
    while (i < n1 && j < n2) {
        if (left[i].ISBN <= right[j].ISBN)
            acervo[k++] = left[i++];
        else
            acervo[k++] = right[j++];
    }
    while (i < n1) acervo[k++] = left[i++];
    while (j < n2) acervo[k++] = right[j++];
}

void ordenavetor(livro acervo[], uint32_t n) {
    if (n < 2) return;

    uint32_t mid = n / 2;
    livro* left = malloc(mid * sizeof(livro));
    livro* right = malloc((n - mid) * sizeof(livro));

    if (!left || !right) {
        printf("Erro ao alocar memória durante ordenação.\n");
        exit(1);
    }

    for (uint32_t i = 0; i < mid; i++) left[i] = acervo[i];
    for (uint32_t i = mid; i < n; i++) right[i - mid] = acervo[i];

    ordenavetor(left, mid);
    ordenavetor(right, n - mid);
    merge(acervo, left, mid, right, n - mid);

    free(left);
    free(right);
}

uint32_t heuristica(uint32_t i, uint32_t f, livro acervo[]) {
    return i + (acervo[f].ISBN - acervo[i].ISBN) % (f - i + 1);
}

int32_t buscainterpolada(livro acervo[], uint64_t nl, uint64_t x, uint32_t *passos) {
    uint32_t i = 0, f = nl - 1, eb = 0, pivo;

    while (i <= f) {
        if (acervo[f].ISBN == acervo[i].ISBN) {
            pivo = i; 
        } else {
            pivo = heuristica(i, f, acervo);
        }

        eb++; 

        if (acervo[pivo].ISBN == x) {
            *passos = eb;
            return pivo;
        } else if (acervo[pivo].ISBN < x) {
            i = pivo + 1;
        } else {
            if (pivo == 0) break; 
            f = pivo - 1;
        }
    }
    *passos = eb;
    return -1;
}





void buscalivro(FILE* saida, livro acervo[], uint64_t ISBN, uint64_t nl, uint32_t *sumsi) {
    uint32_t si = 0;

    int32_t ii = buscainterpolada(acervo, nl, ISBN, &si);
    *sumsi += si;

    if (ii == -1) {
        fprintf(saida, "[%" PRId64 "]|I=%u|ISBN_NOT_FOUND\n", ISBN, si);
        return;
    }

    livro l = acervo[ii];
    char autor_fmt[100], obra_fmt[100];
    strcpy(autor_fmt, l.autor);
    strcpy(obra_fmt, l.obra);
    formatastring(autor_fmt);
    formatastring(obra_fmt);

    fprintf(saida, "[%" PRId64 "]", l.ISBN);
    fprintf(saida, "|I=%u|", si);
    fprintf(saida, "Author:%s,", autor_fmt);
    fprintf(saida, "Title:%s\n", obra_fmt);
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

    uint64_t nl;
    if (fscanf(entrada, "%" PRId64 "\n", &nl) != 1) {
        printf("Erro ao ler número de livros.\n");
        fclose(entrada);
        fclose(saida);
        return 1;
    }

    livro *biblioteca = malloc(nl * sizeof(livro));
    if (!biblioteca) {
        printf("Erro ao alocar memória para biblioteca.\n");
        fclose(entrada);
        fclose(saida);
        return 1;
    }

    char linha[200];
    for (uint32_t i = 0; i < nl; i++) {
        if (fgets(linha, sizeof(linha), entrada)) {
            linha[strcspn(linha, "\n")] = '\0';

            char isbn_str[20];
            sscanf(linha, "%s", isbn_str);
            biblioteca[i].ISBN = strtoull(isbn_str, NULL, 10);

            char* de = strchr(linha, ' ');
            if (!de) continue;

            char* ia = de + 1;
            char* pe = strchr(ia, '&');
            if (!pe) continue;

            uint32_t autorlen = pe - ia;
            strncpy(biblioteca[i].autor, ia, autorlen);
            biblioteca[i].autor[autorlen] = '\0';

            strcpy(biblioteca[i].obra, pe + 1);
        }
    }

    ordenavetor(biblioteca, nl);

    uint32_t nb;
    if (fscanf(entrada, "%u\n", &nb) != 1) {
        printf("Erro ao ler número de buscas.\n");
        free(biblioteca);
        fclose(entrada);
        fclose(saida);
        return 1;
    }

    uint32_t sumsi = 0;
    uint32_t buscas_realizadas = 0;

    char buscas[30];
    uint64_t buscado;

    while (fgets(buscas, sizeof(buscas), entrada) != NULL) {
        if (sscanf(buscas, "%" PRIu64, &buscado) == 1) {
            buscalivro(saida, biblioteca, buscado, nl, &sumsi);
            buscas_realizadas++;
        }
    }

    fprintf(saida, "INTERPOLATION=%u:%u\n", buscas_realizadas, (buscas_realizadas ? sumsi / buscas_realizadas : 0));

    free(biblioteca);
    fclose(entrada);
    fclose(saida);
    return 0;
}
