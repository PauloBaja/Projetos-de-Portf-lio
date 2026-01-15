#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int n, p;
    char **lista; 
} pacote;

int esquerdo(int i) { return 2 * i + 1; }
int direito(int i) { return 2 * i + 2; }

void trocar(pacote *V, int i, int j) {
    pacote aux = V[i];
    V[i] = V[j];
    V[j] = aux;
}

void heapify(pacote *V, int n, int i) {
    int maior = i;
    int E = esquerdo(i);
    int D = direito(i);

    if (E < n && V[E].p < V[maior].p)
        maior = E;

    if (D < n && V[D].p < V[maior].p)
        maior = D;

    if (maior != i) {
        trocar(V, i, maior);
        heapify(V, n, maior);
    }
}

void construir_heap(pacote *V, int n) {
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(V, n, i);
}

void heapsort(pacote *V, int n) {
    construir_heap(V, n);

    for (int i = n - 1; i > 0; i--) {
        trocar(V, 0, i);
        heapify(V, i, 0);
    }
}

int leitura(FILE *entrada, pacote *aux, int lim) {
    fscanf(entrada, "%d %d", &aux->p, &aux->n);

    aux->lista = malloc(aux->n * sizeof(char*));
    for(int i = 0; i < aux->n; i++) {
        aux->lista[i] = malloc(100 * sizeof(char)); 
        fscanf(entrada, "%s", aux->lista[i]);
    }

    return aux->n; 
}

void exibelista(FILE *saida, pacote *col, int num) {

    fprintf(saida, "|");
    for(int j = 0; j < num; j++) {
        for(int i = 0; i < col[j].n; i++) {
            fprintf(saida, "%s", col[j].lista[i]);
            free(col[j].lista[i]); 
            if(i!=(col[j].n)-1) fprintf(saida,",");
        }
        free(col[j].lista); 
        fprintf(saida, "|");
    }
    fprintf(saida, "\n");
}

int main(int argc, char *argv[]) {
    if(argc < 3) {
        printf("Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    FILE *entrada = fopen(argv[1], "r");
    if(!entrada) {
        perror("Erro ao abrir arquivo de entrada");
        return 1;
    }

    FILE *saida = fopen(argv[2], "w");
    if(!saida) {
        perror("Erro ao abrir arquivo de saída");
        fclose(entrada);
        return 1;
    }

    int N, L;
    fscanf(entrada, "%d %d", &N, &L); 

    pacote *lista = malloc(N * sizeof(pacote));
    int capacidade = L;
    int acc = 0;

    for(int k = 0; k < N; k++) {
        pacote aux;
        int lidos = leitura(entrada, &aux, L);

       
        if(lidos > capacidade) {
            heapsort(lista,acc);
            exibelista(saida, lista, acc);
            capacidade = L;
            acc = 0;
        }

        lista[acc++] = aux;
        capacidade -= lidos;
    }

    exibelista(saida, lista, acc);

    return 0;
}
