#include <stdio.h>

int esquerdo(int i) { return 2 * i + 1; }
int direito(int i) { return 2 * i + 2; }

void trocar(int *V, int i, int j) {
    int aux = V[i];
    V[i] = V[j];
    V[j] = aux;
}

void heapify(int *V, int n, int i) {
    int maior = i;
    int E = esquerdo(i);
    int D = direito(i);

    if (E < n && V[E] > V[maior])
        maior = E;

    if (D < n && V[D] > V[maior])
        maior = D;

    if (maior != i) {
        trocar(V, i, maior);
        heapify(V, n, maior);
    }
}

void construir_heap(int *V, int n) {
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(V, n, i);
}

void heapsort(int *V, int n) {
    construir_heap(V, n);

    for (int i = n - 1; i > 0; i--) {
        trocar(V, 0, i);
        heapify(V, i, 0);
    }
}


int main(){

int V[6]={3,5,37,8,15,1};

printf("VETOR ANTES:");
for(int i=0;i<6;i++){
printf(" %d ",V[i]);
}
heapsort(V,6);

printf("\nVETOR DEPOIS:");
for(int i=0;i<6;i++){
printf(" %d ",V[i]);
}


}
