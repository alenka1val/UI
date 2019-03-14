#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define N 3

/*TODO vyriesit nacitavanie
 *TODO free function */

typedef struct node {
    int value;
    int *direction;
    int **puzzle;
} NODE;

NODE *hash_array;

/***operation with string***/

int **str_cpy(int **puzzle) {
    int **cpy;
    cpy = (int **) malloc(N * sizeof(int *));
    for (int i = 0; i < N; i++) {
        cpy[i] = (int *) malloc(N * sizeof(int));
        for (int j = 0; j < N; j++) {
            cpy[i][j] = puzzle[i][j];
        }
    }
    return cpy;
}

int **swap(int **start, int *pst, int vertical, int horizontal) {
    int **to_return, help;
    to_return = str_cpy(start);

    help = to_return[pst[0]][pst[1]];
    to_return[pst[0]][pst[1]] = to_return[pst[0] + vertical][pst[1] + horizontal];
    to_return[pst[0] + vertical][pst[1] + horizontal] = help;
    return to_return;
}

void print_2D(int **puzzle) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", puzzle[i][j]);
        }
        putchar('\n');
    }
}

void print_1D(int *puzzle) {
    for (int i = 0; i < N * N; i++) {
        printf("%d ", puzzle[i]);
    }
    putchar('\n');
}

void fill_rd(int *recent_direction) {
    recent_direction[0] = -2;
    recent_direction[1] = -2;
    return;
}

int **fill_arry(int temp[N * N]) {
    int **puzzle;
    puzzle = (int **) malloc(N * sizeof(int *));
    for (int i = 0; i < N; i++) {
        puzzle[i] = (int *) malloc(N * sizeof(int));
        for (int j = 0; j < N; j++) {
            int c = temp[(i * N) + j];
            if (c >= 0 && c < 10) {
                puzzle[i][j] = c;
            } else {
                printf("ERROR: Zle zadane cislo. Zadane mozu byt len cele cisla v rozsahu <0,8>\n");
                exit(0);
            }
        }
    }
    return puzzle;
}

int *find_pstn(int num, int **puzzle) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (puzzle[i][j] == num) {
                int *tmp = malloc(2 * sizeof(int));
                tmp[0] = i;
                tmp[1] = j;
                return tmp;
            }
        }
    }
    return NULL;
}

int count_diff(int x1, int y1, int x2, int y2) {
    int count = 0;
    count += (x1 > x2) ? x1 - x2 : x2 - x1;
    count += (y1 > y2) ? y1 - y2 : y2 - y1;
    return count;
}

int *str_cat(int **puzzle) {
    int *cat_arry;
    cat_arry = (int *) malloc(N * N * sizeof(int));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cat_arry[(i * N) + j] = puzzle[i][j];
        }
    }
    //print_1D(cat_arry);
    return cat_arry;
}

/***Hash functions***/

int hash(int *puzzle) {
    int index = 0, val;//9973
    for (int i = 0; i < N; i++) {
        index = (i * 16) + puzzle[i];
    }
    index %= 362897;
    return index;
}

void init() {
    hash_array = (NODE *) calloc(1000000, sizeof(NODE));
    for (int i = 0; i < 1000000; i++) {
        hash_array[i].value = -1;
    }
    return;
}

int insert(int **puzzle, int hlbka) {
    int index = hash(str_cat(puzzle)), i = index;

    while (hash_array[i].value != -1 && i != index - 1) {
        if (i == 999999) {
            i = 0;
        } else {
            i++;
        }
    }
    if (i == index - 1) {
        printf("Out of memory");
        exit(0);
    }
    hash_array[i].value = hlbka;
    hash_array[i].puzzle = puzzle;
    return 1;
}

int search(int **puzzle) {
    int index = hash(str_cat(puzzle)), i = index;

    while (hash_array[i].value != -1 && i != index - 1) {
        if (heuristic_1(hash_array[i].puzzle, puzzle) == 0) {
            return 1;
        }
        if (hash_array[i].value == INT_MIN) {
            return 0;
        }
        if (i == 999999) {
            i = 0;
        } else {
            i++;
        }
    }
    return 0;
}

/***finding way***/
/***Heuristika 1- Nasledujuci krok urcuje podla vzdialenosti blokov od ich finalnej pozicie***/

int heuristic_1(int **start, int **end) {
    int counter = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (start[i][j] > 0) {
                if (start[i][j] != end[i][j] && start[i][j] > 0) {
                    int *tmp;
                    tmp = find_pstn(start[i][j], end);
                    counter += count_diff(i, j, tmp[0], tmp[1]);
                }
            }
        }
    }
    return counter;
}

