//  ===== Date Created: 22 November, 2020 ===== 

#if !defined(GRAPHICALDISPLAY_HPP)
#define GRAPHICALDISPLAY_HPP

#include "system.hpp"

// NOTE(fkp): Colour is xxRRGGBB
void draw_rect(u32 x, u32 y, u32 width, u32 height, u32 colour);

// Width is number of columns, height is number of rows
void draw_bitmap(u8* bitmap, u32 x, u32 y, u32 width, u32 height, u32 colour);

#endif
