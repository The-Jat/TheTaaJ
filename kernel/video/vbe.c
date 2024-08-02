#include <stdint.h>
#include <stddef.h>
#include <video/vbe.h>
#include <datastructure.h>
#include <defs.h>

// static global variable, that's why prefix s_
/*
* static global variables = persists the value within this 
* file only. Similar to global variables its scope is limited to this file only.
* It is not accessible from other files in the same program.
* In order to access it in other fies, we have VideoGetTerminal()
* which returns the reference to this static global variable.
* 
* Note: Unlike global variables it would not be accessible even with by using extern keyword.
*/
static BootTerminal_t s_videoTerminal;

// Font stuff defined in font8x16.c file.
extern const uint8_t FontBitmaps[];
extern const uint32_t FontNumChars;
extern const uint32_t FontHeight;
extern const uint32_t FontWidth;


/* VesaDrawPixel
 * Uses the vesa-interface to plot a single pixel */
OsStatus_t VesaDrawPixel(unsigned X, unsigned Y, uint32_t Color) {
	// Variables
	uint32_t *VideoPtr = NULL;

	// Calculate the video-offset
	VideoPtr = (uint32_t*)
		(s_videoTerminal.Info.FrameBufferAddress 
			+ ((Y * s_videoTerminal.Info.BytesPerScanline)
		+ (X * (s_videoTerminal.Info.Depth / 8))));

	// Set the pixel
	(*VideoPtr) = (0xFF000000 | Color);

	// No error
	return Success;
}


/* VesaDrawCharacter
 * Renders a ASCII/UTF16 character at the given pixel-position
 * on the screen */
OsStatus_t VesaDrawCharacter(unsigned CursorX, unsigned CursorY, int Character, uint32_t FgColor, uint32_t BgColor) {
	// Variables
	uint32_t *vPtr = NULL;
	uint8_t *ChPtr = NULL;
	unsigned Row, i = (unsigned)Character;

	// Calculate the video-offset
	vPtr = (uint32_t*)(s_videoTerminal.Info.FrameBufferAddress 
		+ ((CursorY * s_videoTerminal.Info.BytesPerScanline)
		+ (CursorX * (s_videoTerminal.Info.Depth / 8))));


	// Lookup bitmap
	ChPtr = (uint8_t*)&FontBitmaps[i * FontHeight];

	// Iterate bitmap rows
	for (Row = 0; Row < FontHeight; Row++) {
		uint8_t BmpData = ChPtr[Row];
		uint32_t offset;

		// Render data in row
		for (i = 0; i < 8; i++) {
			vPtr[i] = (BmpData >> (7 - i)) & 0x1 ? (0xFF000000 | FgColor) : (0xFF000000 | BgColor);
		}

		// Increase the memory pointer by row
		offset = (uint32_t)vPtr;
		offset += s_videoTerminal.Info.BytesPerScanline;
		vPtr = (uint32_t*)offset;
	}

	// Done - no errors
	return Success;
}


/* TextDrawCharacter
 * Renders an ASCII character at the given text-position
 * on the screen by the given color combination */
OsStatus_t TextDrawCharacter(int Character, unsigned CursorY, unsigned CursorX, uint8_t Color) {
	// Variables
	uint16_t *Video = NULL;
	uint16_t Data = ((uint16_t)Color << 8) | (uint8_t)(Character & 0xFF);

	// Calculate video position
	Video = (uint16_t*)s_videoTerminal.Info.FrameBufferAddress +
		(CursorY * s_videoTerminal.Info.Width + CursorX);

	// Plot it on the screen
	*Video = Data;

	// Done - no errors
	return Success;
}

/* VesaScroll
 * Scrolls the terminal <n> lines up by using the
 * vesa-interface */
