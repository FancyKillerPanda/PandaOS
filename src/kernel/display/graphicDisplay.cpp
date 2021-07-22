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
	draw_bitmap_extended(bitmap, x, y, width, height, &colour, 1);
}

void draw_bitmap_extended(u8* bitmap, u32 x, u32 y, u32 width, u32 height, u32* colours, u8 bitsPerPixel)
{
	ASSERT(bitsPerPixel != 0, "Bitmap cannot have 0 bits per pixel.");
	ASSERT(bitsPerPixel <= 8, "Bitmap does not support more than 8 bits per pixel.");
	
	for (u32 row = 0; row < height; row++)
	{
		for (u32 col = 0; col < width; col++)
		{
			u32 index = (row * (width * bitsPerPixel)) + (col * bitsPerPixel);
			u8 byte = bitmap[index / 8];

			// For example, 3 bits per pixel creates mask 0b111
			u8 mask = (1 << bitsPerPixel) - 1;
			
			// We need to shift further for earlier bits, since
			// bitmaps are defined left to right.
			u8 shiftAmount = 7 - (index % 8);
			u8 maskedByte = byte & (mask << shiftAmount);

			// Colour index needs to be 0-based
			u8 colourIndex = (maskedByte >> shiftAmount) - 1;

			if (maskedByte)
			{
				draw_rect(x + col, y + row, 1, 1, colours[colourIndex]);
			}
		}
	}
}

