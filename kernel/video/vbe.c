#include <stdint.h>
#include <stddef.h>
#include <video/vbe.h>
#include <datastructure.h>
#include <defs.h>

static BootTerminal_t videoTerminal;

// Font stuff
extern const uint8_t MCoreFontBitmaps[];
extern const uint32_t MCoreFontNumChars;
extern const uint32_t MCoreFontHeight;
extern const uint32_t MCoreFontWidth;


/* VesaDrawPixel
 * Uses the vesa-interface to plot a single pixel */
OsStatus_t VesaDrawPixel(unsigned X, unsigned Y, uint32_t Color) {
	// Variables
	uint32_t *VideoPtr = NULL;

	// Calculate the video-offset
	VideoPtr = (uint32_t*)
		(videoTerminal.Info.FrameBufferAddress 
			+ ((Y * videoTerminal.Info.BytesPerScanline)
		+ (X * (videoTerminal.Info.Depth / 8))));

	// Set the pixel
	(*VideoPtr) = (0xFF000000 | Color);

	// No error
	return OsSuccess;
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
	vPtr = (uint32_t*)(videoTerminal.Info.FrameBufferAddress 
		+ ((CursorY * videoTerminal.Info.BytesPerScanline)
		+ (CursorX * (videoTerminal.Info.Depth / 8))));


	// Lookup bitmap
	ChPtr = (uint8_t*)&MCoreFontBitmaps[i * MCoreFontHeight];

	// Iterate bitmap rows
	for (Row = 0; Row < MCoreFontHeight; Row++) {
		uint8_t BmpData = ChPtr[Row];
		uint32_t _;

		// Render data in row
		for (i = 0; i < 8; i++) {
			vPtr[i] = (BmpData >> (7 - i)) & 0x1 ? (0xFF000000 | FgColor) : (0xFF000000 | BgColor);
		}

		// Increase the memory pointer by row
		_ = (uint32_t)vPtr;
		_ += videoTerminal.Info.BytesPerScanline;
		vPtr = (uint32_t*)_;
	}

	// Done - no errors
	return OsSuccess;
}


/* TextDrawCharacter
 * Renders an ASCII character at the given text-position
 * on the screen by the given color combination */
OsStatus_t TextDrawCharacter(int Character, unsigned CursorY, unsigned CursorX, uint8_t Color) {
	// Variables
	uint16_t *Video = NULL;
	uint16_t Data = ((uint16_t)Color << 8) | (uint8_t)(Character & 0xFF);

	// Calculate video position
	Video = (uint16_t*)videoTerminal.Info.FrameBufferAddress +
		(CursorY * videoTerminal.Info.Width + CursorX);

	// Plot it on the screen
	*Video = Data;

	// Done - no errors
	return OsSuccess;
}


/* VbeInitialize
 * Initializes the X86 video sub-system and provides
 * boot-video interface for the entire OS */
void VbeInitialize(Multiboot_t *BootInfo) {
	// Zero out structure
	memset(&videoTerminal, 0, sizeof(videoTerminal));

	// Initialize lock
	//SpinlockReset(&videoTerminal.Lock);

	// Which kind of mode has been enabled for us
	switch (BootInfo->VbeMode) {

		// Text-Mode (80x25)
		case 0: {
			videoTerminal.Type = VIDEO_TEXT;
			videoTerminal.Info.Width = 80;
			videoTerminal.Info.Height = 25;
			videoTerminal.Info.Depth = 16;
			videoTerminal.Info.BytesPerScanline = 2 * 80;
			videoTerminal.Info.FrameBufferAddress = STD_VIDEO_MEMORY;

			videoTerminal.CursorLimitX = 80;
			videoTerminal.CursorLimitY = 25;
			videoTerminal.FgColor = (0 << 4) | (15 & 0x0F);
			videoTerminal.BgColor = 0;
		} break;

		// Text-Mode (80x50)
		case 1: {
			videoTerminal.Type = VIDEO_TEXT;
			videoTerminal.Info.Width = 80;
			videoTerminal.Info.Height = 50;
			videoTerminal.Info.Depth = 16;
			videoTerminal.Info.BytesPerScanline = 2 * 80;
			videoTerminal.Info.FrameBufferAddress = STD_VIDEO_MEMORY;

			videoTerminal.CursorLimitX = 80;
			videoTerminal.CursorLimitY = 50;
			videoTerminal.FgColor = (0 << 4) | (15 & 0x0F);
			videoTerminal.BgColor = 0;
		} break;

		// VGA-Mode (Graphics)
		case 2:
		{
			videoTerminal.Type = VIDEO_GRAPHICS;

			videoTerminal.CursorLimitX = videoTerminal.Info.Width / (MCoreFontWidth + 2);
			videoTerminal.CursorLimitY = videoTerminal.Info.Height / MCoreFontHeight;
			videoTerminal.FgColor = (0 << 4) | (15 & 0x0F);
			videoTerminal.BgColor = 0;
		} break;

		// VBE-Mode (Graphics)
		default:
		{
			// Get active VBE information structure
			VbeMode_t *vbe = (VbeMode_t*)BootInfo->VbeModeInfo;

			// Copy information over
			videoTerminal.Type = VIDEO_GRAPHICS;
			videoTerminal.Info.FrameBufferAddress = vbe->PhysBasePtr;
			videoTerminal.Info.Width = vbe->XResolution;
			videoTerminal.Info.Height = vbe->YResolution;
			videoTerminal.Info.Depth = vbe->BitsPerPixel;
			videoTerminal.Info.BytesPerScanline = vbe->BytesPerScanLine;
			videoTerminal.Info.RedPosition = vbe->RedMaskPos;
			videoTerminal.Info.BluePosition = vbe->BlueMaskPos;
			videoTerminal.Info.GreenPosition = vbe->GreenMaskPos;
			videoTerminal.Info.ReservedPosition = vbe->ReservedMaskPos;
			videoTerminal.Info.RedMask = vbe->RedMaskSize;
			videoTerminal.Info.BlueMask = vbe->BlueMaskSize;
			videoTerminal.Info.GreenMask = vbe->GreenMaskSize;
			videoTerminal.Info.ReservedMask = vbe->ReservedMaskSize;

			// Clear out background (to white)
			memset((void*)videoTerminal.Info.FrameBufferAddress, 0xFF,
				(videoTerminal.Info.BytesPerScanline * videoTerminal.Info.Height));

			videoTerminal.CursorLimitX = videoTerminal.Info.Width;
			videoTerminal.CursorLimitY = videoTerminal.Info.Height;
			videoTerminal.FgColor = 0;
			videoTerminal.BgColor = 0xFFFFFFFF;

		} break;
	}

	// Initialize OS video
	// VideoInitialize();
}


/* VideoGetTerminal
 * Retrieves the current terminal information */
BootTerminal_t*
VideoGetTerminal()
{
	// Simply return the static
	return &videoTerminal;
}
