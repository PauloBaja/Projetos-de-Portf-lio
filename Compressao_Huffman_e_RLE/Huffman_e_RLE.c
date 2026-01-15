#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct no {
    int F;
    unsigned char byte;
    struct no *E, *D;
} no;

typedef struct {
    unsigned long long bits;
    unsigned char tam;
} codigo;



typedef struct {
    no **heap;
    int tamanho;
    int capacidade;
} filaPrioridade;



no *criar_no(unsigned char byte, int freq) {
    no *n = (no *)malloc(sizeof(no));
    n->byte = byte;
    n->F = freq;
    n->E = n->D = NULL;
    return n;
}

void trocar(no **a, no **b) {
    no *t = *a;
    *a = *b;
    *b = t;
}

void heapifyMin(no **heap, int fim, int i) {
    int menor = i;
    int esq = 2 * i + 1;
    int dir = 2 * i + 2;

    if (esq <= fim && heap[esq]->F < heap[menor]->F)
        menor = esq;
    if (dir <= fim && heap[dir]->F < heap[menor]->F)
        menor = dir;

    if (menor != i) {
        trocar(&heap[i], &heap[menor]);
        heapifyMin(heap, fim, menor);
    }
}


filaPrioridade *fp_criar(int *freq) {
    filaPrioridade *fp = malloc(sizeof(filaPrioridade));
    fp->heap = malloc(sizeof(no*) * 256);
    fp->tamanho = 0;
    fp->capacidade = 256;

    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0)
            fp->heap[fp->tamanho++] = criar_no((unsigned char)i, freq[i]);
    }

    for (int i = (fp->tamanho / 2) - 1; i >= 0; i--)
        heapifyMin(fp->heap, fp->tamanho - 1, i);

    return fp;
}

no *fp_extract_min(filaPrioridade *fp) {
    if (fp->tamanho == 0)
        return NULL;

    no *min = fp->heap[0];
    fp->heap[0] = fp->heap[--fp->tamanho];
    heapifyMin(fp->heap, fp->tamanho - 1, 0);
    return min;
}

