#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>
#include <stdint.h>

size_t strlen(const char *s);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strcpy(char *restrict dst, const char *restrict src);
char *strcat(char *restrict dst, const char *restrict src);

uint8_t *memset(uint8_t *s, uint8_t c, size_t n);
uint8_t *memcpy(uint8_t *restrict dest, const uint8_t *src, size_t n);
uint8_t *memmove(uint8_t *dest, const uint8_t *src, size_t n);

#endif