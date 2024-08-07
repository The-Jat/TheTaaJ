#ifndef __DEFS_H__
#define __DEFS_H__

#include <stdint.h>

typedef uint32_t                    reg32_t;
typedef reg32_t                     reg_t;

// Variable Width
#if defined(_X86_32) || defined(i386)
	#define __BITS                      32
	#define __MASK                      0xFFFFFFFF
	//typedef reg32_t                     reg_t;
#elif defined(_X86_64)
	#define __BITS                      64
	#define __MASK                      0xFFFFFFFFFFFFFFFF
	//typedef reg64_t                     reg_t;
#endif

typedef enum {
	Success,
	Error
} OsStatus_t;

#ifndef LOBYTE
	#define LOBYTE(l)	((uint8_t)(uint16_t)(l))
#endif

/* Data manipulation macros */
#ifndef LOWORD
#define LOWORD(l)                               ((uint16_t)(uint32_t)(l))
#endif


#endif