/***Heuristika 2- Nasledujuci krok urcuje podla poctu blokov, ktore nie su na svojej pozicii***/

int heuristic_2(int **start, int **end) {
    int counter = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (start[i][j] > 0) {
                if (start[i][j] != end[i][j] && start[i][j] > 0) {
                    counter++;
                }
            }
        }
    }
    return counter;
}

int gen_state(int **start, int **end, int height, int *recent_direction, int heuristic) {
    NODE *arry_of_possib;
    int place = 0;
    int *tmp, temp;

    temp = (heuristic == 1) ? heuristic_1(start, end) : heuristic_2(start, end);
    if (height == 0 && temp == 0) {
        print_2D(start);
        return 1;
    }

    tmp = find_pstn(0, start);
    arry_of_possib = (NODE *) malloc(4 * sizeof(NODE));

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            if ((i == 0 && j == 0) || (i != 0 && j != 0)) {
                continue;
            }
            arry_of_possib[place].value = -1;
            if (tmp[0] + i < 0 || tmp[0] + i >= N || tmp[1] + j < 0 || tmp[1] + j >= N) {
                place++;
                continue;
            }

            if (recent_direction[0] > -2) {
                if (recent_direction[0] == i) {
                    if (recent_direction[1] == j) {
                        place++;
                        continue;
                    }
                }
            }
            arry_of_possib[place].puzzle = swap(start, tmp, i, j);

            arry_of_possib[place].value = (heuristic == 1) ? heuristic_1(arry_of_possib[place].puzzle, end)
                                                           : heuristic_2(
                            arry_of_possib[place].puzzle, end);
            arry_of_possib[place].direction = (int *) malloc(2 * sizeof(int));
            arry_of_possib[place].direction[0] = -1 * i;
            arry_of_possib[place].direction[1] = -1 * j;
            place++;
        }
    }

    for (int i = 0; i < 4; i++) {
        int min = -1, pst;

        for (int j = 0; j < 4; j++) {
            if (arry_of_possib[j].value > -1) {
                if ((min == -1) || (min > -1 && (arry_of_possib[j].value < min))) {
                    min = arry_of_possib[j].value;
                    pst = j;
                }
            }
        }

        if (min == -1) {
            return 0;
        }
        if (min == 0) {
            print_2D(start);
            printf("-----\n");
            return 1;
        }

        if (arry_of_possib[pst].value != -1) {
            if (arry_of_possib[pst].value == 0) {
                return 0;
            }
            if (search(arry_of_possib[pst].puzzle) == 0) {
                insert(arry_of_possib[pst].puzzle, height);
                if (gen_state(arry_of_possib[pst].puzzle, end, height + 1, arry_of_possib[pst].direction, heuristic) ==
                    1) {
                    print_2D(start);
                    printf("-----\n");
                    return 1;
                } else {
                    arry_of_possib[pst].value = -1;
                }
            }
        }

    }
    return 0;
}

/***IS SOlVABLE FUNCTION***/
/*funkcia rata # inverzii. ak je # inverzii parny je z mnoziny riesitelnych A ak neparny je z mnoziny riesitelnych B*/

int stop_function(int *puzzle) {
    int counter = 0;
    for (int i = 0; i < (N * N) - 1; i++) {
        for (int j = i + 1; j < N * N; j++) {
            if (puzzle[i] && puzzle[j] && puzzle[i] > puzzle[j]) {
                counter++;
            }
        }
    }
    if (counter % 2 == 0) {
        return 1;
    } else {
        return 0;
    }
}

int main() {

    int start[N * N] = {0, 1, 2, 3, 4, 5, 6, 7, 8}, **puzzle_start;
    int end[N * N] = {8, 0, 6, 5, 4, 7, 2, 3, 1}, **puzzle_end;
    int *recent_direction;

    recent_direction = (int *) malloc(2 * sizeof(int));
    fill_rd(recent_direction);
    puzzle_start = fill_arry(start);
    puzzle_end = fill_arry(end);

    if (stop_function(str_cat(puzzle_start)) ^ stop_function(str_cat(puzzle_end)) == 0) {
        init();
        gen_state(puzzle_start, puzzle_end, 0, recent_direction, 1);
        print_2D(puzzle_end);
    } else {
        printf("UNSOLVABLE\n");
    }

    return 0;
}