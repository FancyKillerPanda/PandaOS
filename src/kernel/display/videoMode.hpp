//  ===== Date Created: 22 November, 2020 ===== 

#if !defined(VIDEOMODE_HPP)
#define VIDEOMODE_HPP

#include "system.hpp"

constexpr u32 VIDEO_MEMORY = 0xe0000000;

struct PACKED_STRUCT VideoMode
{
	u16 screenWidth = 0;
	u16 screenHeight = 0;
	u8 bitsPerPixel = 0;
	u16 pitch = 0;
	u32 frameBufferPointer = 0;
};

// TODO(fkp): Is a global video mode really a good idea?
VideoMode videoInfo {};

void init_video(VideoMode* videoMode);

#endif
