#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct robot {
    int cx, cy,found;
    char directions[1000];
} robot;

typedef struct maze {
    int x0, y0, xmax, ymax;
    int coordinates[100][100];
} maze;

void go_right(FILE* saida, robot *bot, maze *mz) {
    bot->cy++;
    mz->coordinates[bot->cx][bot->cy] = 2;
    strcat(bot->directions, "D");
    fprintf(saida, "|D->%d,%d", bot->cx, bot->cy);
}

void go_left(FILE* saida, robot *bot, maze *mz) {
    bot->cy--;
    mz->coordinates[bot->cx][bot->cy] = 2;
    strcat(bot->directions, "E");
    fprintf(saida, "|E->%d,%d", bot->cx, bot->cy);
}

void go_up(FILE* saida, robot *bot, maze *mz) {
    bot->cx--;
    mz->coordinates[bot->cx][bot->cy] = 2;
    strcat(bot->directions, "F");
    fprintf(saida, "|F->%d,%d", bot->cx, bot->cy);
}

void go_down(FILE* saida, robot *bot, maze *mz) {
    bot->cx++;
    mz->coordinates[bot->cx][bot->cy] = 2;
    strcat(bot->directions, "T");
    fprintf(saida, "|T->%d,%d", bot->cx, bot->cy);
    // printf("%ld\n",strlen(bot->directions));
}

void backtracking(FILE* saida, robot *bot) {
    int command_size = strlen(bot->directions)-1;
    int bx = bot->cx;
    int by = bot->cy;

    if (bot->directions[command_size] == 'D') bot->cy--;
    else if (bot->directions[command_size] == 'E') bot->cy++;
    else if (bot->directions[command_size] == 'F') bot->cx++;
    else if (bot->directions[command_size] == 'T') bot->cx--;

    fprintf(saida, "|BT@%d,%d->%d,%d", bx, by, bot->cx, bot->cy);
    bot->directions[command_size] = '\0';
}

void look_around(FILE* saida, robot *bot, maze *mz) {
    mz->coordinates[bot->cx][bot->cy] = 2; // Marca a célula como visitada

    // Verifica se encontrou a borda do labirinto (fim)
    if (bot->cx == 0 || bot->cy == 0 || bot->cx == mz->xmax-1 || bot->cy == mz->ymax-1) {
        fprintf(saida, "|FIM@%d,%d", bot->cx, bot->cy);
        bot->found = 1;  // Marca como encontrado
        return;
    }

    // Verificação das direções possíveis
    if (mz->coordinates[bot->cx][bot->cy + 1] == 0) {  // Direita
        go_right(saida, bot, mz);
        look_around(saida, bot, mz);
        if (bot->found) return; // Se encontrou, sai
    }
    if (mz->coordinates[bot->cx - 1][bot->cy] == 0) {  // Cima
        go_up(saida, bot, mz);
        look_around(saida, bot, mz);
        if (bot->found) return; // Se encontrou, sai
    }
    if (mz->coordinates[bot->cx][bot->cy - 1] == 0) {  // Esquerda
        go_left(saida, bot, mz);
        look_around(saida, bot, mz);
        if (bot->found) return; // Se encontrou, sai
    }
    if (mz->coordinates[bot->cx + 1][bot->cy] == 0) {  // Baixo
        go_down(saida, bot, mz);
        look_around(saida, bot, mz);
        if (bot->found) return; // Se encontrou, sai
    }

    // Backtracking (volta à posição anterior)
    backtracking(saida, bot);       
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

    int N, ry, rx;
    robot *bot = malloc(sizeof(robot));
    maze *mz = malloc(sizeof(maze));

    bot->directions[0] = '\0';  // importante

    char aux;

    fscanf(entrada, "%d", &N);

    for (int i = 0; i < N; i++) {
        fscanf(entrada, "%d %d", &ry, &rx);
        mz->xmax = rx;
        mz->ymax = ry;

        for (int j = 0; j < rx; j++) {
            for (int k = 0; k < ry; k++) {
                fscanf(entrada, " %c", &aux);

                if (aux == '0'){ 
                    mz->coordinates[j][k] = 0;
                    // printf("0 ");
                }

                if (aux == '1'){ 
                    mz->coordinates[j][k] = 1;
                    // printf("1 ");
                }
                if (aux == 'X') {
                    mz->coordinates[j][k] = 2;
                    mz->x0 = j;
                    mz->y0 = k;
                    bot->cx = j;
                    bot->cy = k;
                    // printf("X ");
                }
            }
            // printf("\n");
        }

        
        fprintf(saida,"L%d:INI@%d,%d",i,mz->y0,mz->x0);
        look_around(saida, bot, mz);
        if (bot->cx == mz->x0 && bot->cy == mz->y0 ) fprintf(saida, "|FIM@-,-");

        fprintf(saida, "\n");
    }

    free(bot);
    free(mz);
    fclose(entrada);
    fclose(saida);

    return 0;
}
