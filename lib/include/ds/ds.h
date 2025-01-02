/*
* Generic Data Structures (Shared)
*/

#ifndef _DATASTRUCTURES_H_
#define _DATASTRUCTURES_H_

/* Includes */
//#include <crtdefs.h>
#include <stddef.h>

/* The definition of a key
 * in generic data-structures this can be values or data */
typedef union _DataKey {
	int 		 Value;
	void 		*Pointer;
	char 		*String;
} DataKey_t;

/* This enumeration denotes
 * the kind of key that is to be interpreted by the
 * data-structure */
typedef enum _KeyType {
	KeyInteger,
	KeyPointer,
	KeyString
} KeyType_t;

/* Data-structure locking 
 * primitive definition */
//#include <os/spinlock.h>

/* This is used by data-structures 
 * to allocate memory, since it will 
 * be different for kernel/clib */
void* dsalloc(size_t size);

void dsfree(void *p);

/* Helper Function 
 * Matches two keys based on the key type 
 * returns 0 if they are equal, or -1 if not */
int dsmatchkey(KeyType_t KeyType, DataKey_t Key1, DataKey_t Key2);

/* Helper Function
 * Used by sorting, it compares to values
 * and returns 1 if 1 > 2, 0 if 1 == 2 and
 * -1 if 2 > 1 */
int dssortkey(KeyType_t KeyType, DataKey_t Key1, DataKey_t Key2);

#endif //!_DATASTRUCTURES_H_

