//  ===== Date Created: 13 September, 2020 ===== 

#if !defined(MEMORY_HPP)
#define MEMORY_HPP

#include "system.hpp"

void set_memory(void* dest, usize count, u8 value);
void set_memory_16(void* dest, usize count, u16 value);
void copy_memory(const void* src, void* dest, usize count);

#endif
