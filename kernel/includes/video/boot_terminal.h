#ifndef __BOOT_TERMINAL_H__
#define __BOOT_TERMINAL_H__

#include <stdint.h>
#include <stddef.h>
#include <datastructure.h>
#include <defs.h>
#include <video/video_descriptor.h>
#include <video/video.h>


void Initialize_BootTerminal(Multiboot_t* BootInfo);
Terminal_t* GetBootTerminal();
void VideoDrawBootTerminal(unsigned X, unsigned Y, size_t Width, size_t Height);

#endif
