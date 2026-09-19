#include <cstdio>
#include <cstdlib>
#include "generator.h"

int generate_random_number(int min_value, int max_value) {
    return rand() % (max_value - min_value + 1) + min_value;
}