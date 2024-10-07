#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

size_t intToString(long value, char *buffer, size_t buffer_size) {
    char temp[22]; 
    memset(temp, 0, 22);
    int i = 0;
    bool isNegative = false;

    if (value < 0) {
        isNegative = true;
        value = -value;
    }

    do {
        temp[i++] = (value % 10) + '0';
        value /= 10;
    } while (value > 0);

    if (isNegative) {
        temp[i++] = '-';
    }

    if (i >= buffer_size) {
        return 0;
    }
    memset(buffer, 0, buffer_size);
    size_t length = i;
    for (int j = 0; j < i; j++) {
        buffer[j] = temp[i - j - 1];
    }
    buffer[length] = '\0';
    return length;
}


void *str(long value) {
    size_t buffer_size = 21;
    char *buf = (char *)malloc(buffer_size+1);
    if (buf == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in str\n");
        exit(EXIT_FAILURE);
    }
    intToString(value, buf, buffer_size);
    return (void*)buf;
}

long at(char *buffer, long value) {
    if(buffer != NULL) {
        if(value < strlen(buffer)) {
            return (long)buffer[value];
        }
    }
    return -1;
}

void memclr(void *buffer, long size) {
    memset(buffer, 0, size);
}

long mematl(void *buffer, long index) {
    if(buffer != NULL) {
        long *values = (long *)buffer;
        return *(values + (index));
    }
    return -1;
}

long mematb(void *buffer, long index) {
    if(buffer != NULL) {
        char *ptr = (char*)buffer;
        char c = ptr[index];
        return (long)c;
    }
    return -1;
}


void memstorel(void *buffer, long index, long value) {
    if(buffer != NULL) {
        long *ptr = (long*)buffer;
        ptr[index] = value;
    }
}
void memstoreb(void *buffer, long index, long value) {
    if(buffer != NULL) {
        char *ptr = (char*)buffer;
        ptr[index] = (char) value;
    }
}

void *meminsert(void *buffer, long index, long value, size_t element_size, long *current_size) {
    if (buffer == NULL || index < 0 || index > *current_size) {
        fprintf(stderr, "Error: Invalid index or buffer is null in meminsert\n");
        exit(EXIT_FAILURE);
    }
    void *new_buffer = realloc(buffer, (*current_size + 1) * element_size);
    if (new_buffer == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in meminsert\n");
        exit(EXIT_FAILURE);
    }
    memmove((char*)new_buffer + (index + 1) * element_size, (char*)new_buffer + index * element_size, (*current_size - index) * element_size);
    memcpy((char*)new_buffer + index * element_size, &value, element_size);
    (*current_size)++;
    return new_buffer;
}

void *memremove(void *buffer, size_t index, size_t element_size, size_t *current_size) {
    if (buffer == NULL || index >= *current_size) {
        fprintf(stderr, "Error: Invalid index or buffer is null in memremove\n");
        exit(EXIT_FAILURE);
    }
   memmove((char *)buffer + index * element_size, 
            (char *)buffer + (index + 1) * element_size, 
            (*current_size - index - 1) * element_size);
    void *new_buffer = realloc(buffer, (*current_size - 1) * element_size);
    if (new_buffer == NULL && *current_size > 1) {
        fprintf(stderr, "Error: Memory reallocation failed in memremove\n");
        exit(EXIT_FAILURE);
    }
    *current_size -= 1;
    return new_buffer;
}
long scan_integer() {
    long value;
    if (scanf("%ld", &value) != 1) {
        fprintf(stderr, "Error: Failed to read integer in scan_integer\n");
        exit(EXIT_FAILURE);
    }
    return value;
} 

long len(void *ptr) {
    if(ptr == NULL) {
        fprintf(stderr, "Error: len function pass null pointer\n");
        exit(EXIT_FAILURE);
    }
    return strlen((char*)ptr);
}

char *charAt(const char *src, long index) {
    char *buf = malloc(2);
    buf[0] = src[index];
    buf[1] = 0;
    return buf;
}

void *ptr(const char *s) {
    return (void*)s;
}

char *string(void *s) {
    return (char*)s;
}