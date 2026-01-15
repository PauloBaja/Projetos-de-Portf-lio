#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* binarioParaHex(char *binario) {
    int tamanho = strlen(binario);

    int resto = tamanho % 4;
    if (resto != 0) {
        int novosBits = 4 - resto;
        char temp[65] = {0};
        for (int i = 0; i < novosBits; i++)
            temp[i] = '0';
        strcpy(temp + novosBits, binario);
        strcpy(binario, temp);
        tamanho += novosBits;
    }

    static char hex[17];
    int k = 0;

    for (int i = 0; i < tamanho; i += 4) {
        int valor = 0;
        for (int j = 0; j < 4; j++) {
            valor = valor * 2 + (binario[i + j] - '0');
        }
        hex[k++] = (valor < 10) ? ('0' + valor) : ('A' + (valor - 10));
    }
    hex[k] = '\0';
    return hex;
}


char* onehexbin(char hex){
    switch (hex) {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'A': return "1010";
        case 'B': return "1011";
        case 'C': return "1100";
        case 'D': return "1101";
        case 'E': return "1110";
        case 'F': return "1111";
        default: return NULL;
    }
}

void twohexbin(char hex[3], char bin[9]){
    bin[0] = '\0';  // RESET buffer
    strcat(bin, onehexbin(hex[0]));
    strcat(bin, onehexbin(hex[1]));
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

    int N;
    fscanf(entrada, "%d", &N);

    char ta[3] = {0};
    char tb[3] = {0};
    char bin[9];
    char temp1[200];
    char *result = malloc(100000);
    result[0] = '\0';
    int index=0;

    int aux, acc = 0;

char *resultado = malloc(N * 10000); 

for(int i = 0; i < N; i++){
    fscanf(entrada, "%d", &aux);

    result[0] = '\0';
    acc = 0;

for (int j = 0; j < aux; j++) {

    if (j == 0) {
        fscanf(entrada, "%2s", ta);
        acc = 1;
    }
    else {
        fscanf(entrada, "%2s", tb);

        if (strcmp(ta, tb) == 0) {
            acc++;
        } else {
            char temp1[20];
            sprintf(temp1, "%d%s", acc, ta);
            strcat(result, temp1);

            strcpy(ta, tb);
            acc = 1;
        }
    }
}

char temp1[20];
sprintf(temp1, "%d%s", acc, ta);
strcat(result, temp1);

strcat(resultado, result);
index++;

int tam = strlen(resultado);

fprintf(saida,"%d->%s (%d->%d)\n",index-1,resultado,aux*2,tam);
resultado[0]='\0';

}

}

