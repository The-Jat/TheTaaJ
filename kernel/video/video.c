#include <stdint.h>
#include <stdlib.h>
#include <kernel_utilities.h>
#include <defs.h>
#include <video/video.h>
#include <video/vbe.h>
#include <datastructure.h>
#include <video/vga.h>

static Terminal_t* s_videoTerminal = NULL;

void VideoSetTerminal(Terminal_t* terminal) {
	s_videoTerminal = terminal;
}

/* VideoDrawPixel
 * Draws a pixel of the given color at the specified
 * pixel-position
 */
OsStatus_t VideoDrawPixel(unsigned X, unsigned Y, uint32_t Color) {
	// Start out by determining the kind
	// of draw we want to do
	switch (VideoGetTerminal()->Type)
	{
	// Text-Mode
	case VIDEO_TEXT:			  // FG  ,  BG
		return VgaDrawPixel(X, Y, vga_color(BLACK, LIGHT_MAGENTA));

	// VBE
	case VIDEO_GRAPHICS:
		return VesaDrawPixel(X, Y, Color);

	// No video?
	case VIDEO_NONE:
		return Error;
	}

	// Uh?
	return Error;
}


/* VideoDrawLine
 * Draw's a line from (StartX, StartY) -> (EndX, EndY) 
 * with the given color
 */
void VideoDrawLine(unsigned StartX, unsigned StartY, unsigned EndX, unsigned EndY, unsigned Color) {
	// Variables - clam some values
	int dx = abs(EndX - StartX);
	int sx = StartX < EndX ? 1 : -1;
	int dy = abs(EndY - StartY);
	int sy = StartY < EndY ? 1 : -1;
	int err = (dx > dy ? dx : -dy) / 2, e2;

	// Draw the line by brute force
	for (;;) {
		OsStatus_t status = VideoDrawPixel(StartX, StartY, Color);
		if (StartX == EndX && StartY == EndY) break;
		e2 = err;
		if (e2 >-dx) { err -= dy; StartX += sx; }
		if (e2 < dy) { err += dx; StartY += sy; }
	}
}


/* VideoDrawCharacter
 * Renders a character of the given color(s) 
 * at the specified pixel-position
 */
OsStatus_t VideoDrawCharacter(unsigned X, unsigned Y, int Character, uint32_t Bg, uint32_t Fg) {
	// Start out by determining the kind
	// of draw we want to do
	switch (VideoGetTerminal()->Type)
	{
		// Text-Mode
	case VIDEO_TEXT:
		// unsigned char Foreground = LOBYTE(LOWORD(Fg));
		// unsigned char Background = LOBYTE(LOWORD(Bg));
		return TextDrawCharacter(Character, Y, X, vga_color(BLACK, LIGHT_MAGENTA));

		// VBE
	case VIDEO_GRAPHICS:
		return VesaDrawCharacter(X, Y, Character, Fg, Bg);

		// No video?
	case VIDEO_NONE:
		return Error;
	}

	return Error;
}


/* VesaPutCharacter
 * Uses the vesa-interface to print a new character
 * at the current terminal position
 */
OsStatus_t VesaPutCharacter(int Character) {

	// The first step is to handle special
	// case characters that we shouldn't print out
	switch (Character) 
	{
		// New-Line
		// Reset X and increase Y
	case '\n': {
		s_videoTerminal->CursorX = s_videoTerminal->CursorStartX;
		s_videoTerminal->CursorY += FontHeight;
	} break;

	// Carriage Return
	// Reset X don't increase Y
	case '\r': {
		s_videoTerminal->CursorX = s_videoTerminal->CursorStartX;
	} break;

	// Default
	// Printable character
	default: {
		// Call print with the current location
		// and use the current colors
		VesaDrawCharacter(s_videoTerminal->CursorX, s_videoTerminal->CursorY,
			Character, s_videoTerminal->FgColor, s_videoTerminal->BgColor);
		s_videoTerminal->CursorX += (FontWidth + 2);
	} break;
	}

	// Next step is to do some post-print
	// checks, including new-line and scroll-checks

	// Are we at last X position? - New-line
	if ((s_videoTerminal->CursorX + (FontWidth + 2)) >= s_videoTerminal->CursorLimitX) {
		s_videoTerminal->CursorX = s_videoTerminal->CursorStartX;
		s_videoTerminal->CursorY += FontHeight;
	}

	// Do we need to scroll the terminal?
	if ((s_videoTerminal->CursorY + FontHeight) >= s_videoTerminal->CursorLimitY) {
		VideoScroll(1);
	}

	return Success;
}


