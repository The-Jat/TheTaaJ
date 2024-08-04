#ifndef __VIDEO_H__
#define __VIDEO_H__

#include <video/video_descriptor.h>

typedef struct Terminal {
	unsigned int		Type;
	VideoDescriptor_t	Info;

	// unsigned is shorthand for unsigned int
	unsigned		CursorX;
	unsigned		CursorY;
	
	unsigned		CursorStartX;
	unsigned		CursorStartY;
	unsigned		CursorLimitX;
	unsigned		CursorLimitY;

	uint32_t		FgColor;
	uint32_t		BgColor;
} __attribute__((packed)) Terminal_t;

void VideoSetTerminal(Terminal_t* terminal);
OsStatus_t VideoDrawPixel(unsigned X, unsigned Y, uint32_t Color);
Terminal_t* VideoGetTerminal();

OsStatus_t VesaPutCharacter(int Character);
OsStatus_t VGAPutCharacter(int Character);

void VideoDrawString(const char*);

OsStatus_t VideoScroll(int ByLines);


OsStatus_t TextDrawCharacter(int Character, unsigned CursorY, unsigned CursorX, uint8_t Color);

#endif /* __VIDEO_H__ */
