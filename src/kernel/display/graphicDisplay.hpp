//  ===== Date Created: 08 July, 2021 ===== 

#if !defined(GRAPHICDISPLAY_HPP)
#define GRAPHICDISPLAY_HPP

#include "system/common.hpp"

// NOTE(fkp): Colour is xxRRGGBB
void draw_rect(u32 x, u32 y, u32 width, u32 height, u32 colour);
void clear_screen(u32 colour);

// NOTE(fkp): Width is number of columns, height is number of rows
void draw_bitmap(u8* bitmap, u32 x, u32 y, u32 width, u32 height, u32 colour);
void draw_bitmap_extended(u8* bitmap, u32 x, u32 y, u32 width, u32 height, u32* colours, u8 bitsPerPixel);

#endif
