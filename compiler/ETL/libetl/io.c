#include<stdio.h>
#include<stdlib.h>

void *file_open(const char *src, const char *op) {
    FILE *fptr = fopen(src, op);
    if(!fptr) {
        fprintf(stderr, "Error openining file: %s\n", src);
        exit(EXIT_FAILURE);
    }
    return (void*)fptr;
}

void file_print(void *f, const char *data) {
    fprintf((FILE*)f, "%s", data);
}

long file_write(void *f, void *data, long size) {
    return fwrite(data, size, 1, (FILE*)f);
}

long file_read(void *f, void *data, long size) {
    return fread(data, size, 1, (FILE*)f);
}

long file_seek(void *f, long offset, long origin) {
    return fseek((FILE*)f, offset, (int)origin);
}

void file_close(void *f) {
    fclose((FILE*)f);
}

long file_size(void *f) {
    fseek((FILE*)f, 0, SEEK_END);
    long len = ftell((FILE*)f);
    rewind((FILE*)f);
    return len;
}