//  ===== Date Created: 16 September, 2020 ===== 

#include <cstdlib>
#include <stdio.h>
#include <string.h>

#include "fat16.hpp"

void read_bios_parameter_block(FAT16Information* information, const u8* fileContents)
{
	information->bytesPerSector = read_word(fileContents, 0x0b);
	information->fatCount = fileContents[0x10];
	information->mediaDescriptor = fileContents[0x15];
	information->sectorsPerFat = read_word(fileContents, 0x16);
}

FAT16 init_fat_16(const FAT16Information* information)
{
	FAT16 result;

	result.information = information;
	result.tableSize = information->bytesPerSector * information->sectorsPerFat;
	result.table = (u8*) calloc(result.tableSize, sizeof(u8));
	result.table[0] = 0xf0;
	result.table[1] = 0xff;
	result.table[2] = 0xff;

	return result;
}

void put_entry(FAT16* fat16, u16 index, u16 value)
{
	if (index <= 1)
	{
		printf("Error: Cannot put entry into indices zero or one.\n");
		return;
	}

	bool isLowerHalf = (index % 2) == 0;
	index = (index / 2) * 3;

	if (isLowerHalf)
	{
		fat16->table[index] = value & 0x00ff;
		fat16->table[index + 1] = (fat16->table[index + 1] & 0xf0) | ((value & 0x0f00) >> 8);
	}
	else
	{
		fat16->table[index + 1] = (fat16->table[index + 1] & 0x0f) | ((value & 0x0f) << 4);
		fat16->table[index + 2] = (value & 0x0ff0) >> 4;
	}
}

u16 get_entry(FAT16* fat16, u16 index)
{
	if (index <= 1)
	{
		printf("Error: Cannot get entry from indices zero or one (reserved).\n");
		return 0;
	}

	bool isLowerHalf = (index % 2) == 0;
	index = (index / 2) * 3;
	u16 entry = 0;

	if (isLowerHalf)
	{
		entry = fat16->table[index];
		entry = entry | ((fat16->table[index + 1] & 0x0f) << 8);
	}
	else
	{
		entry = fat16->table[index + 1] >> 4;
		entry = entry | (fat16->table[index + 2] << 4);
	}

	return entry;
}

u16 find_next_free_cluster(FAT16* fat16, u16 after = 2)
{
	if (after < 2)
	{
		after = 2;
	}

	constexpr u16 numberOfClusters = 3072;
	
	for (u16 i = after; i < numberOfClusters; i++)
	{
		if (get_entry(fat16, i) == 0)
		{
			return i;
		}
	}

	return 0;
}

bool has_space_for(FAT16* fat16, u16 numberOfClusters)
{
	u16 lastFree = 0;

	for (u16 i = 0; i < numberOfClusters; i++)
	{
		lastFree = find_next_free_cluster(fat16, lastFree);

		if (lastFree == 0)
		{
			return false;
		}
	}

	return true;
}

u16 allocate_clusters(FAT16* fat16, u16 numberOfRequiredClusters)
{
	u16 firstCluster = 0;
	u16 previousCluster = 0;

	if (!has_space_for(fat16, numberOfRequiredClusters))
	{
		return 0;
	}
	
	for (u16 i = 0; i < numberOfRequiredClusters; i++)
	{
		u16 cluster = find_next_free_cluster(fat16);

		if (i == 0)
		{
			firstCluster = cluster;
		}

		if (previousCluster != 0)
		{
			put_entry(fat16, previousCluster, cluster);
		}

		previousCluster = cluster;
		put_entry(fat16, cluster, 0x01);
	}

	put_entry(fat16, previousCluster, 0x0ff8);
	return firstCluster;
}

void store_file_in_root_directory(FAT16* fat16, FILE* file, const char* filename, u32 size, u16 cluster)
{
	struct Entry
	{
		u8 filename[8] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
		u8 extension[3] = { ' ', ' ', ' ' };
		u8 attributes = 0;
		u8 reserved[10];
		u8 time[2];
		u8 date[2];
		u16 cluster;
		u32 size;
	};
	
	// Calculates the 8.3 filename/extension pair
	Entry entry;
	usize fullFilenameLength = strlen(filename);
	i32 extensionStart = -1;
		
	for (usize j = 0; j < 8; j++)
	{
		if (j == fullFilenameLength)
		{
			break;
		}
		else if (filename[j] == '.')
		{
			extensionStart = (i32) j;
			break;
		}

		entry.filename[j] = filename[j];
	}

	if (extensionStart != -1)
	{
		for (usize j = (usize) extensionStart; j < (usize) extensionStart + 3; j++)
		{
			if (j == fullFilenameLength)
			{
				break;
			}

			entry.extension[j - (usize) extensionStart] = filename[j];
		}
	}

	// Rest of the entry
	entry.attributes = 0x04; // System file
	// TODO(fkp): Time and date
	entry.cluster = cluster;
	entry.size = size;

	static_assert(sizeof(Entry) == 32, "Entry might have had padding added.");
	fwrite((u8*) &entry, 1, sizeof(Entry), file);
}

bool store_file(FAT16* fat16, const char* path)
{
	FILE* file = fopen(path, "rb");

	if (!file)
	{
		printf("Error: Failed to store file '%s'.\n", path);
		return false;
	}

	fseek(file, 0, SEEK_END);
	usize fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	u8* fileContents = (u8*) malloc(fileSize);
	fread(fileContents, 1, fileSize, file);

	// Inserts the file's data
	usize newRawDataSize = fat16->rawDataSize + fileSize;
	u8* newRawData = (u8*) malloc(fat16->rawDataSize);

	if (fat16->rawDataSize != 0)
	{
		memcpy(newRawData, fat16->rawData, fat16->rawDataSize);
		free(fat16->rawData);
	}
	
	fat16->rawDataSize = newRawDataSize;
	fat16->rawData = newRawData;
	
	// Stores the FAT entry
	u16 numberOfRequiredClusters = 1 + ((fileSize - 1) / fat16->information->bytesPerSector);
	u16 firstCluster = allocate_clusters(fat16, numberOfRequiredClusters);

	if (firstCluster == 0)
	{
		printf("Error: Not enough space to allocate for '%s'.\n", path);
		fclose(file);
		
		return false;
	}

	store_file_in_root_directory(fat16, file, path, fileSize, firstCluster);
	fclose(file);
	
	return true;
}

u16 read_word(const u8* data, usize indexOfFirstByte)
{
	u16 firstByte = (u16) data[indexOfFirstByte];
	u16 secondByte = (u16) data[indexOfFirstByte + 1];
	u16 result = (secondByte << 8) | firstByte;

	return result;
}
