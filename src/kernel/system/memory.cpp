//  ===== Date Created: 13 September, 2020 ===== 

#include "memory.hpp"

void set_memory(void* dest, usize count, u8 value)
{
	u8* dest_u8 = (u8*) dest;
	
	while (count-- > 0)
	{
		*dest_u8 = value;
		dest_u8 += 1;
	}
}

void set_memory_16(void* dest, usize count, u16 value)
{
	u16* dest_u16 = (u16*) dest;
	
	while (count-- > 0)
	{
		*dest_u16 = value;
		dest_u16 += 1;
	}
}

void copy_memory(const void* src, void* dest, usize count)
{
	const u8* src_u8 = (u8*) src;
	u8* dest_u8 = (u8*) dest;
	
	while (count-- > 0)
	{
		*dest_u8 = *src_u8;
		dest_u8 += 1;
		src_u8 += 1;
	}
}
