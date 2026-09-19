#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "compute.h"

int compute(FILE *f) {
    char buffer[4096];
    int line = 0;
    int errors = 0;

    while (fgets(buffer, sizeof buffer, f) != NULL) {
        line++;

        char *nl = strchr(buffer, '\n');
        if (nl != NULL) *nl = '\0';

        char *p = buffer;
        long sum = 0;
        long tmp = 0;
        bool has_tmp = false;

        while (true) {
            char *end;
            long value = strtol(p, &end, 10);
            if (p == end) break; 

            if (has_tmp) sum += tmp;
            tmp = value;
            has_tmp = true;

            p = end;
        }

        if (!has_tmp) continue; 
        
        if (tmp != sum) {
            fprintf(stderr, "%d: %ld %ld\n", 
                    line, sum, tmp);
            errors++;
        }
    }

    return errors;
}