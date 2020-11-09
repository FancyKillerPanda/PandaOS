//  ===== Date Created: 13 September, 2020 ===== 

#if !defined(MEMORY_HPP)
#define MEMORY_HPP

#include "system.hpp"

void memset(void* dest, u8 value, usize count);
void memset_16(void* dest, u16 value, usize count);
void memcpy(void* dest, const void* src, usize count);

#endif
