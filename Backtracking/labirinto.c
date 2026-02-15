#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 100
#define MAX_DIR 1000
#define MAX_DEPTH 10000

typedef struct robot {
    int cx, cy;
    int found;
    char directions[MAX_DIR];
    int dir_len;
    int depth;
} robot;

typedef struct maze {
    int x0, y0;
    int xmax, ymax;
    int coordinates[MAX][MAX];
} maze;

/* Movimentos */

void go_right(FILE* saida, robot *bot, maze *mz) {
    if (bot->cy + 1 >= mz->ymax) return;
    bot->cy++;

    if (bot->dir_len < MAX_DIR - 1) {
        bot->directions[bot->dir_len++] = 'D';
        bot->directions[bot->dir_len] = '\0';
    }

    fprintf(saida, "|D->%d,%d", bot->cx, bot->cy);
}

void go_left(FILE* saida, robot *bot, maze *mz) {
    if (bot->cy - 1 < 0) return;
    bot->cy--;

    if (bot->dir_len < MAX_DIR - 1) {
        bot->directions[bot->dir_len++] = 'E';
        bot->directions[bot->dir_len] = '\0';
    }

    fprintf(saida, "|E->%d,%d", bot->cx, bot->cy);
}

void go_up(FILE* saida, robot *bot, maze *mz) {
    if (bot->cx - 1 < 0) return;
    bot->cx--;

    if (bot->dir_len < MAX_DIR - 1) {
        bot->directions[bot->dir_len++] = 'F';
        bot->directions[bot->dir_len] = '\0';
    }

    fprintf(saida, "|F->%d,%d", bot->cx, bot->cy);
}

void go_down(FILE* saida, robot *bot, maze *mz) {
    if (bot->cx + 1 >= mz->xmax) return;
    bot->cx++;

    if (bot->dir_len < MAX_DIR - 1) {
        bot->directions[bot->dir_len++] = 'T';
        bot->directions[bot->dir_len] = '\0';
    }

    fprintf(saida, "|T->%d,%d", bot->cx, bot->cy);
}

/* Backtracking seguro */

void backtracking(FILE* saida, robot *bot) {
    if (bot->dir_len == 0) return;

    char last = bot->directions[bot->dir_len - 1];
    int nx = bot->cx;
    int ny = bot->cy;

    if (last == 'D') ny--;
    else if (last == 'E') ny++;
    else if (last == 'F') nx++;
    else if (last == 'T') nx--;

    if (nx >= 0 && nx < MAX && ny >= 0 && ny < MAX) {
        fprintf(saida, "|BT@%d,%d->%d,%d", bot->cx, bot->cy, nx, ny);
        bot->cx = nx;
        bot->cy = ny;
    }

    bot->directions[--bot->dir_len] = '\0';
}

/* DFS recursivo com proteção */

void look_around(FILE* saida, robot *bot, maze *mz) {

    if (bot->found) return;
    if (bot->depth++ > MAX_DEPTH) return;

    if (bot->cx < 0 || bot->cx >= mz->xmax ||
        bot->cy < 0 || bot->cy >= mz->ymax)
        goto end;

    if (mz->coordinates[bot->cx][bot->cy] != 0)
        goto end;

    if ((bot->cx == 0 || bot->cy == 0 ||
         bot->cx == mz->xmax - 1 || bot->cy == mz->ymax - 1)) {

        fprintf(saida, "|FIM@%d,%d", bot->cx, bot->cy);
        bot->found = 1;
        goto end;
    }

    mz->coordinates[bot->cx][bot->cy] = 2;

    if (!bot->found && bot->cy + 1 < mz->ymax &&
        mz->coordinates[bot->cx][bot->cy + 1] == 0) {
        go_right(saida, bot, mz);
        look_around(saida, bot, mz);
    }

    if (!bot->found && bot->cx - 1 >= 0 &&
        mz->coordinates[bot->cx - 1][bot->cy] == 0) {
        go_up(saida, bot, mz);
        look_around(saida, bot, mz);
    }

    if (!bot->found && bot->cy - 1 >= 0 &&
        mz->coordinates[bot->cx][bot->cy - 1] == 0) {
        go_left(saida, bot, mz);
        look_around(saida, bot, mz);
    }

    if (!bot->found && bot->cx + 1 < mz->xmax &&
        mz->coordinates[bot->cx + 1][bot->cy] == 0) {
        go_down(saida, bot, mz);
        look_around(saida, bot, mz);
    }

    if (!bot->found)
        backtracking(saida, bot);

end:
    bot->depth--;
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
        if (entrada) fclose(entrada);
        if (saida) fclose(saida);
        return 1;
    }

    int N;
    fscanf(entrada, "%d", &N);

    robot *bot = calloc(1, sizeof(robot));
    maze  *mz  = calloc(1, sizeof(maze));

    for (int i = 0; i < N; i++) {

        memset(bot, 0, sizeof(robot));
        memset(mz, 0, sizeof(maze));

        int ry, rx;
        fscanf(entrada, "%d %d", &ry, &rx);

        mz->xmax = rx;
        mz->ymax = ry;

        char aux;

        for (int j = 0; j < rx; j++) {
            for (int k = 0; k < ry; k++) {
                fscanf(entrada, " %c", &aux);
                if (aux == '1') mz->coordinates[j][k] = 1;
                else {
                    mz->coordinates[j][k] = 0;
                    if (aux == 'X') {
                        bot->cx = j;
                        bot->cy = k;
                        mz->x0 = j;
                        mz->y0 = k;
                    }
                }
            }
        }

        fprintf(saida, "L%d:INI@%d,%d", i, mz->x0, mz->y0);
        look_around(saida, bot, mz);

        if (!bot->found)
            fprintf(saida, "|FIM@-,-");

        if (i != N - 1) fprintf(saida, "\n");
    }

    free(bot);
    free(mz);
    fclose(entrada);
    fclose(saida);
    return 0;
}
