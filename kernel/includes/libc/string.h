#ifndef __STRING_H__
#define __STRING_H__

#include <stdint.h>
#include <stddef.h>

#define LBLOCKSIZE (sizeof(long))
#define UNALIGNED(X)   ((long)X & (LBLOCKSIZE - 1))
#define TOO_SMALL(LEN) ((LEN) < LBLOCKSIZE)


char *strcat(char *destination, const char *source);

void *memset(void *dest, int c, size_t count);

#endif /* __STRING_H__ */
