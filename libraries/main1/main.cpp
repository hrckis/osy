#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "generator.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <rows> <cols>\n", argv[0]);
        return 1;
    }

    srand(time(NULL));
    int M = atoi(argv[1]);
    int N = atoi(argv[2]);

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            int random_number = generate_random_number(10, 1000);
            printf("%d ", random_number);
        }
        printf("\n");
    }

    return 0;
}