void fp_inserir(filaPrioridade *fp, no *n) {
    int i = fp->tamanho++;
    fp->heap[i] = n;

    while (i > 0 && fp->heap[(i - 1) / 2]->F > fp->heap[i]->F) {
        trocar(&fp->heap[i], &fp->heap[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

void fp_liberar(filaPrioridade *fp) {
    free(fp->heap);
    free(fp);
}


no *construir_arvore(int *freq) {
    filaPrioridade *fp = fp_criar(freq);

    if (fp->tamanho == 0) {
        fp_liberar(fp);
        return NULL;
    }

    while (fp->tamanho > 1) {
        no *x = fp_extract_min(fp);
        no *y = fp_extract_min(fp);

        no *pai = criar_no(
            (x->byte < y->byte) ? x->byte : y->byte,
            x->F + y->F
        );

        pai->E = x;
        pai->D = y;

        fp_inserir(fp, pai);
    }

    no *raiz = fp_extract_min(fp);
    fp_liberar(fp);
    return raiz;
}

void gerar_codigos(no *r, unsigned long long bits, unsigned char tam, codigo *tab) {
    if (!r) return;

    if (!r->E && !r->D) {
        tab[r->byte].bits = (tam == 0) ? 0 : bits;
        tab[r->byte].tam  = (tam == 0) ? 1 : tam;
        return;
    }

    gerar_codigos(r->E, bits << 1, tam + 1, tab);
    gerar_codigos(r->D, (bits << 1) | 1, tam + 1, tab);
}


typedef struct {
    unsigned char *data;
    int capacity;
    int byte_pos;
    int bit_pos;
} BitStream;

BitStream *bitstream_criar(int cap) {
    BitStream *bs = malloc(sizeof(BitStream));
    bs->data = malloc(cap);
    bs->capacity = cap;
    bs->byte_pos = 0;
    bs->bit_pos = 0;
    return bs;
}

void bitstream_free(BitStream *bs) {
    free(bs->data);
    free(bs);
}

void bitstream_write(BitStream *bs, unsigned long long bits, int tam) {
    for (int i = tam - 1; i >= 0; i--) {
        unsigned char bit = (bits >> i) & 1;
        if (bs->bit_pos == 0)
            bs->data[bs->byte_pos] = 0;
        bs->data[bs->byte_pos] |= bit << (7 - bs->bit_pos);
        bs->bit_pos++;
        if (bs->bit_pos == 8) {
            bs->bit_pos = 0;
            bs->byte_pos++;
            if (bs->byte_pos >= bs->capacity) {
                bs->capacity *= 2;
                bs->data = realloc(bs->data, bs->capacity);
            }
        }
    }
}

int bitstream_tamanho(BitStream *bs) {
    return bs->byte_pos + (bs->bit_pos > 0 ? 1 : 0);
}



int rle(unsigned char *e, int n, unsigned char *out) {
    int out_idx = 0, count = 1;
    for (int i = 1; i <= n; i++) {
        if (i < n && e[i] == e[i-1] && count < 255)
            count++;
        else {
            out[out_idx++] = count;
            out[out_idx++] = e[i-1];
            count = 1;
        }
    }
    return out_idx;
}



void print_hex_bits(FILE *f, unsigned char *v, int n, int ultimo_bits) {
    static const char hex[] = "0123456789ABCDEF";
    for (int i = 0; i < n - 1; i++) {
        fputc(hex[v[i] >> 4], f);
        fputc(hex[v[i] & 0x0F], f);
    }
    if (ultimo_bits > 0) {
        unsigned char last = v[n - 1] >> (8 - ultimo_bits);
        if (ultimo_bits <= 4)
            fputc(hex[last], f);
        else {
            fputc(hex[last >> 4], f);
            fputc(hex[last & 0x0F], f);
        }
    }
}


void liberar_arvore(no *r) {
    if (r) {
        liberar_arvore(r->E);
        liberar_arvore(r->D);
        free(r);
    }
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

    for (int l = 0; l < N; l++) {
        int qtd;
        fscanf(entrada, "%d", &qtd);

        unsigned char *dados = malloc(qtd);
        int freq[256] = {0};

        for (int i = 0; i < qtd; i++) {
            unsigned int v;
            fscanf(entrada, "%2X", &v);
            dados[i] = v;
            freq[v]++;
        }

        unsigned char rle_out[20000];
        int rle_tam = rle(dados, qtd, rle_out);

        no *raiz = construir_arvore(freq);
        codigo tab[256] = {{0,0}};
        if (raiz) gerar_codigos(raiz, 0, 0, tab);

        BitStream *bs = bitstream_criar(20000);
        for (int i = 0; i < qtd; i++)
            bitstream_write(bs, tab[dados[i]].bits, tab[dados[i]].tam);

        int huf_tam = bitstream_tamanho(bs);
        int ultimo_bits = (bs->bit_pos == 0) ? 8 : bs->bit_pos;

        float pr = (float)rle_tam / qtd * 100.0f;
        float ph = (float)huf_tam / qtd * 100.0f;

        if (huf_tam <= rle_tam) {
            fprintf(saida, "%d->HUF(%.2f%%)=", l, ph);
            print_hex_bits(saida, bs->data, huf_tam, ultimo_bits);
        } else {
            fprintf(saida, "%d->RLE(%.2f%%)=", l, pr);
            for (int i = 0; i < rle_tam; i++)
                fprintf(saida, "%02X", rle_out[i]);
        }
        fprintf(saida, "\n");

        // bitstream_free(bs);
        // liberar_arvore(raiz);
        // free(dados);
    }

    // fclose(entrada);
    // fclose(saida);
    return 0;
}
