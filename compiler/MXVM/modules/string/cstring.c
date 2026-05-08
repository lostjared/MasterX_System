/**
 * @file cstring.c
 * @brief String module C implementation — string manipulation, comparison, and formatting
 * @author Jared Bruni
 */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int64_t strfind(const char *src, const char *search, long n) {
    if (!src || !search || n < 0)
        return -1;
    size_t src_len = strlen(src);
    if ((size_t)n > src_len)
        return -1;
    const char *start = src + n;
    const char *found = strstr(start, search);
    if (!found)
        return -1;
    return (long)(found - src);
}

int64_t substr(char *dest, int64_t size, const char *src, int64_t pos, int64_t len) {
    if (!dest || !src || size <= 0 || pos < 0 || len < 0)
        return 0;
    size_t src_len = strlen(src);
    if ((size_t)pos > src_len)
        return 0;
    size_t copy_len = (size_t)len;
    if (pos + copy_len > src_len)
        copy_len = src_len - pos;
    if (copy_len >= (size_t)size)
        copy_len = (size_t)size - 1;
    memcpy(dest, src + pos, copy_len);
    dest[copy_len] = '\0';
    return (long)copy_len;
}

int64_t strat(const char *src, int64_t pos) {
    if (!src || pos < 0)
        return 0;
    size_t len = strlen(src);
    if ((size_t)pos >= len)
        return 0;
    char c = *(src + pos);
    return (int64_t)c;
}

int64_t pos(const char *substr, const char *s) {
    if (!substr || !s)
        return 0;
    const char *found = strstr(s, substr);
    if (!found)
        return 0;
    return (int64_t)(found - s) + 1;
}

char *copy(const char *s, int64_t index, int64_t count) {
    if (!s || index < 1 || count < 0)
        return NULL;

    size_t s_len = strlen(s);
    size_t start_pos = (size_t)index - 1;
    if (start_pos > s_len)
        return NULL;

    size_t copy_len = (size_t)count;
    if (start_pos + copy_len > s_len) {
        copy_len = s_len - start_pos;
    }

    char *new_buf = (char *)malloc(copy_len + 1);
    if (!new_buf)
        return NULL;

    memcpy(new_buf, s + start_pos, copy_len);
    new_buf[copy_len] = '\0';
    return new_buf;
}

char *insert(const char *source, const char *dest, int64_t index) {
    if (!source || !dest || index < 1)
        return NULL;

    size_t source_len = strlen(source);
    size_t dest_len = strlen(dest);
    size_t insert_pos = (size_t)index - 1;
    if (insert_pos > dest_len)
        insert_pos = dest_len;

    size_t new_len = source_len + dest_len;
    char *new_buf = (char *)malloc(new_len + 1);
    if (!new_buf)
        return NULL;

    memcpy(new_buf, dest, insert_pos);
    memcpy(new_buf + insert_pos, source, source_len);
    memcpy(new_buf + insert_pos + source_len, dest + insert_pos, dest_len - insert_pos);

    new_buf[new_len] = '\0';
    return new_buf;
}

char *delete_str(const char *s, int64_t index, int64_t count) {
    if (!s || index < 1 || count < 0)
        return NULL;

    size_t s_len = strlen(s);
    size_t start_pos = (size_t)index - 1;
    if (start_pos >= s_len) {
        char *new_buf = (char *)malloc(s_len + 1);
        if (!new_buf)
            return NULL;
        strcpy(new_buf, s);
        return new_buf;
    }

    size_t del_count = (size_t)count;
    if (start_pos + del_count > s_len) {
        del_count = s_len - start_pos;
    }

    size_t new_len = s_len - del_count;
    char *new_buf = (char *)malloc(new_len + 1);
    if (!new_buf)
        return NULL;

    memcpy(new_buf, s, start_pos);
    memcpy(new_buf + start_pos, s + start_pos + del_count, s_len - (start_pos + del_count));
    new_buf[new_len] = '\0';
    return new_buf;
}

char *inttostr(int64_t value) {
    char *buf = (char *)malloc(21);
    if (!buf)
        return NULL;
    snprintf(buf, 21, "%lld", value);
    return buf;
}

int64_t strtoint(const char *s) {
    if (!s)
        return 0;
    return atoll(s);
}
