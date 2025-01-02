/*
 * MollenOS - Generic Data Structures (Shared)
 */
#include <defs.h>
/* Includes 
 * - Library */
#include <ds/ds.h>
#include <string.h>

//#ifdef LIBC_KERNEL
	#include <heap.h>
//#else
//	#include <stdlib.h>
//#endif

/* This is used by data-structures
 * to allocate memory, since it will
 * be different for kernel/clib */
void *dsalloc(size_t size)
{
//#ifdef LIBC_KERNEL
	return kmalloc(size);
//#else
//	return malloc(size);
//#endif
}

/* This is used by data-structures
 * to allocate memory, since it will
 * be different for kernel/clib */
void dsfree(void *p)
{
//#ifdef LIBC_KERNEL
	kfree(p);
//#else
//	free(p);
//#endif
}

/* Helper Function
 * Matches two keys based on the key type
 * returns 0 if they are equal, or -1 if not */
int dsmatchkey(KeyType_t KeyType, DataKey_t Key1, DataKey_t Key2)
{
	switch (KeyType)
	{
		/* Check if ints match */
		case KeyInteger: {
			if (Key1.Value == Key2.Value)
				return 0;
		} break;

		/* Check if pointers match */
		case KeyPointer: {
			if (Key1.Pointer == Key2.Pointer)
				return 0;
		} break;

		/* Check if strings match */
		case KeyString: {
			return strcmp(Key1.String, Key2.String);
		} break;
	}

	/* Damn, no match */
	return -1;
}

/* Helper Function
 * Used by sorting, it compares to values
 * and returns 1 if 1 > 2, 0 if 1 == 2 and
 * -1 if 2 > 1 */
int dssortkey(KeyType_t KeyType, DataKey_t Key1, DataKey_t Key2)
{
	switch (KeyType)
	{
		/* Check if ints match */
		case KeyInteger: {
			if (Key1.Value == Key2.Value)
				return 0;
			else if (Key1.Value > Key2.Value)
				return 1;
			else
				return -1;
		} break;

		/* Sort on pointers? Wtf?
		 * this is impossible */
		case KeyPointer: {
			return 0;
		} break;

		/* Check if strings match 
		 * by using strcmp, it follows
		 * our return policy */
		case KeyString: {
			return strcmp(Key1.String, Key2.String);
		} break;
	}

	/* Damn, no match */
	return 0;
}
