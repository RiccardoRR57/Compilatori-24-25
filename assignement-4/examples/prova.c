#include <stdio.h>

// Funzioni fittizie per dare un "corpo" ai loop
// e prevenire che vengano ottimizzati via troppo aggressivamente.
void do_something_in_loop1(int i) {
    if (i % 100 == 0) { // Aggiungi un po' di complessità minima
        printf("L1: %d\n", i);
    }
}

void do_something_in_loop2(int j) {
    if (j % 100 == 0) {
        printf("L2: %d\n", j);
    }
}

int main() {
    volatile int limit1 = 1000; // volatile per cercare di prevenire l'unrolling completo
    volatile int limit2 = 500;

    // Primo loop
    for (int i = 0; i < limit1; ++i) {
        do_something_in_loop1(i);
    }

    // Secondo loop, immediatamente successivo
    for (int j = 0; j < limit2; ++j) {
        do_something_in_loop2(j);
    }

    printf("Loops terminati.\n");
    return 0;
}