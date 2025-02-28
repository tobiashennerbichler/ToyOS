#include "string.h"

size_t strlen(const char *s) {
    const char *start = s;
    while(*s != 0) {
        s++;
    }
    return (size_t)(s - start);
}

int strcmp(const char *s1, const char *s2) {
    size_t len_s1 = strlen(s1);
    size_t len_s2 = strlen(s2);
    size_t min_len = (len_s1 > len_s2) ? len_s2 : len_s1;
    return strncmp(s1, s2, min_len);
}

int strncmp(const char *s1, const char *s2, size_t n) {
    for(size_t i = 0; i < n; i++) {
        if(s1[i] != s2[i]) {
            unsigned char uchar1 = (unsigned char) s1[i];
            unsigned char uchar2 = (unsigned char) s2[i];
            return (uchar1 > uchar2) ? 1 : -1;
        }
    }

    return 0;
}

char *strcpy(char *restrict dst, const char *restrict src) {
    size_t len = strlen(src);
    for(size_t i = 0; i < len; i++) {
        dst[i] = src[i];
    }
    dst[len] = 0;
    return dst;
}

char *strcat(char *restrict dst, const char *restrict src) {
    size_t src_len = strlen(src);
    size_t dst_len = strlen(dst);
    for(size_t i = 0; i < src_len; i++) {
        dst[dst_len + i] = src[i];
    }
    dst[dst_len + src_len] = 0;
    return dst;
}

uint8_t *memset(uint8_t *s, uint8_t c, size_t n) {
    for(size_t i = 0; i < n; i++) {
        s[i] = c;
    }
    return s;
}

uint8_t *memcpy(uint8_t *restrict dest, const uint8_t *src, size_t n) {
    for(size_t i = 0; i < n; i++) {
        dest[i] = src[i];
    }
    return dest;
}

uint8_t *memmove(uint8_t *dest, const uint8_t *src, size_t n) {
    return dest;
}