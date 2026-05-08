/**
 * @file io.c
 * @brief I/O module C implementation — file operations, random numbers, and time functions
 * @author Jared Bruni
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int64_t fsize(FILE *fptr) {
    fseek(fptr, 0, SEEK_END);
    int64_t total = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    return total;
}

int64_t seed_random() {
    unsigned int seed = (unsigned int)time(NULL) ^ (uintptr_t)&seed;
    srand(seed);
    return 0;
}

int64_t rand_number(int64_t size) {
    return rand() % size;
}

char *mxvm_fgets(FILE *fp) {
    static char buf[4096];
    char *res = fgets(buf, sizeof(buf), fp);
    if (res) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n')
            buf[len - 1] = '\0';
        return strdup(buf);
    }
    return strdup("");
}
