#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct contador{
    int tam;
    char nome[3];
}counter;

void addvetor(counter *lista, int c, int pos, char nome[3]) {
    lista[pos].tam = c;
    strcpy(lista[pos].nome,nome);
}

void trocar(int *i, int *j, int *acc){
    int aux = *i;
    *i = *j;
    *j = aux;
    (*acc)++; 
}

int hoare(int* V, int i , int j, int *acc){
    (*acc)++;

    int P = V[i], x = i - 1, y = j + 1;
    
    while(1){
        do { y--;} while(V[y] > P); 
        do { x++;} while(V[x] < P); 
        if(x < y) trocar(&V[x], &V[y], acc);
        else return y;
    }

}


int lomuto(int* V, int i, int j, int *acc) {
    int P = V[j];
    int x = i - 1;

    (*acc)++; 

    for (int y = i; y < j; y++) {
        if (V[y] <= P)
            trocar(&V[++x], &V[y], acc);
    }

    trocar(&V[++x], &V[j], acc); 
    return x;
}

int hoare_rand(int* V, int i, int j,int *acc) {

    trocar(&V[i], &V[i + (abs(V[i]) % (j - i + 1))],acc);

    return hoare(V, i, j,acc);
 }

int lomuto_rand(int *V, int i, int j, int *acc) {  
    trocar(&V[ i + (abs(V[i]) % (j - i + 1))], &V[j], acc);             
    return lomuto(V, i, j, acc);               
}


int hmediana(int *V , int i , int j, int *acc){
    int n = j - i + 1;
    int idx1 = i + n/4;
    int idx2 = i + n/2;
    int idx3 = i + 3*n/4;

    int a = V[idx1], b = V[idx2], c = V[idx3];
    int med_idx;
    
    if ((a <= b && b <= c) || (c <= b && b <= a))
        med_idx = idx2;
    else if ((b <= a && a <= c) || (c <= a && a <= b))
        med_idx = idx1;
    else
        med_idx = idx3;

    trocar(&V[i], &V[med_idx], acc);

    return hoare(V, i, j, acc);
}


int lmediana(int *V , int i , int j, int *acc){
    int n = j - i + 1;
 
    int idx1 = i + n/4;
    int idx2 = i + n/2;
    int idx3 = i + 3*n/4;

    int a = V[idx1], b = V[idx2], c = V[idx3];
    int med_idx;
    
    if ((a <= b && b <= c) || (c <= b && b <= a))
        med_idx = idx2;
    else if ((b <= a && a <= c) || (c <= a && a <= b))
        med_idx = idx1;
    else
        med_idx = idx3;

    trocar(&V[j], &V[med_idx], acc);
    return lomuto(V, i, j, acc);
}
void qshp(int* V, int i , int j, int *acc){
    if (i < j){
        (*acc)++;
        int p = hoare(V, i, j, acc);
        qshp(V, i, p, acc);
        qshp(V, p + 1, j, acc);
    }
}

void qslp(int* V, int i , int j, int *acc){
    if (i < j){
        (*acc)++; 
        int p = lomuto(V, i, j, acc);
        qslp(V, i, p - 1, acc); 
        qslp(V, p + 1, j, acc); 
    }
}

void qsla(int* V, int i , int j, int *acc){
    if (i < j){
        (*acc)++;
        int p = lomuto_rand(V, i, j, acc);
        qsla(V, i, p - 1, acc);  
        qsla(V, p + 1, j, acc);
    }
}


void qsha(int* V, int i , int j, int *acc){
    if (i < j){
        (*acc)++;
        int p = hoare_rand(V, i, j, acc);
        qsha(V, i, p, acc);
        qsha(V, p + 1, j, acc);
    }
}

void qshm(int* V, int i , int j, int *acc){
    if (i < j){
        (*acc)++;
        int p = hmediana(V, i, j, acc);
        qshm(V, i, p, acc);
        qshm(V, p + 1, j, acc);
    }
}
void qslm(int* V, int i , int j, int *acc){
    if (i < j){
        (*acc)++;
        int p = lmediana(V, i, j, acc);
        qslm(V, i, p - 1, acc);  
        qslm(V, p + 1, j, acc);
    }
}

// Copia n elementos do vetor 'origem' para o vetor 'destino'
void copiar(counter *destino, counter *origem, int n) {
    for (int k = 0; k < n; k++) {
        destino[k].tam = origem[k].tam;
        strcpy(destino[k].nome, origem[k].nome);
    }
}

