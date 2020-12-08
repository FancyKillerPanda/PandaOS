//  ===== Date Created: 08 December, 2020 ===== 

#if !defined(TEXTDISPLAY_HPP)
#define TEXTDISPLAY_HPP

#include "system/common.hpp"

void move_cursor(u8 row, u8 col);

void print_char(u8 character, u8 attribute = 0x07);
void print(const u8* string, u8 attribute = 0x07);

#endif
