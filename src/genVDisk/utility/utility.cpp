//  ===== Date Created: 18 September, 2020 ===== 

#include "utility/utility.hpp"

u8* concat_strings(const u8* strOne, const u8* strTwo)
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

bool read_entire_file(const u8* path, u8** data, usize* size)
{
	FILE* file = fopen(path, "rb");

	if (!file)
	{
		printf("Error: Failed to open file ('%s').\n", path);
		return false;
	}

	fseek(file, 0, SEEK_END);
	*size = ftell(file);
	fseek(file, 0, SEEK_SET);

	*data = (u8*) malloc(*size);
	fread(*data, 1, *size, file);

	fclose(file);
	return true;
}
