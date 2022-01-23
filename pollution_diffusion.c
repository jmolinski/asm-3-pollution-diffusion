#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Typ fixed to po prostu integer, ale o interpretacji stałopozycyjnej.
typedef int32_t fixed;

const unsigned FRACTION_BITS = 8;

const float SCALING_FACTOR = 1.0f / (1 << FRACTION_BITS);

// Przygotowuje symulację, np. inicjuje pomocnicze struktury.
void start(int szer, int wys, fixed *M, fixed waga);

// Przeprowadza pojedynczy krok symulacji dla podanego wejścia
// (rozmiar tablicy T jest zgodny z parametrem wys powyżej).
// Po jej wykonaniu matryca M (przekazana przez parametr start) zawiera nowy stan.
void step(fixed T[]);

fixed waga;
int wysokosc;
int szerokosc;
fixed* matryca;

void start(int szer, int wys, fixed *M, fixed coeff) {
    szerokosc = szer;
    wysokosc = wys;
    matryca = M;
    waga = coeff;
}

fixed get_diff(fixed this, int r, int c) {
    if (c < 0 || c >= szerokosc || r < 0 || r >= wysokosc) {
        return 0;
    }
    return matryca[r * szerokosc + c] - this;
}

void step(fixed T[]) {
    fixed* second_part = &matryca[szerokosc * wysokosc];

    for (int i = 0; i < wysokosc; ++i) {
        second_part[i * szerokosc] = T[i];
    }

    for (int c = 1; c < szerokosc; c++) {
        for (int r = 0; r < wysokosc; r++) {

            fixed this = matryca[r * szerokosc + c];

            fixed delta = get_diff(this, r - 1, c) + get_diff(this, r - 1, c - 1) + get_diff(this, r, c - 1) + get_diff(this, r + 1, c) + get_diff(this, r + 1, c - 1);
            if (delta >= 0) {
                delta = (delta * waga) >> FRACTION_BITS;
            } else {
                delta = -((-delta * waga) >> FRACTION_BITS);
            }

            second_part[r * szerokosc + c] = matryca[r * szerokosc + c] + delta;
        }
    }

    for (int i = 0; i < wysokosc * szerokosc; ++i) {
        matryca[i] = second_part[i];
    }
}

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

void read_matrix(FILE *fp, fixed *matrix, int columns, int rows) {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            matrix[r * columns + c] = read_fixed_from_file(fp);
        }
    }
}

void read_data_for_step(FILE *fp, fixed *data_for_step, int rows) {
    for (int r = 0; r < rows; r++) {
        data_for_step[r] = read_fixed_from_file(fp);
    }
}

void print_matrix(fixed *matrix, int columns, int rows) {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            printf("%.3f\t", fixed_to_float(matrix[r * columns + c]));
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
    fscanf(fp, "%d", &columns);
    fscanf(fp, "%d", &rows);
    fixed coeff = read_fixed_from_file(fp);
    fixed *matrix = calloc((columns * rows) * 2, sizeof(fixed));
    read_matrix(fp, matrix, columns, rows);

    int steps;
    fscanf(fp, "%d", &steps);

    start(columns, rows, matrix, coeff);

    fixed *data_for_step = malloc(sizeof(fixed) * rows);
    for (int i = 0;; i++) {
        printf("Liczba wykonanych krokow: %d\nStan macierzy:\n", i);
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
