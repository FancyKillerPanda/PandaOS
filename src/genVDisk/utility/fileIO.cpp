//  ===== Date Created: 02 December, 2020 ===== 

#include <stdlib.h>
#include "fileIO.hpp"

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
