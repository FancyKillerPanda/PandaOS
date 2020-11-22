//  ===== Date Created: 22 November, 2020 ===== 

#if !defined(VIDEOMODE_HPP)
#define VIDEOMODE_HPP

#include "system.hpp"

struct PACKED_STRUCT VideoMode
{
	u16 screenWidth = 0;
	u16 screenHeight = 0;
	u8 bitsPerPixel = 0;
	u32 frameBufferPointer = 0;
};

#endif
