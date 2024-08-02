#include <stdint.h>
#include <stdlib.h>
#include <kernel_utilities.h>
#include <defs.h>
#include <video/video.h>
#include <video/vbe.h>
#include <datastructure.h>

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
 * with the given color
 */
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
 * at the specified pixel-position
 */
OsStatus_t VideoDrawCharacter(unsigned X, unsigned Y, int Character, uint32_t Bg, uint32_t Fg) {
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

	return Error;
}


/* VideoPutCharacter
 * Uses the vesa-interface to print a new character
 * at the current terminal position
 */
OsStatus_t VideoPutCharacter(int Character) {

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
		VideoPutCharacter(*str);
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


Terminal_t*
VideoGetTerminal()
{
	// Simply return the static pointer instance.
	return s_videoTerminal;
}
