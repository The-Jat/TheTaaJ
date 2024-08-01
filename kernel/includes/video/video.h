#ifndef __VIDEO_H__
#define __VIDEO_H__

OsStatus_t VideoDrawPixel(unsigned X, unsigned Y, uint32_t Color);
void VideoInitialize(Multiboot_t* BootInfo);

#endif /* __VIDEO_H__ */
