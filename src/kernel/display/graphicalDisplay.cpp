//  ===== Date Created: 22 November, 2020 ===== 

#include "graphicalDisplay.hpp"

void draw_pixel(u32 x, u32 y, u32 colour)
{
	u32 rowOffset = y * videoInfo.pitch;
	u32 colOffset = (x * videoInfo.bitsPerPixel) / 8;
	u8* location = (u8*) (VIDEO_MEMORY + rowOffset + colOffset);

	location[0] = (u8) ((colour & 0x000000ff) >> 0);
	location[1] = (u8) ((colour & 0x0000ff00) >> 8);
	location[2] = (u8) ((colour & 0x00ff0000) >> 16);
}

void draw_rect(u32 x, u32 y, u32 width, u32 height, u32 colour)
{
	// TODO(fkp): This can be very easily optimised by not calling
	// draw_pixel() for each one. This is just a proof of concept!
	// TODO(fkp): Ensure that the rect is within the bounds of the
	// screen, or only draw the area that is.
	for (u32 currentY = y; currentY < y + height; currentY++)
	{
		for (u32 currentX = x; currentX < x + width; currentX++)
		{
			draw_pixel(currentX, currentY, colour);
		}
	}
}
