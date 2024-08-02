#include <video/boot_terminal.h>
#include <video/video.h>
#include <video/vbe.h>

static Terminal_t s_bootTerminal;
static VbeMode_t* vbe;


char *g_bootVideoWindowTitle = "Welcome to the TaaJ OS.";
unsigned int BootTerminalColor = 0xF46F87;



/* VbeInitialize
 * Initializes the X86 video sub-system and provides
 * boot-video interface for the entire OS
 */
void Initialize_BootTerminal(Multiboot_t* BootInfo) {

	// Zero out structure
	memset(&s_bootTerminal, 0, sizeof(s_bootTerminal));

	// Initialize lock
	//SpinlockReset(&s_bootTerminal.Lock);

	// Which kind of mode has been enabled for us
	switch (BootInfo->VbeMode) {

		// Text-Mode (80x25)
		case 0: {
			s_bootTerminal.Type = VIDEO_TEXT;
			s_bootTerminal.Info.Width = 80;
			s_bootTerminal.Info.Height = 25;
			s_bootTerminal.Info.Depth = 16;
			s_bootTerminal.Info.BytesPerScanline = 2 * 80;
			s_bootTerminal.Info.FrameBufferAddress = STD_VIDEO_MEMORY;

			s_bootTerminal.CursorLimitX = 80;
			s_bootTerminal.CursorLimitY = 25;
			s_bootTerminal.FgColor = (0 << 4) | (15 & 0x0F);
			s_bootTerminal.BgColor = 0;
		} break;

		// Text-Mode (80x50)
		case 1: {
			s_bootTerminal.Type = VIDEO_TEXT;
			s_bootTerminal.Info.Width = 80;
			s_bootTerminal.Info.Height = 50;
			s_bootTerminal.Info.Depth = 16;
			s_bootTerminal.Info.BytesPerScanline = 2 * 80;
			s_bootTerminal.Info.FrameBufferAddress = STD_VIDEO_MEMORY;

			s_bootTerminal.CursorLimitX = 80;
			s_bootTerminal.CursorLimitY = 50;
			s_bootTerminal.FgColor = (0 << 4) | (15 & 0x0F);
			s_bootTerminal.BgColor = 0;
		} break;

		// VGA-Mode (Graphics)
		case 2:
		{
			s_bootTerminal.Type = VIDEO_GRAPHICS;
//uncomment later, these two lines
//			s_bootTerminal.CursorLimitX = s_bootTerminal.Info.Width / (FontWidth + 2);
//			s_bootTerminal.CursorLimitY = s_bootTerminal.Info.Height / FontHeight;
			s_bootTerminal.FgColor = (0 << 4) | (15 & 0x0F);
			s_bootTerminal.BgColor = 0;
		} break;

		// VBE-Mode (Graphics)
		default:
		{
			// Get active VBE information structure
			VbeMode_t *vbe = (VbeMode_t*)BootInfo->VbeModeInfo;

			// Copy information over
			s_bootTerminal.Type = VIDEO_GRAPHICS;
			s_bootTerminal.Info.FrameBufferAddress = vbe->PhysBasePtr;
			s_bootTerminal.Info.Width = vbe->XResolution;
			s_bootTerminal.Info.Height = vbe->YResolution;
			s_bootTerminal.Info.Depth = vbe->BitsPerPixel;
			s_bootTerminal.Info.BytesPerScanline = vbe->BytesPerScanLine;
			s_bootTerminal.Info.RedPosition = vbe->RedMaskPos;
			s_bootTerminal.Info.BluePosition = vbe->BlueMaskPos;
			s_bootTerminal.Info.GreenPosition = vbe->GreenMaskPos;
			s_bootTerminal.Info.ReservedPosition = vbe->ReservedMaskPos;
			s_bootTerminal.Info.RedMask = vbe->RedMaskSize;
			s_bootTerminal.Info.BlueMask = vbe->BlueMaskSize;
			s_bootTerminal.Info.GreenMask = vbe->GreenMaskSize;
			s_bootTerminal.Info.ReservedMask = vbe->ReservedMaskSize;
			
			set_vbe_mode((VbeMode_t*)BootInfo->VbeModeInfo);

			// Clear out background (to white)
			memset((void*)s_bootTerminal.Info.FrameBufferAddress, 0xFF,
				(s_bootTerminal.Info.BytesPerScanline * s_bootTerminal.Info.Height));

			s_bootTerminal.CursorLimitX = s_bootTerminal.Info.Width;
			s_bootTerminal.CursorLimitY = s_bootTerminal.Info.Height;
			s_bootTerminal.FgColor = 0;
			s_bootTerminal.BgColor = 0xFFFFFFFF;

		} break;
	}

	VideoSetTerminal(&s_bootTerminal);

	// Draw the Boot Terminal
	VideoDrawBootTerminal(0, 0, GetBootTerminal()->Info.Width, GetBootTerminal()->Info.Height);

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
	unsigned TitleStartX = (GetBootTerminal()->CursorLimitX / 2) - (length/2)*8;
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
	for(int i = 0; i < borderWidth; i++) {
		VideoDrawLine(X, Y + Height - i, X + Width, Y + Height - i, BootTerminalColor);
	}

	// Render title in middle of header
	while (*TitlePtr) {
		VideoDrawCharacter(TitleStartX, TitleStartY, *TitlePtr, BootTerminalColor, 0xFFFFFF);
		TitleStartX += 10;
		TitlePtr++;
	}

	// Define some virtual borders to prettify just a little
	GetBootTerminal()->CursorX = GetBootTerminal()->CursorStartX = X + 11;
	GetBootTerminal()->CursorLimitX = X + Width - 1;
	GetBootTerminal()->CursorY = GetBootTerminal()->CursorStartY = Y + 49;
	GetBootTerminal()->CursorLimitY = Y + Height - 17;

	// Print the very first line in terminal.
	BootTerminalPrintString("Hey, welcome to the BootTerminal...\n");
}


void BootTerminalPrintString(const char* str) {
	VideoDrawString(str);
}

/* GetBootTerminal
 * Retrieves the current terminal information */
Terminal_t* GetBootTerminal() {
	// Simply return the static pointer instance.
	return &s_bootTerminal;
}

