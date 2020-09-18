//  ===== Date Created: 18 September, 2020 ===== 

#include "utility.hpp"

u8* concat_strings(const char* strOne, const char* strTwo)
{
	usize strOneLength = strlen(strOne);
	u8* result = (u8*) calloc(strOneLength + strlen(strTwo) + 1, sizeof(u8));
	memcpy(result, strOne, strOneLength);
	strcat(result, strTwo);

	return result;
}

usize write_data_as_blocks(FILE* file, const u8* data, usize size, usize minNumberOfBlocks)
{
	constexpr usize blockSize = 512;
	usize amountOfPadding;

	if (blockSize * minNumberOfBlocks <= size)
	{
		amountOfPadding = (blockSize - (size % blockSize)) % blockSize;
	}
	else
	{
		amountOfPadding = (blockSize * minNumberOfBlocks) - size;
	}
	
	u8* padding = (u8*) calloc(amountOfPadding, sizeof(u8));
	
	fwrite(data, 1, size, file);
	fwrite(padding, 1, amountOfPadding, file);
	free(padding);

	return (size + amountOfPadding) / blockSize;
}

u16 read_word(const u8* data, usize indexOfFirstByte)
{
	u16 firstByte = (u16) data[indexOfFirstByte];
	u16 secondByte = (u16) data[indexOfFirstByte + 1];
	u16 result = (secondByte << 8) | firstByte;

	return result;
}
