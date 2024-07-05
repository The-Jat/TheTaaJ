#ifndef _MEM_H_
#define _MEM_H_

typedef int size_t;

extern void *memcpyb(void *dest, const void *src, size_t n);
extern void *memsetb(void *dest, int c, int n);

#endif _MEM_H_

