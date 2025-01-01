#ifndef __VIDEO_DESCRIPTOR_H__
#define __VIDEO_DESCRIPTOR_H__

#include <stdint.h>
#include <stddef.h>


typedef struct VideoDescriptor {
	uintptr_t		FrameBufferAddress;
	
	size_t			BytesPerScanline;
	size_t			Height;
	size_t			Width;
	int			Depth;

	int			RedPosition;
	int			BluePosition;
	int			GreenPosition;
	int			ReservedPosition;

	int			RedMask;
	int			BlueMask;
	int			GreenMask;
	int			ReservedMask;
} __attribute__((packed)) VideoDescriptor_t;

#endif
