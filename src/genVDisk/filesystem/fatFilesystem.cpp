//  ===== Date Created: 04 October, 2020 =====

#include "fatFilesystem.hpp"

struct RootDirectoryEntry
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
static_assert(sizeof(RootDirectoryEntry) == 32,
              "RootDirectoryEntry might have had padding added.");

FATFilesystem init_fat(FilesystemType type, const u8* vbrFileContents)
{
	FATInformation information;
	information.bytesPerSector = read_word(vbrFileContents, 0x0b);
	information.fatCount = vbrFileContents[0x10];
	information.rootDirectoryEntries = read_word(vbrFileContents, 0x11);
	information.mediaDescriptor = vbrFileContents[0x15];
	information.sectorsPerFat = read_word(vbrFileContents, 0x16);

	FATFilesystem result;
	result.type = type;
	result.information = information;
	result.tableSize = information.bytesPerSector * information.sectorsPerFat;
	result.table = (u8*) calloc(result.tableSize, sizeof(u8));
	result.rootDirectorySize = information.rootDirectoryEntries * sizeof(RootDirectoryEntry);
	result.rootDirectoryData = (u8*) calloc(result.rootDirectorySize, sizeof(u8));

	if (result.type == FilesystemType::Fat16)
	{
		result.table[0] = information.mediaDescriptor;
		result.table[1] = 0xff;
		result.table[2] = 0xff;
	}

	return result;
}

void put_entry(FATFilesystem* fat, u32 index, u32 value)
{
	if (index <= 1)
	{
		printf("Error: Cannot put entry into indices zero or one.\n");
		return;
	}

	if (fat->type == FilesystemType::Fat16)
	{
		bool isLowerHalf = (index % 2) == 0;
		index = (index / 2) * 3;

		if (isLowerHalf)
		{
			fat->table[index] = (u16) value & 0x00ff;
			fat->table[index + 1] = (fat->table[index + 1] & 0xf0) | (((u16) value & 0x0f00) >> 8);
		}
		else
		{
			fat->table[index + 1] = (fat->table[index + 1] & 0x0f) | (((u16) value & 0x0f) << 4);
			fat->table[index + 2] = ((u16) value & 0x0ff0) >> 4;
		}
	}
}

u32 get_entry(FATFilesystem* fat, u32 index)
{
	if (index <= 1)
	{
		printf("Error: Cannot get entry from indices zero or one (reserved).\n");
		return 0;
	}

	u32 entry = 0;
	
	if (fat->type == FilesystemType::Fat16)
	{
		bool isLowerHalf = (index % 2) == 0;
		index = (index / 2) * 3;

		if (isLowerHalf)
		{
			entry = fat->table[index];
			entry = entry | ((fat->table[index + 1] & 0x0f) << 8);
		}
		else
		{
			entry = fat->table[index + 1] >> 4;
			entry = entry | (fat->table[index + 2] << 4);
		}
	}
	
	return entry;
}

u32 find_next_free_cluster(FATFilesystem* fat, u32 after = 2)
{
	if (after < 2)
	{
		after = 2;
	}

	constexpr u32 numberOfClusters = 3072;

	for (u32 i = after; i < numberOfClusters; i++)
	{
		if (get_entry(fat, i) == 0)
		{
			return i;
		}
	}

	return 0;
}

bool has_space_for(FATFilesystem* fat, u32 numberOfClusters)
{
	u32 lastFree = 0;

	for (u16 i = 0; i < numberOfClusters; i++)
	{
		lastFree = find_next_free_cluster(fat, lastFree);

		if (lastFree == 0)
		{
			return false;
		}
	}

	return true;
}

u32 allocate_clusters(FATFilesystem* fat, u32 numberOfRequiredClusters)
{
	u32 firstCluster = 0;
	u32 previousCluster = 0;

	if (!has_space_for(fat, numberOfRequiredClusters))
	{
		return 0;
	}

	for (u32 i = 0; i < numberOfRequiredClusters; i++)
	{
		u32 cluster = find_next_free_cluster(fat);
		
		if (i == 0)
		{
			firstCluster = cluster;
		}

		if (previousCluster != 0)
		{
			put_entry(fat, previousCluster, cluster);
		}

		previousCluster = cluster;
		put_entry(fat, cluster, 0x01);
	}

	if (fat->type == FilesystemType::Fat16)
	{
		put_entry(fat, previousCluster, 0x0ff8);
	}

	return firstCluster;
}

void store_file_in_root_directory(FATFilesystem* fat, FILE* file, const char* filename, u32 size, u32 cluster)
{
	// Calculates the 8.3 filename/extension pair
	RootDirectoryEntry entry;
	usize fullFilenameLength = strlen(filename);
	s32 extensionStart = -1;

	for (usize j = 0; j < 11; j++)
	{
		if (j == fullFilenameLength)
		{
			break;
		}
		else if (filename[j] == '.')
		{
			extensionStart = (s32) j + 1;
			break;
		}

		if (j >= 8)
		{
			continue;
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
	entry.size = size;

	if (fat->type == FilesystemType::Fat16)
	{
		entry.cluster = cluster;
	}
	
	memcpy(fat->rootDirectoryData + (fat->numberOfRootDirectoryEntries * sizeof(RootDirectoryEntry)), (u8*) &entry, sizeof(entry));
	fat->numberOfRootDirectoryEntries += 1;
}

bool store_file(FATFilesystem* fat, const char* prefix, const char* name)
{
	u8* filename = concat_strings(prefix, name);
	FILE* file = fopen(filename, "rb");

	if (!file)
	{
		printf("Error: Failed to store file '%s'.\n", filename);
		free(filename);
		return false;
	}

	fseek(file, 0, SEEK_END);
	usize fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	u8* fileContents = (u8*) malloc(fileSize);
	fread(fileContents, 1, fileSize, file);

	// Stores the FAT entry
	u32 numberOfRequiredClusters = 1 + ((fileSize - 1) / fat->information.bytesPerSector);
	u32 firstCluster = allocate_clusters(fat, numberOfRequiredClusters);

	if (firstCluster == 0)
	{
		printf("Error: Not enough space to allocate for '%s'.\n", filename);
		fclose(file);
		free(filename);
		
		return false;
	}

	// Inserts the file's data
	usize newRawDataSize = fat->rawDataSize + (numberOfRequiredClusters * fat->information.bytesPerSector);
	u8* newRawData = (u8*) malloc(newRawDataSize);

	if (fat->rawDataSize != 0)
	{
		memcpy(newRawData, fat->rawData, fat->rawDataSize);
		free(fat->rawData);
	}
	
	memcpy(newRawData + fat->rawDataSize, fileContents, fileSize);
	fat->rawDataSize = newRawDataSize;
	fat->rawData = newRawData;

	store_file_in_root_directory(fat, file, name, fileSize, firstCluster);
	fclose(file);
	free(filename);
	
	return true;
}
	
usize write_fat_into(FATFilesystem* fat, FILE* file)
{
	usize numberOfBlocksWritten = 0;

	for (u8 i = 0; i < fat->information.fatCount; i++)
	{
		numberOfBlocksWritten += write_data_as_blocks(file, fat->table, fat->tableSize, fat->information.sectorsPerFat);
	}

	printf("Info: Wrote FAT...\n");
	numberOfBlocksWritten += write_data_as_blocks(file, fat->rootDirectoryData, fat->rootDirectorySize, 0);
	printf("Info: Wrote root directory...\n");
	numberOfBlocksWritten += write_data_as_blocks(file, fat->rawData, fat->rawDataSize, 0);
	printf("Info: Wrote raw file data...\n");

	return numberOfBlocksWritten;
}
