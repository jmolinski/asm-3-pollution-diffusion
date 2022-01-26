#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

// Typ fixed to po prostu integer, ale o interpretacji stałopozycyjnej.
typedef int32_t fixed;

#define FRACTION_BITS 8

const float SCALING_FACTOR = 1.0f / (1 << FRACTION_BITS);

// Przygotowuje symulację.
extern void start(int szer, int wys, fixed *M, fixed c);

// Przeprowadza pojedynczy krok symulacji dla podanego wejścia
// (rozmiar tablicy T jest zgodny z parametrem wys powyżej).
// Po jej wykonaniu matrix M (przekazana przez parametr start) zawiera nowy stan.
extern void step(fixed T[]);

static inline fixed float_to_fixed(float f) {
    return (fixed)round(f / SCALING_FACTOR);
}

static inline float fixed_to_float(fixed f) {
    return (float) (f) * SCALING_FACTOR;
}

static inline fixed read_fixed_from_file(FILE *fp) {
    float f;
    fscanf(fp, "%f", &f);
    if (f < 0) {
        fprintf(stderr, "Wartość ujemna: %f\n", f);
        exit(1);
    }
    return float_to_fixed(f);
}

void read_matrix(FILE *fp, fixed *m, int columns, int rows) {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            m[r * columns + c] = read_fixed_from_file(fp);
        }
    }
}

void read_data_for_step(FILE *fp, fixed *data_for_step, int rows) {
    for (int r = 0; r < rows; r++) {
        data_for_step[r] = read_fixed_from_file(fp);
    }
}

void print_matrix(fixed *m, int columns, int rows) {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            printf("%.3f\t", fixed_to_float(m[r * columns + c]));
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

    int columns_n, rows_n;
    fscanf(fp, "%d", &columns_n);

    fscanf(fp, "%d", &rows_n);
    fixed w = read_fixed_from_file(fp);
    fixed *m = calloc((columns_n * rows_n) * 2, sizeof(fixed));

    read_matrix(fp, m, columns_n, rows_n);

    int steps;
    fscanf(fp, "%d", &steps);

    start(columns_n, rows_n, m, w);

    fixed *data_for_step = malloc(sizeof(fixed) * rows_n);
    for (int i = 0;; i++) {
        printf("Liczba wykonanych krokow: %d\nStan macierzy:\n", i);
        print_matrix(m, columns_n, rows_n);
        printf("\n");

        if (i == steps) {
            break;
        }

        read_data_for_step(fp, data_for_step, rows_n);
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
    free(m);

    return 0;
}
