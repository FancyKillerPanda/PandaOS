//  ===== Date Created: 04 October, 2020 ===== 

#if !defined(FATFILESYSTEM_HPP)
#define FATFILESYSTEM_HPP

#include "utility/utility.hpp"

enum class FilesystemType
{
	Fat16,
	Fat32,
};

struct FATInformation
{
	u16 bytesPerSector = 0;
	u8 fatCount = 0;
	u16 rootDirectoryEntries = 0;
	u8 mediaDescriptor = 0;
	u16 sectorsPerFat = 0;
};

struct FATFilesystem
{
	FilesystemType type;
	FATInformation information;

	usize tableSize;
	u8* table = nullptr;

	usize rootDirectorySize = 0;
	u8* rootDirectoryData = nullptr;
	usize numberOfRootDirectoryEntries = 0;

	usize rawDataSize = 0;
	u8* rawData = nullptr;
};

FATFilesystem init_fat(FilesystemType type, const u8* vbrFileContents);
bool store_file(FATFilesystem* fat, const char* prefix, const char* name);
usize write_fat_into(FATFilesystem* fat, FILE* file);
u16 read_word(const u8* data, usize indexOfFirstByte);

#endif
