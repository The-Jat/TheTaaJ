#include <mem.h>


/*
 * memcpy
 * Set `count` bytes to `val`.
 */
// Byte Version of Memcpy
void *memcpyb(void *dest, const void *src, size_t n) {

	unsigned char *d = (unsigned char *)dest;
	const unsigned char *s = (const unsigned char *)src;

	// Copy bytes from src to dest
	while (n--) {
		*d++ = *s++;
	}
    
	// OR
	/*
	for(int i = 0; i< count; i++)
	{
		dest[i] = src[i];
	}
	*/

	return dest;
}


/*
 * memsetb
 * Set `count` bytes to `val`.
 */
void *memsetb(void *dest, int c, int n) {

	unsigned char *p = (unsigned char *)dest;

	while (n--) {
		*p++ = (unsigned char)c;
	}
	// OR
	/*
	for(int i = 0; i < n; i++){
		dest[i] = val;
	}
	*/
	
	return dest;
}
