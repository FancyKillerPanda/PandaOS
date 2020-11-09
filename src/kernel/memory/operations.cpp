//  ===== Date Created: 13 September, 2020 ===== 

#include "memory/operations.hpp"

void memset(void* dest, u8 value, usize count)
{
	u8* dest_u8 = (u8*) dest;
	
	while (count-- > 0)
	{
		*dest_u8 = value;
		dest_u8 += 1;
	}
}

void memset_16(void* dest, u16 value, usize count)
{
	u16* dest_u16 = (u16*) dest;
	
	while (count-- > 0)
	{
		*dest_u16 = value;
		dest_u16 += 1;
	}
}

void memcpy(void* dest, const void* src, usize count)
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
