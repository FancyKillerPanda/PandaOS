//  ===== Date Created: 14 September, 2020 ===== 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utility.hpp"
#include "clargs.hpp"
#include "geometry.hpp"
#include "fat16.hpp"

bool write_descriptor_file(const u8* descriptorFileName, usize hardDiskSize);
bool write_extent_file(const u8* extentFileName, const CLArgs& arguments);
bool write_data_as_blocks(FILE* file, const u8* data, usize size, usize minNumberOfBlocks = 0);
u16 read_word(const u8* data, usize indexOfFirstByte);

i32 main(i32 argc, const u8* argv[])
{
	CLArgs arguments;
	
	if (!handle_command_line_args(argc, argv, &arguments))
	{
		return 1;
	}

	// DEBUG
	for (usize i = 0; i < arguments.numberOfOtherFiles; i++)
	{
		printf("File: '%s'\n", arguments.otherFiles[i]);
	}

	// The descriptor file
	usize imagePathLength = strlen(arguments.imagePath);
	usize imageNameLength = strlen(arguments.imageName);
	u8* descriptorFileName = (u8*) calloc(imagePathLength + imageNameLength + strlen(".vmdk") + 1, sizeof(u8));
	memcpy(descriptorFileName, arguments.imagePath, imagePathLength);
	strcat(descriptorFileName, arguments.imageName);
	strcat(descriptorFileName, ".vmdk");
	
	write_descriptor_file(descriptorFileName, 64 * 1024 * 1024);
	free(descriptorFileName);
	
	// The extent file
	u8* extentFileName = (u8*) calloc(imagePathLength + imageNameLength + strlen("-flat.vmdk") + 1, sizeof(u8));
	memcpy(extentFileName, arguments.imagePath, imagePathLength);
	strcat(extentFileName, arguments.imageName);
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
	read_bios_parameter_block(&fat16Information, vbrFileContents);
	free(vbrFileContents);
	fclose(vbrFile);

	// Writes out the FAT table
	FAT16 fat16 = init_fat_16(&fat16Information);

	for (usize i = 0; i < arguments.numberOfOtherFiles; i++)
	{
		store_file(&fat16, arguments.otherFiles[i]);
	}
	
	for (u8 i = 0; i < fat16Information.fatCount; i++)
	{
		write_data_as_blocks(extentFile, fat16.table, fat16.tableSize, fat16Information.sectorsPerFat);
	}

	/*
	// The root directory
	constexpr usize sizeOfEntry = 32;
	u8* rootDirectoryData = (u8*) malloc(sizeOfEntry * arguments.numberOfOtherFiles * sizeof(u8));
	
	for (usize i = 0; i < arguments.numberOfOtherFiles; i++)
	{
	}
	*/

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
