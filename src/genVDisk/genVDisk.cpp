//  ===== Date Created: 14 September, 2020 ===== 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utility.hpp"
#include "clargs.hpp"
#include "geometry.hpp"

bool write_descriptor_file(const u8* descriptorFileName, usize hardDiskSize);
bool write_extent_file(const u8* extentFileName, const CLArgs& arguments);
bool write_data_as_blocks(FILE* file, const u8* data, usize size, usize minNumberOfBlocks = 0);
u16 read_word(const u8* data, usize indexOfFirstByte);

struct FAT16Information
{
	u16 bytesPerSector = 0;
	u8 fatCount = 0;
	u8 mediaDescriptor = 0;
	u16 sectorsPerFat = 0;
};

i32 main(i32 argc, const u8* argv[])
{
	CLArgs arguments;
	
	if (!handle_command_line_args(argc, argv, &arguments))
	{
		return 1;
	}

	// The descriptor file
	usize imageNameLength = strlen(arguments.imageName);
	u8* descriptorFileName = (u8*) calloc(imageNameLength + strlen(".vmdk") + 1, sizeof(u8));
	memcpy(descriptorFileName, arguments.imageName, imageNameLength);
	strcat(descriptorFileName, ".vmdk");
	
	write_descriptor_file(descriptorFileName, 64 * 1024 * 1024);
	free(descriptorFileName);
	
	// The extent file
	u8* extentFileName = (u8*) calloc(imageNameLength + strlen("-flat.vmdk") + 1, sizeof(u8));
	memcpy(extentFileName, arguments.imageName, imageNameLength);
	strcat(extentFileName, "-flat.vmdk");

	write_extent_file(extentFileName, arguments);
	free(extentFileName);
}

bool write_descriptor_file(const u8* descriptorFileName, usize hardDiskSize)
{
	DiskGeometry diskGeometry;
	
	if (!calculate_geometry(&diskGeometry, hardDiskSize))
	{
		return false;
	}
	
	FILE* descriptorFile = fopen(descriptorFileName, "wb");

	if (!descriptorFile)
	{
		printf("Error: Failed to open descriptor file (%s).\n", descriptorFileName);
		return false;
	}

	// Header
	fputs("# Disk DescriptorFile\n"
		  "version=1\n"
		  "encoding=\"UTF-8\"\n"
		  "CID=12345678\n"
		  "parentCID=ffffffff\n"
		  "createType=\"monolithicFlat\"\n",
		  descriptorFile);

	// Extent description
	fprintf(descriptorFile,
			"# Extent description\n"
			"RW %lu FLAT \"%s\" 0\n",
			diskGeometry.totalSectorsOnHardDisk, descriptorFileName);

	// Disk database
	fprintf(descriptorFile,
			"# The Disk Data Base\n"
			"#DDB\n"
			"ddb.virtualHWVersion = \"16\"\n"
			"ddb.geometry.cylinders = \"%lu\"\n"
			"ddb.geometry.heads = \"%lu\"\n"
			"ddb.geometry.sectors = \"%lu\"\n"
			"ddb.adapterType = \"ide\"\n"
			"ddb.toolsVersion = \"0\"\n",
			diskGeometry.numberOfCylinders,
			diskGeometry.numberOfHeads,
			diskGeometry.numberOfSectors);

	fclose(descriptorFile);
	return true;
}

bool write_extent_file(const u8* extentFileName, const CLArgs& arguments)
{
	FILE* extentFile = fopen(extentFileName, "wb");

	if (!extentFile)
	{
		printf("Error: Failed to open extent file (%s).\n", extentFileName);
		return false;
	}

	FILE* vbrFile = fopen(arguments.volumeBootRecordFile, "rb");

	if (!vbrFile)
	{
		printf("Error: VBR file (%s) is invalid.\n", arguments.volumeBootRecordFile);
		fclose(extentFile);
		return false;
	}
	
	// Reads the entire contents of the VBR file and writes it to the extent file
	fseek(vbrFile, 0, SEEK_END);
	usize vbrFileSize = ftell(vbrFile);
	fseek(vbrFile, 0, SEEK_SET);

	if (vbrFileSize != 512)
	{
		printf("VBR file (%s) is not 512 bytes long.\n", arguments.volumeBootRecordFile);
		fclose(vbrFile);
		fclose(extentFile);
		
		return false;
	}
	
	u8* vbrFileContents = (u8*) malloc(vbrFileSize);
	fread(vbrFileContents, 1, vbrFileSize, vbrFile);
	write_data_as_blocks(extentFile, vbrFileContents, vbrFileSize);

	// Gets information about the FAT from the BPB/EBPB in the VBR
	FAT16Information fat16Information;
	fat16Information.bytesPerSector = read_word(vbrFileContents, 0x0b);
	fat16Information.fatCount = vbrFileContents[0x10];
	fat16Information.mediaDescriptor = vbrFileContents[0x15];
	fat16Information.sectorsPerFat = read_word(vbrFileContents, 0x16);
	free(vbrFileContents);
	fclose(vbrFile);

	// Writes out the FAT table
	for (u8 i = 0; i < fat16Information.fatCount; i++)
	{
		write_data_as_blocks(extentFile, data, 4, fat16Information.sectorsPerFat);
	}

	fclose(extentFile);
	return true;
}

bool write_data_as_blocks(FILE* file, const u8* data, usize size, usize minNumberOfBlocks)
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
	
	return true;
}

u16 read_word(const u8* data, usize indexOfFirstByte)
{
	u16 firstByte = (u16) data[indexOfFirstByte];
	u16 secondByte = (u16) data[indexOfFirstByte + 1];
	u16 result = (secondByte << 8) | firstByte;

	return result;
}
