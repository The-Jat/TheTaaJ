#include <string.h>


char *strcat(char *destination, const char *source) {
    // Find the end of the destination string
    char *dest_end = destination;
    while (*dest_end != '\0') {
        dest_end++;
    }

    // Append the source string to the destination string
    while (*source != '\0') {
        *dest_end = *source;
        dest_end++;
        source++;
    }
    
    // Null-terminate the concatenated string
    *dest_end = '\0';

    return destination;
}


void *memset(void *dest, int c, size_t count)
{
	char *s = (char *)dest;
	int i;
	unsigned long buffer;
	unsigned long *aligned_addr;
	unsigned int d = c & 0xff;	/* To avoid sign extension, copy C to an
					unsigned variable.  */

	while (UNALIGNED (s))
	{
		if (count--)
			*s++ = (char) c;
		else
			return dest;
	}

	if (!TOO_SMALL (count))
	{
		/* If we get this far, we know that n is large and s is word-aligned. */
		aligned_addr = (unsigned long *) s;

		/* Store D into each char sized location in BUFFER so that
			we can set large blocks quickly.  */
		buffer = (d << 8) | d;
		buffer |= (buffer << 16);
		for (i = 32; i < LBLOCKSIZE * 8; i <<= 1)
			buffer = (buffer << i) | buffer;

		/* Unroll the loop.  */
		while (count >= LBLOCKSIZE*4)
		{
			*aligned_addr++ = buffer;
			*aligned_addr++ = buffer;
			*aligned_addr++ = buffer;
			*aligned_addr++ = buffer;
			count -= 4*LBLOCKSIZE;
		}

		while (count >= LBLOCKSIZE)
		{
			*aligned_addr++ = buffer;
			count -= LBLOCKSIZE;
		}
		/* Pick up the remainder with a bytewise loop.  */
		s = (char*)aligned_addr;
	}

	while (count--)
		*s++ = (char) c;

	return dest;
}

