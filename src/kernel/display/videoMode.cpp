//  ===== Date Created: 22 November, 2020 ===== 

#include "videoMode.hpp"
#include "memory/virtualAllocator.hpp"

void init_video(VideoMode* videoMode)
{
	videoInfo = *videoMode;
	u32 frameBufferSize = videoInfo.pitch * videoInfo.screenHeight;
	
	// TODO(fkp): A very similar calculation is used in quite a few places.
	// Maybe abstract it out into a utility function.
	u32 numberOfPages = ((frameBufferSize + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1)) / PAGE_SIZE;

	for (u32 i = 0; i < numberOfPages; i++)
	{
		map_page_address((void*) (VIDEO_MEMORY + (i * PAGE_SIZE)), (void*) (videoInfo.frameBufferPointer + (i * PAGE_SIZE)));
	}

	log_info("Video Mode: %dx%d px (pitch: %d), %d bits per pixel.",
			 videoInfo.screenWidth, videoInfo.screenHeight,
			 videoInfo.pitch, videoInfo.bitsPerPixel);
	log_info("Allocated %d pages from %x for video framebuffer.",
			 numberOfPages, videoInfo.frameBufferPointer);

	ASSERT(videoInfo.bitsPerPixel == 24 || videoInfo.bitsPerPixel == 32,
		   "Unsupported number of bits per pixel.");
}
