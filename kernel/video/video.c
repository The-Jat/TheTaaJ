#include <stdint.h>
#include <stdlib.h>
#include <defs.h>
#include <video/vbe.h>
#include <datastructure.h>

char *GlbBootVideoWindowTitle = "Welcome to the TaaJ OS.";


/* VideoDrawPixel
 * Draws a pixel of the given color at the specified
 * pixel-position */
OsStatus_t VideoDrawPixel(unsigned X, unsigned Y, uint32_t Color) {
	// Start out by determining the kind
	// of draw we want to do
	switch (VideoGetTerminal()->Type)
	{
	// Text-Mode
	case VIDEO_TEXT:
		return OsError;

	// VBE
	case VIDEO_GRAPHICS:
		return VesaDrawPixel(X, Y, Color);

	// No video?
	case VIDEO_NONE:
		return OsError;
	}

	// Uh?
	return OsError;
}


/* VideoDrawLine
 * Draw's a line from (StartX, StartY) -> (EndX, EndY) 
 * with the given color */
void VideoDrawLine(unsigned StartX, unsigned StartY, unsigned EndX, unsigned EndY, unsigned Color) {
	// Variables - clam some values
	int dx = abs(EndX - StartX), sx = StartX < EndX ? 1 : -1;
	int dy = abs(EndY - StartY), sy = StartY < EndY ? 1 : -1;
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
 * at the specified pixel-position */
OsStatus_t
VideoDrawCharacter(unsigned X, unsigned Y, int Character, uint32_t Bg, uint32_t Fg) {
	// Start out by determining the kind
	// of draw we want to do
	switch (VideoGetTerminal()->Type)
	{
		// Text-Mode
	case VIDEO_TEXT:
		return OsError;//TextDrawCharacter(Character, Y, X, LOBYTE(LOWORD(videoTerminal.FgColor)));

		// VBE
	case VIDEO_GRAPHICS:
		return VesaDrawCharacter(X, Y, Character, Fg, Bg);

		// No video?
	case VIDEO_NONE:
		return OsError;
	}

	// Uh?
	return OsError;
}

/* VideoDrawBootTerminal
 * Draws the Boot Terminal */
void VideoDrawBootTerminal(unsigned X, unsigned Y, size_t Width, size_t Height) {
	// Variables
	unsigned TitleStartX = X + 8 + 32 + 8;
	unsigned TitleStartY = Y + 18;
	int i;

	// Instantiate a pointer to title
	char *TitlePtr = (char*)GlbBootVideoWindowTitle;

	// Draw the header
	for (i = 0; i < 48; i++) {
		VideoDrawLine(X, Y + i, X + Width, Y + i, 0x2980B9);
	}
	
	// Draw remaining borders
	VideoDrawLine(X, Y, X, Y + Height, 0x2980B9);
	VideoDrawLine(X + Width, Y, X + Width, Y + Height, 0x2980B9);
	VideoDrawLine(X, Y + Height, X + Width, Y + Height, 0x2980B9);

	// Render title in middle of header
	while (*TitlePtr) {
		VideoDrawCharacter(TitleStartX, TitleStartY, *TitlePtr, 0x2980B9, 0xFFFFFF);
		TitleStartX += 10;
		TitlePtr++;
	}

	// Define some virtual borders to prettify just a little
	VideoGetTerminal()->CursorX = VideoGetTerminal()->CursorStartX = X + 11;
	VideoGetTerminal()->CursorLimitX = X + Width - 1;
	VideoGetTerminal()->CursorY = VideoGetTerminal()->CursorStartY = Y + 49;
	VideoGetTerminal()->CursorLimitY = Y + Height - 17;
}


// Initialize the vbe and draw the Boot Terminal
void VideoInitialize(Multiboot_t* BootInfo) {
	// Initialize the VBE
	VbeInitialize(BootInfo);

	// Draw the Boot Terminal
	VideoDrawBootTerminal((VideoGetTerminal()->CursorLimitX / 2) - 325, 0, 
			650, VideoGetTerminal()->Info.Height);
}
