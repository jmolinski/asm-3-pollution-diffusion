#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// Typ fixed to po prostu integer, ale o interpretacji stałopozycyjnej.
typedef float fixed;

// Przygotowuje symulację, np. inicjuje pomocnicze struktury.
void start(int szer, int wys, fixed *M, fixed waga);

// Przeprowadza pojedynczy krok symulacji dla podanego wejścia
// (rozmiar tablicy T jest zgodny z parametrem wys powyżej).
// Po jej wykonaniu matryca M (przekazana przez parametr start) zawiera nowy stan.
void step(fixed T[]);

void read_matrix(FILE *fp, fixed *matrix, int columns, int rows) {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            // TODO convert decimal
            fscanf(fp, "%f", &matrix[r * columns + c]);
        }
    }
}

void read_data_for_step(FILE *fp, fixed *data_for_step, int rows) {
    for (int r = 0; r < rows; r++) {
        // TODO convert decimal
        fscanf(fp, "%f", &data_for_step[r]);
    }
}

void print_matrix(fixed *matrix, int columns, int rows) {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            // TODO print decimal
            printf("%.3f\t", matrix[r * columns + c]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    assert(argc == 2);

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("File cannot be opened %s\n", argv[1]);
        return 1;
    }

    int columns, rows;
    float cooler_temperature;
    fixed coeff;
    fscanf(fp, "%d", &columns);
    fscanf(fp, "%d", &rows);
    // TODO read decimal
    fscanf(fp, "%f", &coeff);
    fixed *matrix = calloc((columns * rows) * 2, sizeof(fixed));
    read_matrix(fp, matrix, columns, rows);

    int steps;
    fscanf(fp, "%d", &steps);

    fixed *data_for_step = malloc(sizeof(fixed) * rows);
    for (int i = 1;; i++) {
        printf("Liczba wykonanych krokow: %d\nStan macierzy:\n", i - 1);
        print_matrix(matrix, columns, rows);
        printf("\n");

        if (i == steps) {
            break;
        }

        read_data_for_step(fp, data_for_step, rows);
        step(data_for_step);

        if (i < steps) {
            printf("Nacisnij ENTER aby kontynuowac\n");
            char c;
            do {
                c = getchar();
            } while (c != '\n');
        }
    }

    fclose(fp);
    free(data_for_step);
    free(matrix);

    return 0;
}