OsStatus_t 
VesaScroll(int ByLines) {
	// Variables
	uint8_t *VideoPtr = NULL;
	size_t BytesToCopy = 0;
	int Lines = 0;
	int i = 0, j = 0;

	// How many lines do we need to modify?
	Lines = (s_videoTerminal.CursorLimitY - s_videoTerminal.CursorStartY);

	// Calculate the initial screen position
	VideoPtr = (uint8_t*)(s_videoTerminal.Info.FrameBufferAddress +
		((s_videoTerminal.CursorStartY * s_videoTerminal.Info.BytesPerScanline)
			+ (s_videoTerminal.CursorStartX * (s_videoTerminal.Info.Depth / 8))));

	// Calculate num of bytes
	BytesToCopy = ((s_videoTerminal.CursorLimitX - s_videoTerminal.CursorStartX)
		* (s_videoTerminal.Info.Depth / 8));

	// Do the actual scroll
	for (i = 0; i < ByLines; i++) {
		for (j = 0; j < Lines; j++) {
			memcpy(VideoPtr, VideoPtr +
				(s_videoTerminal.Info.BytesPerScanline * FontHeight), BytesToCopy);
			VideoPtr += s_videoTerminal.Info.BytesPerScanline;
		}
	}

	// Clear out the lines that was scrolled
	VideoPtr = (uint8_t*)(s_videoTerminal.Info.FrameBufferAddress +
		((s_videoTerminal.CursorStartX * (s_videoTerminal.Info.Depth / 8))));

	// Scroll pointer down to bottom - n lines
	VideoPtr += (s_videoTerminal.Info.BytesPerScanline 
		* (s_videoTerminal.CursorLimitY - (FontHeight * ByLines)));

	// Clear out lines
	for (i = 0; i < ((int)FontHeight * ByLines); i++) {
		memset(VideoPtr, 0xFF, BytesToCopy);
		VideoPtr += s_videoTerminal.Info.BytesPerScanline;
	}

	// We did the scroll, modify cursor
	s_videoTerminal.CursorY -= (FontHeight * ByLines);

	// No errors
	return Success;
}


/* VesaPutCharacter
 * Uses the vesa-interface to print a new character
 * at the current terminal position */
OsStatus_t VesaPutCharacter(int Character) {
	// Acquire terminal lock
	//SpinlockAcquire(&s_videoTerminal.Lock);

	// The first step is to handle special
	// case characters that we shouldn't print out
	switch (Character) 
	{
		// New-Line
		// Reset X and increase Y
	case '\n': {
		s_videoTerminal.CursorX = s_videoTerminal.CursorStartX;
		s_videoTerminal.CursorY += FontHeight;
	} break;

	// Carriage Return
	// Reset X don't increase Y
	case '\r': {
		s_videoTerminal.CursorX = s_videoTerminal.CursorStartX;
	} break;

	// Default
	// Printable character
	default: {
		// Call print with the current location
		// and use the current colors
		VesaDrawCharacter(s_videoTerminal.CursorX, s_videoTerminal.CursorY,
			Character, s_videoTerminal.FgColor, s_videoTerminal.BgColor);
		s_videoTerminal.CursorX += (FontWidth + 2);
	} break;
	}

	// Next step is to do some post-print
	// checks, including new-line and scroll-checks

	// Are we at last X position? - New-line
	if ((s_videoTerminal.CursorX + (FontWidth + 2)) >= s_videoTerminal.CursorLimitX) {
		s_videoTerminal.CursorX = s_videoTerminal.CursorStartX;
		s_videoTerminal.CursorY += FontHeight;
	}

	// Do we need to scroll the terminal?
	if ((s_videoTerminal.CursorY + FontHeight) >= s_videoTerminal.CursorLimitY) {
		VesaScroll(1);
	}

	// Release lock and return OK
	//SpinlockRelease(&s_videoTerminal.Lock);
	return Success;
}


/* VbeInitialize
 * Initializes the X86 video sub-system and provides
 * boot-video interface for the entire OS */
