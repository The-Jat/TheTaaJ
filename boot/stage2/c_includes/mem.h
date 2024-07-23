#ifndef _MEM_H_
#define _MEM_H_

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long int uint64_t;

typedef int size_t;

extern void *memcpyb(void *dest, const void *src, size_t n);
extern void *memsetb(void *dest, int c, int n);

extern int strcmp(const char * l, const char * r);

extern char * strchr(const char * s, int c);

#endif _MEM_H_

