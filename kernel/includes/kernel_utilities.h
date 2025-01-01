#ifndef __KERNEL_UTILITIES_H
#define	__KERNEL_UTILITIES_H
#include <stddef.h>

extern unsigned char *memcpy(unsigned char *dest, const unsigned char *src, int count);
//extern unsigned char *memset(unsigned char *dest, unsigned char val, int count);
extern unsigned short *memsetw(unsigned short *dest, unsigned short val, int count);

extern size_t strlen(const char *str);

extern size_t strnlen(const char *str, size_t maxlen);

extern unsigned char inportb (unsigned short _port);
extern void outportb (unsigned short _port, unsigned char _data);

#endif
