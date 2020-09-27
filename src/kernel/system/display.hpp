//  ===== Date Created: 12 September, 2020 ===== 

#if !defined(DISPLAY_HPP)
#define DISPLAY_HPP

#include "system.hpp"

void enable_cursor();
void move_cursor(u8 row, u8 col);
void disable_cursor();

void clear_screen(u8 attribute = 0x07);
void scroll_screen_up(u8 numberOfLines = 0, u8 attribute = 0x07);

void print_char(u8 character, u8 attribute = 0x07);
void print_integer(u32 integer, u8 attribute = 0x07);
void print_hex_integer(u32 integer, u8 attribute = 0x07);
void print(const u8* string, u8 attribute = 0x07);

#endif