void VideoDrawString(const char* str) {
	while(*str != '\0') {
		switch(s_videoTerminal->Type)
		{
		case VIDEO_TEXT:
			VGAPutCharacter(*str);
			break;
		case VIDEO_GRAPHICS:
			VesaPutCharacter(*str);
			break;
		}
		str++;
	}
}


/* VesaScroll
 * Scrolls the terminal <n> lines up by using the
 * vesa-interface */
OsStatus_t VideoScroll(int ByLines) {
	// Variables
	uint8_t *VideoPtr = NULL;
	size_t BytesToCopy = 0;
	int Lines = 0;
	int i = 0, j = 0;

	// How many lines do we need to modify?
	Lines = (VideoGetTerminal()->CursorLimitY - VideoGetTerminal()->CursorStartY);

	// Calculate the initial screen position
	VideoPtr = (uint8_t*)(VideoGetTerminal()->Info.FrameBufferAddress +
		((VideoGetTerminal()->CursorStartY * VideoGetTerminal()->Info.BytesPerScanline)
			+ (VideoGetTerminal()->CursorStartX * (VideoGetTerminal()->Info.Depth / 8))));

	// Calculate num of bytes
	BytesToCopy = ((VideoGetTerminal()->CursorLimitX - VideoGetTerminal()->CursorStartX)
		* (VideoGetTerminal()->Info.Depth / 8));

	// Do the actual scroll
	for (i = 0; i < ByLines; i++) {
		for (j = 0; j < Lines; j++) {
			memcpy(VideoPtr, VideoPtr +
				(VideoGetTerminal()->Info.BytesPerScanline * FontHeight), BytesToCopy);
			VideoPtr += VideoGetTerminal()->Info.BytesPerScanline;
		}
	}

	// Clear out the lines that was scrolled
	VideoPtr = (uint8_t*)(VideoGetTerminal()->Info.FrameBufferAddress +
		((VideoGetTerminal()->CursorStartX * (VideoGetTerminal()->Info.Depth / 8))));

	// Scroll pointer down to bottom - n lines
	VideoPtr += (VideoGetTerminal()->Info.BytesPerScanline 
		* (VideoGetTerminal()->CursorLimitY - (FontHeight * ByLines)));

	// Clear out lines
	for (i = 0; i < ((int)FontHeight * ByLines); i++) {
		memset(VideoPtr, 0xFF, BytesToCopy);
		VideoPtr += VideoGetTerminal()->Info.BytesPerScanline;
	}

	// We did the scroll, modify cursor
	VideoGetTerminal()->CursorY -= (FontHeight * ByLines);

	// No errors
	return Success;
}


/* TextDrawCharacter
 * Renders an ASCII character at the given text-position
 * on the screen by the given color combination
 */
OsStatus_t TextDrawCharacter(int Character, unsigned CursorY, unsigned CursorX, uint8_t Color) {
	// Variables
	uint16_t *Video = NULL;
	uint16_t Data = ((uint16_t)Color << 8) | (uint8_t)(Character & 0xFF);

	// Calculate video position
	Video = (uint16_t*)VideoGetTerminal()->Info.FrameBufferAddress +
		(CursorY * VideoGetTerminal()->Info.Width + CursorX);

	// Plot it on the screen
	*Video = Data;

	// Done - no errors
	return Success;
}

inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ __volatile__("inb %1, %0" : "=a"(result) : "dN"(port));
    return result;
}

