#include <print.h>

/*
 * Text pointer, background, foreground
 */
unsigned short * textmemptr = (unsigned short *)0xB8000; //VGA Memory Address

int attrib = 0x0F;
int csr_x = 0, csr_y = 0;

typedef enum {
	true = 1,
	false = 0
} bool;

/*
 * inportb
 * Read from an I/O port.
 */
unsigned char inportb(unsigned short _port) {
	unsigned char rv;
	__asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
	return rv;
}

/*
 * outportb
 * Write to an I/O port.
 */
void outportb( unsigned short _port, unsigned char _data) {
	__asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

unsigned char *memcpy(unsigned char *dest, const unsigned char *src, int count) {
	int i;
	i = 0;
	for ( ; i < count; ++i ) {
		dest[i] = src[i];
		
	}
	return dest;
}

/*
 * memset
 * Set `count` bytes to `val`.
 */
unsigned char *memset(unsigned char *dest, unsigned char val, int count) {
	int i;
	i = 0;
	for ( ; i < count; ++i ) {
		dest[i] = val;
	}
	return dest;
}

/*
 * memsetw
 * Set `count` shorts to `val`.
 */
unsigned short *memsetw(unsigned short *dest, unsigned short val, int count) {
	int i;
	i = 0;
	for ( ; i < count; ++i ) {
		dest[i] = val;
	}
	return dest;
}

/*
 * strlen
 * Returns the length of a given `str`.
 */
int strlen(const char *str) {
	int i = 0;
	while (str[i] != (char)0) {
		++i;
	}
	return i;
}

/*
 * scroll
 * Scroll the screen
 */
void scroll() {
	unsigned blank, temp;
	blank = 0x20 | (attrib << 8);
	if (csr_y >= 25) {
		/*
		 * Move the current text chunk that makes up the screen
		 * back in the buffer by one line.
		 */
		temp = csr_y - 25 + 1;
		memcpy(textmemptr, textmemptr + temp * 80, (25 - temp) * 80 * 2);
		/*
		 * Set the chunk of memory that occupies
		 * the last line of text to the blank character
		 */
		memsetw(textmemptr + (25 - temp) * 80, blank, 80);
		csr_y = 25 - 1;
	}
}

/*
 * move_csr
 * Update the hardware cursor
 */
void move_csr() {
	unsigned temp;
	temp = csr_y * 80 + csr_x;
	
	/*
	 * Write stuff out.
	 */
	outportb(0x3D4, 14);
	outportb(0x3D5, temp >> 8);
	outportb(0x3D4, 15);
	outportb(0x3D5, temp);
}

/*
 * cls
 * Clear the screen
 */
void cls() {
	unsigned blank;
	int i;
	blank = 0x20 | (attrib << 8);
	for (i = 0; i < 25; ++i) {
		memsetw(textmemptr + i * 80, blank, 80);
	}
	csr_x = 0;
	csr_y = 0;
	move_csr();
}

/*
 * putch
 * Puts a character to the screen
 */
void putch(unsigned char c, unsigned short color) {
	unsigned short *where;
	unsigned att = color << 8;
	if (c == 0x08) {
		/* Backspace */
		if (csr_x != 0) csr_x--;
	} else if (c == 0x09) {
		/* Tab */
		csr_x = (csr_x + 8) & ~(8 - 1);
	} else if (c == '\r') {
		/* Carriage return */
		csr_x = 0;
	} else if (c == '\n') {
		/* New line */
		csr_x = 0;
		csr_y++;
	} else if (c >= ' ') {
		where = textmemptr + (csr_y * 80 + csr_x);
		*where = c | att;
		csr_x++;
	}

	if (csr_x >= 80) {
		csr_x = 0;
		csr_y++;
	}
	scroll();
	move_csr();
}

/*
 * puts
 * Put string to screen
 */
void puts(unsigned char * text, enum vga_color fg, enum vga_color bg) {
	int i;
	int len = strlen(text);
	
	unsigned short color = fg | bg << 4;
	
	for (i = 0; i < len; ++i) {
		putch(text[i], color);
	}
}


void placech(unsigned char c, int x, int y, int attr) {
    // Declare a pointer to an unsigned short.
    unsigned short *where;

    // Shift the attribute byte left by 8 bits to make room for the character in the lower byte.
    unsigned att = attr << 8;

    // Calculate the memory address based on the x and y coordinates.
    // textmemptr is assumed to be a pointer to the start of the video memory.
    where = textmemptr + (y * 80 + x);

    // Place the character and attribute at the calculated memory location.
    *where = c | att;
}

int x = 0;
int y = 0;
int attr = 0x07;

void set_attribute(enum vga_color bg, enum vga_color fg) {

	attr = bg << 4 | fg;

}
void boot_scroll(){

//copy each line to the line above
int i;
for ( i = 0 ; i < 24*80; i++){
	textmemptr[i] = textmemptr[i + 80];
}
for(i = 24*80 ; i<25*80; i++){
textmemptr[i] = 0x0F << 8 | ' ';
}


}
void boot_print(char * str) {
	while (*str) {
		if (*str == '\n') {
			for (; x < 80; ++x) {
				placech(' ', x, y, attr);
			}
			x = 0;
			y += 1;
			if (y == 25) {
				boot_scroll();
				y = 24;
			}
		} else {
			placech(*str, x, y, attr);
			x++;
			if (x == 80) {
				x = 0;
				y += 1;
				if (y == 24) {
					y = 0;
				}
			}
		}
		str++;
	}
}

void boot_print_hex(unsigned int value) {
	char out[9] = {0};
	for (int i = 7; i > -1; i--) {
		out[i] = "0123456789abcdef"[(value >> (4 * (7 - i))) & 0xF];
	}
	boot_print(out);
}

void boot_clear(bool useDefaultAttrb) {
	x = 0;
	y = 0;
	for (int y = 0; y < 25; ++y) {
		for (int x = 0; x < 80; ++x) {
			if (useDefaultAttrb == true){
				placech(' ', x, y, 0x00);
			} else {
				placech(' ', x, y, attr);
			}
		}
	}
}

int abc = 123;
int def = 456;
void test(){
	set_attribute(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLUE);
	boot_clear(false);
	boot_print("HI");
	
	boot_print_hex(&abc);
	for (int i = 0; i<22; i++){

	boot_print("\n");
	boot_print_hex(def);
	}
	boot_print("\n");
	boot_print("end");
	
	boot_print("\n");
	boot_print("end2");
	//boot_print("\n");
	//boot_print("end3");

}
