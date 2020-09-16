//  ===== Date Created: 16 September, 2020 ===== 

#if !defined(FAT16_HPP)
#define FAT16_HPP

#include "utility.hpp"

struct FAT16Information
{
	u16 bytesPerSector = 0;
	u8 fatCount = 0;
	u16 rootDirectoryEntries = 0;
	u8 mediaDescriptor = 0;
	u16 sectorsPerFat = 0;
};

struct FAT16
{
	const FAT16Information* information = nullptr;
	
	usize tableSize = 0;
	u8* table = nullptr;
	
	usize rootDirectorySize = 0;
	u8* rootDirectoryData = nullptr;
	usize numberOfRootDirectoryEntries = 0;
	
	usize rawDataSize = 0;
	u8* rawData = nullptr;
};

void read_bios_parameter_block(FAT16Information* information, const u8* fileContents);
FAT16 init_fat_16(const FAT16Information* information);
bool store_file(FAT16* fat16, const char* prefix, const char* name);
void write_fat16_into(FAT16* fat16, FILE* file);
u16 read_word(const u8* data, usize indexOfFirstByte);

#endif
