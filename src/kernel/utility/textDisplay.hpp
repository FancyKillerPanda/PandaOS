//  ===== Date Created: 08 December, 2020 ===== 

#if !defined(TEXTDISPLAY_HPP)
#define TEXTDISPLAY_HPP

#include "system/common.hpp"

void move_cursor(u8 row, u8 col);
void scroll_screen_up(u8 numberOfLines = 0, u8 attribute = 0x07);

void print_char(u8 character, u8 attribute = 0x07);

void print(const u8* string, u8 attribute = 0x07);
void vprintf(const u8* string, va_list argsPointer);
void printf(const u8* string, ...);

#endif