void intercalar(counter *S, counter *E, int i, int m, int j) {
    int i1 = i;      
    int i2 = m + 1;    
    int k = i;        

    while (i1 <= m && i2 <= j) {
        if (E[i1].tam <= E[i2].tam) {
            S[k].tam = E[i1].tam;
            strcpy(S[k].nome, E[i1].nome);
            i1++;
        } else {
            S[k].tam = E[i2].tam;
            strcpy(S[k].nome, E[i2].nome);
            i2++;
        }
        k++;
    }

    while (i1 <= m) {
        S[k].tam = E[i1].tam;
        strcpy(S[k].nome, E[i1].nome);
        i1++;
        k++;
    }

    while (i2 <= j) {
        S[k].tam = E[i2].tam;
        strcpy(S[k].nome, E[i2].nome);
        i2++;
        k++;
    }

    for (int t = i; t <= j; t++) {
        E[t].tam = S[t].tam;
        strcpy(E[t].nome, S[t].nome);
    }
}


void mergesort(counter *S, counter *E, int i, int j){


if(i<j){

    int m  = i +(j-i) / 2;
    
    mergesort(S,E,i,m);
    mergesort(S,E,m+1,j);
    intercalar(S,E,i,m,j);
}
}
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    FILE *entrada = fopen(argv[1], "r");
    if (!entrada) {
        perror("Erro ao abrir arquivo de entrada");
        return 1;
    }

    FILE *saida = fopen(argv[2], "w");
    if (!saida) {
        perror("Erro ao abrir arquivo de saída");
        fclose(entrada);
        return 1;
    }

    int N;
    int *vetor;

    fscanf(entrada, "%d" ,&N);

    for (int i=0; i<N; i++){
        int len,xhp=1,xha=1,xhm=1,xlp=1,xla=1,xlm=1;
        counter lista[6],aux[6];

        fscanf(entrada, "%d", &len);
        fprintf(saida,"[%d]",len);

        vetor = malloc(len*sizeof(int));
        for(int j=0; j < len ; j++) fscanf(entrada, "%d",&vetor[j]);

        int *vhp = malloc(len * sizeof(int));
        int *vha = malloc(len * sizeof(int));
        int *vhm = malloc(len * sizeof(int));
        int *vlp = malloc(len * sizeof(int));
        int *vla = malloc(len * sizeof(int));
        int *vlm = malloc(len * sizeof(int));

  
        memcpy(vhp, vetor, len * sizeof(int));
        memcpy(vha, vetor, len * sizeof(int));
        memcpy(vhm, vetor, len * sizeof(int));
        memcpy(vlp, vetor, len * sizeof(int));
        memcpy(vla, vetor, len * sizeof(int));
        memcpy(vlm, vetor, len * sizeof(int));
        
        
        qslp(vlp,0,len-1,&xlp);
        qslm(vlm,0,len-1,&xlm);  
        qsla(vla,0,len-1,&xla);
        qshp(vhp,0,len-1,&xhp);
        qshm(vhm,0,len-1,&xhm);
        qsha(vha,0,len-1,&xha);

        addvetor(lista, xlp, 0, "LP");
        addvetor(lista, xlm, 1, "LM");
        addvetor(lista, xla, 2, "LA");
        addvetor(lista, xhp, 3, "HP");
        addvetor(lista, xhm, 4, "HM");
        addvetor(lista, xha, 5, "HA");

        
        mergesort(aux,lista,0,5);
        for(int k=0;k<6;k++){
             if (k==0) fprintf(saida,":%s(%d)",lista[k].nome,lista[k].tam);
                else fprintf(saida,",%s(%d)",lista[k].nome,lista[k].tam);
        }
        fprintf(saida,"\n");
    }


}

// int main(){
   

//     int V[] = {847, 38, -183, -13, 94, -2, -42, 54, 28, 100};
//     int N = 10;

//         int acc_hp = 1,acc_ha=1,acc_hm=1;  

//         int Vp[N], Va[N], Vm[N];
        
//         for(int i = 0; i < N; i++){
//             Vp[i] = V[i];  
//             Va[i] = V[i]; 
//             Vm[i] = V[i]; 
//         }

//         qshp(Vp, 0, N-1, &accp);
//         qsha(Va, 0, N-1, &acca);
//         qshm(Vm, 0, N-1, &accm);


//         printf("HP:%d\n", accp);
//         printf("HA:%d\n", acca);
//         printf("HM:%d\n", accm);
    

//     return 0;
// }
