#include <stdint.h>
#include <stddef.h>
#include <video/vbe.h>
#include <datastructure.h>
#include <defs.h>

static VbeMode_t s_vbeMode;

/*
* static global variables = persists the value within this 
* file only. Unlike to global variables its scope is limited to this file only.
* It is not accessible from other files in the same program.
* In order to access it in other fies, we can have getter function()
* which returns the reference to this static global variable.
* 
* Note: Unlike global variables it would not be accessible even with by using extern keyword.
*/

/* set_vbe_mode
 * Sets the s_vbeMode from the boot_terminal
 */
void set_vbe_mode(const VbeMode_t* mode) {
	s_vbeMode = *mode;
}


/* VesaDrawPixel
 * Uses the vesa-interface to plot a single pixel
 */
OsStatus_t VesaDrawPixel(unsigned X, unsigned Y, uint32_t Color) {
	// Variables
	uint32_t *VideoPtr = NULL;

	// Calculate the video-offset
	VideoPtr = (uint32_t*)
		(s_vbeMode.PhysBasePtr 
			+ ((Y * s_vbeMode.BytesPerScanLine)
		+ (X * (s_vbeMode.BitsPerPixel / 8))));
	
	// Set the pixel
	(*VideoPtr) = (0xFF000000 | Color);

	// No error
	return Success;
}


/* VesaDrawCharacter
 * Renders a ASCII/UTF16 character at the given pixel-position
 * on the screen
 */
OsStatus_t VesaDrawCharacter(unsigned CursorX, unsigned CursorY, int Character, uint32_t FgColor, uint32_t BgColor) {
	// Variables
	uint32_t *vPtr = NULL;
	uint8_t *ChPtr = NULL;
	unsigned Row, i = (unsigned)Character;

	// Calculate the video-offset
	vPtr = (uint32_t*)(s_vbeMode.PhysBasePtr 
		+ ((CursorY * s_vbeMode.BytesPerScanLine)
		+ (CursorX * (s_vbeMode.BitsPerPixel / 8))));


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
		offset += s_vbeMode.BytesPerScanLine;
		vPtr = (uint32_t*)offset;
	}

	// Done - no errors
	return Success;
}

