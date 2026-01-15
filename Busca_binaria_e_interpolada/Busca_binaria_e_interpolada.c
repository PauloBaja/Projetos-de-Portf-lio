#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <stdint.h>

typedef struct {
    uint64_t ISBN;
    char *obra;
    char *autor;
} livro;

static inline void formatastring(char str[]) {
    for (int i = 0; str[i]; i++)
        if (str[i] == ' ') str[i] = '_';
}

void ordenavetor(livro acervo[], livro aux[], uint32_t inicio, uint32_t fim) {
    if (fim - inicio < 2) return;

    uint32_t mid = (inicio + fim) / 2;
    ordenavetor(acervo, aux, inicio, mid);
    ordenavetor(acervo, aux, mid, fim);

    uint32_t i = inicio, j = mid, k = inicio;
    while (i < mid && j < fim) {
        if (acervo[i].ISBN <= acervo[j].ISBN)
            aux[k++] = acervo[i++];
        else
            aux[k++] = acervo[j++];
    }
    while (i < mid) aux[k++] = acervo[i++];
    while (j < fim) aux[k++] = acervo[j++];

    for (uint32_t i = inicio; i < fim; i++) acervo[i] = aux[i];
}

int32_t buscabinaria(livro acervo[], uint64_t nl, uint64_t x, uint32_t *steps) {
    uint32_t i = 0, f = nl - 1, eb = 0, pivo;

    while (i <= f) {
        pivo = (i + f) / 2;
        if (acervo[pivo].ISBN == x) {
            *steps = eb + 1;
            return pivo;
        } else if (x < acervo[pivo].ISBN) {
            eb++;
            f = pivo - 1;
        } else {
            eb++;
            i = pivo + 1;
        }
    }
    *steps = eb + 1;
    return -1;
}

uint32_t heuristica(uint32_t i, uint32_t f, livro acervo[]) {
    return i + (acervo[f].ISBN - acervo[i].ISBN) % (f - i + 1);
}

int32_t buscainterpolada(livro acervo[], uint64_t nl, uint64_t x, uint32_t *steps) {
    uint32_t i = 0, f = nl - 1, eb = 0, pivo;

    while (i <= f) {
        if (acervo[f].ISBN == acervo[i].ISBN) {
            pivo = i;
        } else {
            pivo = heuristica(i, f, acervo);
        }

        eb++;

        if (acervo[pivo].ISBN == x) {
            *steps = eb;
            return pivo;
        } else if (acervo[pivo].ISBN < x) {
            i = pivo + 1;
        } else {
            if (pivo == 0) break;
            f = pivo - 1;
        }
    }
    *steps = eb;
    return -1;
}

void buscalivro(FILE* saida, livro acervo[], uint64_t ISBN, uint64_t nl, uint32_t *vb, uint32_t *vi, uint32_t *sumsb, uint32_t *sumsi) {
    uint32_t sb = 0, si = 0;
    int32_t ib = buscabinaria(acervo, nl, ISBN, &sb);
    int32_t ii = buscainterpolada(acervo, nl, ISBN, &si);

    (*sumsb) += sb;
    (*sumsi) += si;

    if (sb < si) (*vb)++;
    else (*vi)++;

    if (ib == -1 || ii == -1 || ib != ii) {
        fprintf(saida, "[%" PRId64 "]B=%u|I=%u|ISBN_NOT_FOUND\n", ISBN, sb, si);
        return;
    }

    livro l = acervo[ib];
    char autor_fmt[100], obra_fmt[100];
    strncpy(autor_fmt, l.autor, sizeof(autor_fmt) - 1);
    strncpy(obra_fmt, l.obra, sizeof(obra_fmt) - 1);
    autor_fmt[sizeof(autor_fmt) - 1] = '\0';
    obra_fmt[sizeof(obra_fmt) - 1] = '\0';
    formatastring(autor_fmt);
    formatastring(obra_fmt);

    fprintf(saida, "[%" PRId64 "]B=%u|I=%u|Author:%s,Title:%s\n", l.ISBN, sb, si, autor_fmt, obra_fmt);
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
    fscanf(entrada, "%" SCNu64 "\n", &nl);

    livro *biblioteca = malloc(nl * sizeof(livro));
    livro *aux = malloc(nl * sizeof(livro));
    if (!biblioteca || !aux) {
        printf("Erro ao alocar memória.\n");
        fclose(entrada);
        fclose(saida);
        return 1;
    }

    char linha[200];
    for (uint64_t i = 0; i < nl; i++) {
        if (fgets(linha, sizeof(linha), entrada)) {
            linha[strcspn(linha, "\n")] = '\0';

            char *p = linha;
            while (*p && *p != ' ') p++;
            if (!*p) continue;
            *p++ = '\0';

            biblioteca[i].ISBN = strtoull(linha, NULL, 10);

            char *sep = strchr(p, '&');
            if (!sep) continue;

            *sep = '\0';
            biblioteca[i].autor = strdup(p);
            biblioteca[i].obra = strdup(sep + 1);
        }
    }

    ordenavetor(biblioteca, aux, 0, nl);

    uint32_t vb = 0, vi = 0, sumsb = 0, sumsi = 0;
    uint32_t nb;
    fscanf(entrada, "%u\n", &nb);

    char buscas[30];
    uint64_t buscado;

    while (fgets(buscas, sizeof(buscas), entrada) != NULL) {
        if (sscanf(buscas, "%" SCNu64, &buscado) == 1) {
            buscalivro(saida, biblioteca, buscado, nl, &vb, &vi, &sumsb, &sumsi);
        } else {
            break;
        }
    }

    fprintf(saida, "BINARY=%u:%u\n", vb, sumsb / nb);
    fprintf(saida, "INTERPOLATION=%u:%u\n", vi, sumsi / nb);

    for (uint64_t i = 0; i < nl; i++) {
        free(biblioteca[i].autor);
        free(biblioteca[i].obra);
    }



    
    free(biblioteca);
    free(aux);
    fclose(entrada);
    fclose(saida);

    return 0;
}
