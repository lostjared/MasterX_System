/**
 * @file std.c
 * @brief Standard library module C implementation — math, string conversion, and utility functions
 * @author Jared Bruni
 */
// ...existing code...
#include "mx_std.h"
#include <stdlib.h>
#include <string.h>

static int g_argc = 0;
static char **g_argv = NULL;
static int g_args_registered = 0;

static char *mx_strdup_c(const char *s) {
    size_t len = strlen(s) + 1;
    char *d = (char *)malloc(len);
    if (d)
        memcpy(d, s, len);
    return d;
}

void free_program_args(void) {
    if (g_argv) {
        int i;
        for (i = 0; i < g_argc; ++i) {
            free(g_argv[i]);
        }
        free(g_argv);
        g_argv = NULL;
        g_argc = 0;
    }
}

void set_program_args(int argc, const char **argv) {
    int i;
    if (g_argv) {
        free_program_args();
    }

    if (argc <= 0 || argv == NULL) {
        return;
    }

    g_argv = (char **)malloc(sizeof(char *) * (size_t)argc);
    if (!g_argv) {
        g_argc = 0;
        return;
    }

    for (i = 0; i < argc; ++i) {
        const char *s = argv[i] ? argv[i] : "";
        g_argv[i] = mx_strdup_c(s);
        if (!g_argv[i]) {
            int j;
            for (j = 0; j < i; ++j)
                free(g_argv[j]);
            free(g_argv);
            g_argv = NULL;
            g_argc = 0;
            return;
        }
    }
    g_argc = argc;

    if (!g_args_registered) {
        // atexit(mxvm_free_program_args);
        g_args_registered = 1;
    }
}

int argc(void) {
    return g_argc;
}

const char *argv(int idx) {
    if (idx < 0 || idx >= g_argc)
        return NULL;
    return g_argv[idx];
}

int64_t float_to_int(double d) {
    return (int64_t)d;
}

double int_to_float(int64_t i) {
    return (double)i;
}

void release(void *ptr) {
    if (ptr != NULL)
        free(ptr);
}