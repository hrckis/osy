#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "compute.h"

int compute(FILE *f) {
    char buffer[4096];
    int line = 0;

    while (fgets(buffer, sizeof buffer, f) != NULL) {
        line++;

        char *nl = strchr(buffer, '\n');
        if (nl != NULL) *nl = '\0';

        char *p = buffer; // &buffer[0];
        long sum = 0;
        int count = 0;

        while (true) {
            char *end;
            long value = strtol(p, &end, 10);
            if (p == end) break; 

            sum += value;
            count++;
            p = end;
        }

        if (count == 0) continue; 

        printf("%d %s %ld\n", line, buffer, sum);
    }

    return 0;
}