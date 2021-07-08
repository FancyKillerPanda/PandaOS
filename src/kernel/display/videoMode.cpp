//  ===== Date Created: 08 July, 2021 ===== 

#include "display/videoMode.hpp"
#include "memory/virtualAllocator.hpp"

void init_video(VideoMode* videoMode)
{
	videoInfo = *videoMode;

	u32 framebufferSize = videoInfo.pitch * videoInfo.screenHeight;
	allocate_virtual_range((void*) VIDEO_MEMORY, framebufferSize,
						   (void*) videoInfo.framebufferPointer);

	log_info("Video Mode: %d*%dpx (pitch: %d bytes), %d bits per pixel.",
			 videoInfo.screenWidth, videoInfo.screenHeight,
			 videoInfo.pitch, videoInfo.bitsPerPixel);
	log_info("Allocated pages from physcial %x (linear %x) for video framebuffer.",
			 (u32) videoInfo.framebufferPointer, VIDEO_MEMORY);

	ASSERT(videoInfo.bitsPerPixel == 24 || videoInfo.bitsPerPixel == 32,
		   "Unsupported number of bits per pixel.");
}