inline unsigned short inw(unsigned short port) {
    unsigned short result;
    __asm__ __volatile__("inw %1, %0" : "=a"(result) : "dN"(port));
    return result;
}

inline void outb(unsigned short port, unsigned char data) {
    __asm__ __volatile__("outb %0, %1" : : "a"(data), "dN"(port));
}

inline void outw(unsigned short port, unsigned short data) {
    __asm__ __volatile__("outw %0, %1" : : "a"(data), "dN"(port));
}

inline void inwm(unsigned short port, unsigned char * data, unsigned long size) {
	asm volatile ("rep insw" : "+D" (data), "+c" (size) : "d" (port) : "memory");
}


/* TextScroll
 * Scrolls the terminal <n> lines up by using the
 * text-interface */
OsStatus_t  TextScroll(int ByLines) {
	// Variables
	uint16_t *Video = (uint16_t*)VideoGetTerminal()->Info.FrameBufferAddress;
	uint16_t Color = (uint16_t)(VideoGetTerminal()->FgColor << 8);
	unsigned i;
	int j;

	// Move display n lines up
	for (j = 0; j < ByLines; j++) {
		for (i = 0; i < (VideoGetTerminal()->Info.Height - 1) * VideoGetTerminal()->Info.Width;
			i++) {
			Video[i] = Video[i + VideoGetTerminal()->Info.Width];
		}

		// Clear last line
		for (i = ((VideoGetTerminal()->Info.Height - 2) * VideoGetTerminal()->Info.Width);
			i < (VideoGetTerminal()->Info.Height * VideoGetTerminal()->Info.Width);
			i++) {
			Video[i] = (uint16_t)(Color | ' ');
		}
	}

	// Update new Y cursor position
	VideoGetTerminal()->CursorY = (VideoGetTerminal()->Info.Height - ByLines);

	// Done - no errors
	return Success;
}


/* TextPutCharacter
 * Uses the text-interface to print a new character
 * at the current terminal position */
OsStatus_t VGAPutCharacter(int Character) {
	// Variables
	uint16_t CursorLoc = 0;

	// Special case characters
	// Backspace
	if (Character == 0x08 && VideoGetTerminal()->CursorX)
		VideoGetTerminal()->CursorX--;

	// Tab
	else if (Character == 0x09)
		VideoGetTerminal()->CursorX = ((VideoGetTerminal()->CursorX + 8) & ~(8 - 1));

	// Carriage Return
	else if (Character == '\r')
		VideoGetTerminal()->CursorX = 3;

	// New Line
	else if (Character == '\n') {
		VideoGetTerminal()->CursorX = 3;
		VideoGetTerminal()->CursorY++;
	}
	
	// Printable characters
	else if (Character >= ' ') {
		TextDrawCharacter(Character, VideoGetTerminal()->CursorY, 
			VideoGetTerminal()->CursorX, LOBYTE(LOWORD(VideoGetTerminal()->FgColor)));
		VideoGetTerminal()->CursorX++;
	}

	// Go to new line?
	if (VideoGetTerminal()->CursorX >= VideoGetTerminal()->Info.Width) {
		VideoGetTerminal()->CursorX = 0;
		VideoGetTerminal()->CursorY++;
	}

	// Scroll if at last line
	if (VideoGetTerminal()->CursorY >= (VideoGetTerminal()->Info.Height-1)) {
		// TODO
		// VGA scroll is not working perfectly.
		TextScroll(1);
	}

	// Update HW Cursor
	CursorLoc = (uint16_t)((VideoGetTerminal()->CursorY * VideoGetTerminal()->Info.Width) 
		+ VideoGetTerminal()->CursorX);

	// Send the high byte.
	outb(0x3D4, 14);
	outb(0x3D5, (uint8_t)(CursorLoc >> 8));

	// Send the low byte.
	outb(0x3D4, 15);
	outb(0x3D5, (uint8_t)CursorLoc);


	return Success;
}


Terminal_t*
VideoGetTerminal()
{
	// Simply return the static pointer instance.
	return s_videoTerminal;
}
