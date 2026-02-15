#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct palavra{
    char *string;
    int a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z;
}palavra;

void empilha(palavra *palavra, long *aux){
    int tam = strlen(palavra->string);
    for(int i = 0; i < tam; i++){
        (*aux)++; // conta a iteração

        if (palavra->string[i] == 'a') palavra->a++;
        if (palavra->string[i] == 'b') palavra->b++;
        if (palavra->string[i] == 'c') palavra->c++;
        if (palavra->string[i] == 'd') palavra->d++;
        if (palavra->string[i] == 'e') palavra->e++;
        if (palavra->string[i] == 'f') palavra->f++;
        if (palavra->string[i] == 'g') palavra->g++;
        if (palavra->string[i] == 'h') palavra->h++;
        if (palavra->string[i] == 'i') palavra->i++;
        if (palavra->string[i] == 'j') palavra->j++;
        if (palavra->string[i] == 'k') palavra->k++;
        if (palavra->string[i] == 'l') palavra->l++;
        if (palavra->string[i] == 'm') palavra->m++;
        if (palavra->string[i] == 'n') palavra->n++;
        if (palavra->string[i] == 'o') palavra->o++;
        if (palavra->string[i] == 'p') palavra->p++;
        if (palavra->string[i] == 'q') palavra->q++;
        if (palavra->string[i] == 'r') palavra->r++;
        if (palavra->string[i] == 's') palavra->s++;
        if (palavra->string[i] == 't') palavra->t++;
        if (palavra->string[i] == 'u') palavra->u++;
        if (palavra->string[i] == 'v') palavra->v++;
        if (palavra->string[i] == 'w') palavra->w++;
        if (palavra->string[i] == 'x') palavra->x++;
        if (palavra->string[i] == 'y') palavra->y++;
        if (palavra->string[i] == 'z') palavra->z++;
    }
}

void desempilha(palavra *palavra, char letra, long *aux){
    (*aux)++;

    if (letra == 'a') palavra->a--;
    if (letra == 'b') palavra->b--;
    if (letra == 'c') palavra->c--;
    if (letra == 'd') palavra->d--;
    if (letra == 'e') palavra->e--;
    if (letra == 'f') palavra->f--;
    if (letra == 'g') palavra->g--;
    if (letra == 'h') palavra->h--;
    if (letra == 'i') palavra->i--;
    if (letra == 'j') palavra->j--;
    if (letra == 'k') palavra->k--;
    if (letra == 'l') palavra->l--;
    if (letra == 'm') palavra->m--;
    if (letra == 'n') palavra->n--;
    if (letra == 'o') palavra->o--;
    if (letra == 'p') palavra->p--;
    if (letra == 'q') palavra->q--;
    if (letra == 'r') palavra->r--;
    if (letra == 's') palavra->s--;
    if (letra == 't') palavra->t--;
    if (letra == 'u') palavra->u--;
    if (letra == 'v') palavra->v--;
    if (letra == 'w') palavra->w--;
    if (letra == 'x') palavra->x--;
    if (letra == 'y') palavra->y--;
    if (letra == 'z') palavra->z--;
}

int acessa(palavra *palavra, char letra, long *aux){
    (*aux)++;

    if (letra == 'a') return palavra->a;
    if (letra == 'b') return palavra->b;
    if (letra == 'c') return palavra->c;
    if (letra == 'd') return palavra->d;
    if (letra == 'e') return palavra->e;
    if (letra == 'f') return palavra->f;
    if (letra == 'g') return palavra->g;
    if (letra == 'h') return palavra->h;
    if (letra == 'i') return palavra->i;
    if (letra == 'j') return palavra->j;
    if (letra == 'k') return palavra->k;
    if (letra == 'l') return palavra->l;
    if (letra == 'm') return palavra->m;
    if (letra == 'n') return palavra->n;
    if (letra == 'o') return palavra->o;
    if (letra == 'p') return palavra->p;
    if (letra == 'q') return palavra->q;
    if (letra == 'r') return palavra->r;
    if (letra == 's') return palavra->s;
    if (letra == 't') return palavra->t;
    if (letra == 'u') return palavra->u;
    if (letra == 'v') return palavra->v;
    if (letra == 'w') return palavra->w;
    if (letra == 'x') return palavra->x;
    if (letra == 'y') return palavra->y;
    if (letra == 'z') return palavra->z;

    return 0;
}

int verficador(palavra *a, palavra *b, long *aux){
    int aux2;
    for(int i = 0; i < strlen(b->string); i++){
        aux2 = acessa(a, b->string[i], aux);

        if (aux2 > 0)
            desempilha(a, b->string[i], aux);
        else
            return 0;
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

    palavra *a = malloc(sizeof(palavra));
    palavra *b = malloc(sizeof(palavra));

    a->string = malloc(100 * sizeof(char));
    b->string = malloc(100 * sizeof(char));
    
    long tt = 0;

    for (int i = 0; i < N; i++) {

        // contador de "tempo"
        long aux = 0;

        // zera contadores (mantendo ponteiros)
        memset(a, 0, sizeof(palavra));
        memset(b, 0, sizeof(palavra));

        // restaura ponteiros após o memset
        a->string = malloc(100 * sizeof(char));
        b->string = malloc(100 * sizeof(char));

        fscanf(entrada, "%s %s", a->string, b->string);

       
            empilha(a, &aux);

            if (verficador(a, b, &aux) == 1)
                fprintf(saida, "%s e %s sao anagramas. (iteracoes=%ld)\n", a->string, b->string, aux);
            else
                fprintf(saida, "%s e %s nao sao anagramas (iteracoes=%ld)\n", a->string, b->string, aux);
           
                tt+=aux;
        free(a->string);
        free(b->string);
    }
    fprintf(saida, "TOTAL DE ITERACOES %ld\n",tt);

    free(a);
    free(b);
    fclose(entrada);
    fclose(saida);

    return 0;
}
