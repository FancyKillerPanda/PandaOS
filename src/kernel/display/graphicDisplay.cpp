//  ===== Date Created: 08 July, 2021 ===== 

#include "display/graphicDisplay.hpp"
#include "display/videoMode.hpp"

void draw_rect(u32 x, u32 y, u32 width, u32 height, u32 colour)
{
	if (x >= videoInfo.screenWidth)
	{
		return;
	}

	if (y >= videoInfo.screenHeight)
	{
		return;
	}

	if (x + width > videoInfo.screenWidth)
	{
		width = videoInfo.screenWidth - x;
	}

	if (y + height > videoInfo.screenHeight)
	{
		height = videoInfo.screenHeight - y;
	}

	u8 red = (u8) ((colour & 0x00ff0000) >> 16);
	u8 green = (u8) ((colour & 0x0000ff00) >> 8);
	u8 blue = (u8) ((colour & 0x000000ff) >> 0);
	
	u8* location = (u8*) VIDEO_MEMORY + (y * videoInfo.pitch);
	u8 stride = videoInfo.bitsPerPixel / 8;
	
	for (u32 currentY = y; currentY < y + height; currentY++)
	{
		for (u32 currentX = x; currentX < x + width; currentX++)
		{
			location[(currentX * stride) + 0] = blue;
			location[(currentX * stride) + 1] = green;
			location[(currentX * stride) + 2] = red;
		}

		location += videoInfo.pitch;
	}
}

void clear_screen(u32 colour)
{
	draw_rect(0, 0, videoInfo.screenWidth, videoInfo.screenHeight, colour);
}

void draw_bitmap(u8* bitmap, u32 x, u32 y, u32 width, u32 height, u32 colour)
{
	for (u32 row = 0; row < height; row++)
	{
		for (u32 col = 0; col < width; col++)
		{
			u32 index = (row * width) + col;
			u8 byte = bitmap[index / 8];

			if (byte & (1 << (index % 8)))
			{
				draw_rect(x + col, y + row, 1, 1, colour);
			}
		}
	}
}
