//  ===== Date Created: 12 September, 2020 ===== 

#if !defined(DISPLAY_HPP)
#define DISPLAY_HPP

#include "system.hpp"

// TODO(fkp): Implement these properly
inline void move_cursor() {}
inline void clear_screen() {}
inline void scroll_screen() {}

void print_char(u8 character, u8 attribute = 0x0f);
void print(const u8* string, u8 attribute = 0x0f);

#endif
