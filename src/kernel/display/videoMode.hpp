//  ===== Date Created: 07 July, 2021 ===== 

#if !defined(VIDEOMODE_HPP)
#define VIDEOMODE_HPP

#include "system/common.hpp"

struct PACKED_STRUCT VideoMode
{
	u16 screenWidth = 0;
	u16 screenHeight = 0;
	u8 bitsPerPixel = 0;
	u16 pitch = 0;
	u32 framebufferPointer = 0;
};

constexpr u32 VIDEO_MEMORY = 0xe0000000;
VideoMode videoInfo {};

void init_video(VideoMode* videoMode);

#endif
