#include <stdint.h>
#include <stdlib.h>
#include <kernel_utilities.h>
#include <defs.h>
#include <video/vbe.h>
#include <datastructure.h>

char *g_bootVideoWindowTitle = "Welcome to the TaaJ OS.";
unsigned int BootTerminalColor = 0xF46F87;


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
		return Error;

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
		return Error;//TextDrawCharacter(Character, Y, X, LOBYTE(LOWORD(s_videoTerminal.FgColor)));

		// VBE
	case VIDEO_GRAPHICS:
		return VesaDrawCharacter(X, Y, Character, Fg, Bg);

		// No video?
	case VIDEO_NONE:
		return Error;
	}

	// Uh?
	return Error;
}


/* VideoDrawBootTerminal: Draws the Boot Terminal
 * x = x point of Boot Terminal
 * y = y point of Boot Terminal
 * Width = Width of the Boot Terminal
 * Height = Height of the Boot Terminal
 */
void VideoDrawBootTerminal(unsigned X, unsigned Y, size_t Width, size_t Height) {

	char *TitlePtr = (char*)g_bootVideoWindowTitle;
	int length = strlen(TitlePtr);

	// Title location.
	unsigned TitleStartX = (VideoGetTerminal()->CursorLimitX / 2) - (length/2)*8;
	unsigned TitleStartY = Y + 18;
	int i;

	// Draw the header
	for (i = 0; i < 48; i++) {
		VideoDrawLine(X, Y + i, X + Width, Y + i, BootTerminalColor);
	}

	// Draw remaining borders
	int borderWidth = 5;
	// Left border
	for(int i = 0; i < borderWidth; i++){
		VideoDrawLine(X + i, Y, X + i, Y + Height, BootTerminalColor);
	}
	// Right border
	for(int i = 0; i < borderWidth; i++) {
		VideoDrawLine(X + Width - i, Y, X + Width - i, Y + Height, BootTerminalColor);
	}
	// Bottom border
	VideoDrawLine(X, Y + Height, X + Width, Y + Height, BootTerminalColor);

	// Render title in middle of header
	while (*TitlePtr) {
		VideoDrawCharacter(TitleStartX, TitleStartY, *TitlePtr, BootTerminalColor, 0xFFFFFF);
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
	VideoDrawBootTerminal(0, 0, VideoGetTerminal()->Info.Width, VideoGetTerminal()->Info.Height);
}
