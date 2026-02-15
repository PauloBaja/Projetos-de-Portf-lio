#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct robot {
    int cx, cy;
    int found;
    char directions[1000];
} robot;

typedef struct maze {
    int x0, y0;
    int xmax, ymax;
    int coordinates[100][100];
} maze;

/* Movimentos */

void go_right(FILE* saida, robot *bot) {
    bot->cy++;
    if (strlen(bot->directions) < sizeof(bot->directions) - 2)
        strcat(bot->directions, "D");
    fprintf(saida, "|D->%d,%d", bot->cx, bot->cy);
}

void go_left(FILE* saida, robot *bot) {
    bot->cy--;
    if (strlen(bot->directions) < sizeof(bot->directions) - 2)
        strcat(bot->directions, "E");
    fprintf(saida, "|E->%d,%d", bot->cx, bot->cy);
}

void go_up(FILE* saida, robot *bot) {
    bot->cx--;
    if (strlen(bot->directions) < sizeof(bot->directions) - 2)
        strcat(bot->directions, "F");
    fprintf(saida, "|F->%d,%d", bot->cx, bot->cy);
}

void go_down(FILE* saida, robot *bot) {
    bot->cx++;
    if (strlen(bot->directions) < sizeof(bot->directions) - 2)
        strcat(bot->directions, "T");
    fprintf(saida, "|T->%d,%d", bot->cx, bot->cy);
}

/* Backtracking seguro */

void backtracking(FILE* saida, robot *bot) {
    int len = strlen(bot->directions);
    if (len == 0) return;

    int bx = bot->cx;
    int by = bot->cy;
    char last = bot->directions[len - 1];

    if (last == 'D') bot->cy--;
    else if (last == 'E') bot->cy++;
    else if (last == 'F') bot->cx++;
    else if (last == 'T') bot->cx--;

    fprintf(saida, "|BT@%d,%d->%d,%d", bx, by, bot->cx, bot->cy);
    bot->directions[len - 1] = '\0';
}

/* Busca DFS */

void look_around(FILE* saida, robot *bot, maze *mz) {

    /* Saída = borda com 0 */
    if ((bot->cx == 0 || bot->cy == 0 ||
         bot->cx == mz->xmax - 1 || bot->cy == mz->ymax - 1) &&
        mz->coordinates[bot->cx][bot->cy] == 0) {

        fprintf(saida, "|FIM@%d,%d", bot->cx, bot->cy);
        bot->found = 1;
        return;
    }

    /* Marca como visitado */
    mz->coordinates[bot->cx][bot->cy] = 2;

    /* Direita */
    if (!bot->found &&
        bot->cy + 1 < mz->ymax &&
        mz->coordinates[bot->cx][bot->cy + 1] == 0) {

        go_right(saida, bot);
        look_around(saida, bot, mz);
    }

    /* Cima */
    if (!bot->found &&
        bot->cx - 1 >= 0 &&
        mz->coordinates[bot->cx - 1][bot->cy] == 0) {

        go_up(saida, bot);
        look_around(saida, bot, mz);
    }

    /* Esquerda */
    if (!bot->found &&
        bot->cy - 1 >= 0 &&
        mz->coordinates[bot->cx][bot->cy - 1] == 0) {

        go_left(saida, bot);
        look_around(saida, bot, mz);
    }

    /* Baixo */
    if (!bot->found &&
        bot->cx + 1 < mz->xmax &&
        mz->coordinates[bot->cx + 1][bot->cy] == 0) {

        go_down(saida, bot);
        look_around(saida, bot, mz);
    }

    if (!bot->found)
        backtracking(saida, bot);
}

/* MAIN */

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

    robot *bot = malloc(sizeof(robot));
    maze  *mz  = malloc(sizeof(maze));

    for (int i = 0; i < N; i++) {

        int ry, rx;
        fscanf(entrada, "%d %d", &ry, &rx);

        mz->xmax = rx;
        mz->ymax = ry;

        bot->found = 0;
        bot->directions[0] = '\0';

        /* Limpa labirinto anterior */
        memset(mz->coordinates, 0, sizeof(mz->coordinates));

        char aux;

        for (int j = 0; j < rx; j++) {
            for (int k = 0; k < ry; k++) {
                fscanf(entrada, " %c", &aux);

                if (aux == '0') mz->coordinates[j][k] = 0;
                else if (aux == '1') mz->coordinates[j][k] = 1;
                else if (aux == 'X') {
                    mz->coordinates[j][k] = 0; // mantém 0 para permitir saída na borda
                    mz->x0 = j;
                    mz->y0 = k;
                    bot->cx = j;
                    bot->cy = k;
                }
            }
        }

        fprintf(saida, "L%d:INI@%d,%d", i, mz->y0, mz->x0);
        look_around(saida, bot, mz);

        if (!bot->found)
            fprintf(saida, "|FIM@-,-");

        if(i!=N-1) fprintf(saida, "\n");
    }

    free(bot);
    free(mz);
    fclose(entrada);
    fclose(saida);

    return 0;
}