void VbeInitialize(Multiboot_t *BootInfo) {
	// Zero out structure
	memset(&s_videoTerminal, 0, sizeof(s_videoTerminal));

	// Initialize lock
	//SpinlockReset(&s_videoTerminal.Lock);

	// Which kind of mode has been enabled for us
	switch (BootInfo->VbeMode) {

		// Text-Mode (80x25)
		case 0: {
			s_videoTerminal.Type = VIDEO_TEXT;
			s_videoTerminal.Info.Width = 80;
			s_videoTerminal.Info.Height = 25;
			s_videoTerminal.Info.Depth = 16;
			s_videoTerminal.Info.BytesPerScanline = 2 * 80;
			s_videoTerminal.Info.FrameBufferAddress = STD_VIDEO_MEMORY;

			s_videoTerminal.CursorLimitX = 80;
			s_videoTerminal.CursorLimitY = 25;
			s_videoTerminal.FgColor = (0 << 4) | (15 & 0x0F);
			s_videoTerminal.BgColor = 0;
		} break;

		// Text-Mode (80x50)
		case 1: {
			s_videoTerminal.Type = VIDEO_TEXT;
			s_videoTerminal.Info.Width = 80;
			s_videoTerminal.Info.Height = 50;
			s_videoTerminal.Info.Depth = 16;
			s_videoTerminal.Info.BytesPerScanline = 2 * 80;
			s_videoTerminal.Info.FrameBufferAddress = STD_VIDEO_MEMORY;

			s_videoTerminal.CursorLimitX = 80;
			s_videoTerminal.CursorLimitY = 50;
			s_videoTerminal.FgColor = (0 << 4) | (15 & 0x0F);
			s_videoTerminal.BgColor = 0;
		} break;

		// VGA-Mode (Graphics)
		case 2:
		{
			s_videoTerminal.Type = VIDEO_GRAPHICS;

			s_videoTerminal.CursorLimitX = s_videoTerminal.Info.Width / (FontWidth + 2);
			s_videoTerminal.CursorLimitY = s_videoTerminal.Info.Height / FontHeight;
			s_videoTerminal.FgColor = (0 << 4) | (15 & 0x0F);
			s_videoTerminal.BgColor = 0;
		} break;

		// VBE-Mode (Graphics)
		default:
		{
			// Get active VBE information structure
			VbeMode_t *vbe = (VbeMode_t*)BootInfo->VbeModeInfo;

			// Copy information over
			s_videoTerminal.Type = VIDEO_GRAPHICS;
			s_videoTerminal.Info.FrameBufferAddress = vbe->PhysBasePtr;
			s_videoTerminal.Info.Width = vbe->XResolution;
			s_videoTerminal.Info.Height = vbe->YResolution;
			s_videoTerminal.Info.Depth = vbe->BitsPerPixel;
			s_videoTerminal.Info.BytesPerScanline = vbe->BytesPerScanLine;
			s_videoTerminal.Info.RedPosition = vbe->RedMaskPos;
			s_videoTerminal.Info.BluePosition = vbe->BlueMaskPos;
			s_videoTerminal.Info.GreenPosition = vbe->GreenMaskPos;
			s_videoTerminal.Info.ReservedPosition = vbe->ReservedMaskPos;
			s_videoTerminal.Info.RedMask = vbe->RedMaskSize;
			s_videoTerminal.Info.BlueMask = vbe->BlueMaskSize;
			s_videoTerminal.Info.GreenMask = vbe->GreenMaskSize;
			s_videoTerminal.Info.ReservedMask = vbe->ReservedMaskSize;

			// Clear out background (to white)
			memset((void*)s_videoTerminal.Info.FrameBufferAddress, 0xFF,
				(s_videoTerminal.Info.BytesPerScanline * s_videoTerminal.Info.Height));

			s_videoTerminal.CursorLimitX = s_videoTerminal.Info.Width;
			s_videoTerminal.CursorLimitY = s_videoTerminal.Info.Height;
			s_videoTerminal.FgColor = 0;
			s_videoTerminal.BgColor = 0xFFFFFFFF;

		} break;
	}

}


/* VideoGetTerminal
 * Retrieves the current terminal information */
BootTerminal_t*
VideoGetTerminal()
{
	// Simply return the static
	return &s_videoTerminal;
}
