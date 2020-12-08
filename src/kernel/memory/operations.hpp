//  ===== Date Created: 08 December, 2020 ===== 

#if !defined(OPERATIONS_HPP)
#define OPERATIONS_HPP

#include "system/common.hpp"

void memset(void* dest, u8 value, usize count);
void memset_16(void* dest, u16 value, usize count);
void memcpy(void* dest, const void* src, usize count);

#endif
