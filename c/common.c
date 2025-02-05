#include "common.h"

/* --------- MEMORY OPERATIONS --------- */

// Copy `n` bytes from `src` to `dst`
void *memcpy(void *dst, const void *src, size_t n) {
  uint8_t *d = (uint8_t *)dst;
  const uint8_t *s = (const uint8_t *)src;
  while (n--)
    *d++ = *s++;
  return dst;
}

// Fill the first `n` byutes of `buf` with `c`
void *memset(void *buf, char c, size_t n) {
  uint8_t *p = (uint8_t *)buf;
  while (n--)
    // This is equivalent to
    // *p = c; and
    // p = p + 1;
    *p++ = c;
  return buf;
}

/* --------- STRING OPERATIONS --------- */

// Copies the string from `src` to `dst`
char *strcpy(char *dst, const char *src) {
  char *d = dst;
  while (*src)
    *d++ = *src++;
  *d = '\0';
  return dst;
}

// Compare `s1` and `s2`, and returns
//
// - 0 if `s1` == `s2`,
// - a positive value if `s1` > `s2`, or
// - a negative value if `s1` < `s2`.
int strcmp(const char *s1, const char *s2) {
  while (*s1 && *s2) {
    if (*s1 != *s2)
      break;
    s1++;
    s2++;
  }
  return *(unsigned char *)s1 - *(unsigned char *)s2;
}

void putchar(char ch);

void printf(const char *fmt, ...) {
  va_list vargs;
  va_start(vargs, fmt);

  while (*fmt) {
    // Handle format strings
    if (*fmt == '%') {
      fmt++;
      switch (*fmt) {
      case '\0':
        putchar('%');
        goto end;
      case '%':
        putchar('%');
        break;
      case 's': {
        const char *s = va_arg(vargs, const char *);
        while (*s) {
          putchar(*s);
          s++;
        }
        break;
      }
      case 'd': {
        int value = va_arg(vargs, int);
        // Handle negative numbers
        if (value < 0) {
          putchar('-');
          value = -value;
        }

        int divisor = 1;
        while (value / divisor > 9)
          divisor *= 10;
        while (divisor > 0) {
          putchar('0' + value / divisor);
          value %= divisor;
          divisor /= 10;
        }
        break;
      }

        // Handle hex
        // TODO: How the fuck does this work?
      case 'x': {
        int value = va_arg(vargs, int);
        for (int i = 7; i >= 0; i--) {
          int nibble = (value >> (i * 4)) & 0xf;
          putchar("0123456789abcdef"[nibble]);
        }
      }
      }
    } else {
      putchar(*fmt);
    }
    fmt++;
  }
end:
  va_end(vargs);
}
