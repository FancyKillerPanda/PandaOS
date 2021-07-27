//  ===== Date Created: 27 July, 2021 ===== 

#include <stdlib.h>
#include "utility/common.hpp"

u8* concat_strings(const u8* strOne, const u8* strTwo)
{
	usize strOneLength = strlen(strOne);
	u8* result = (u8*) calloc(strOneLength + strlen(strTwo) + 1, sizeof(u8));
	memcpy(result, strOne, strOneLength);
	strcat(result, strTwo);

	return result;
}
