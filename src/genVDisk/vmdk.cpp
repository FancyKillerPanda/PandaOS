//  ===== Date Created: 18 September, 2020 ===== 

#include <stdio.h>

#include "vmdk.hpp"
#include "geometry.hpp"
#include "fat16.hpp"

bool write_descriptor_file(const u8* descriptorFileName, const char* extentFileName, usize hardDiskSize)
{
	printf("Info: Writing VMDK descriptor to '%s'\n", descriptorFileName);
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
			"\n# Extent description\n"
			"RW %lu FLAT \"%s\" 0\n",
			diskGeometry.totalSectorsOnHardDisk, extentFileName);

	// Disk database
	fprintf(descriptorFile,
			"\n# The Disk Data Base\n"
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
	printf("Info: Successfully wrote descriptor file...\n");
	
	return true;
}

bool write_extent_file(const u8* extentFileName, const CLArgs& arguments)
{
	printf("\nInfo: Writing flat extent to '%s'\n", extentFileName);
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
	
	usize numberOfBlocksWritten = 0;
	u8* vbrFileContents = (u8*) malloc(vbrFileSize);
	fread(vbrFileContents, 1, vbrFileSize, vbrFile);

	numberOfBlocksWritten += write_data_as_blocks(extentFile, vbrFileContents, vbrFileSize);
	printf("Info: Wrote Volume Boot Record to extent file...\n");

	// Gets information about the FAT from the BPB/EBPB in the VBR
	FAT16Information fat16Information;
	read_bios_parameter_block(&fat16Information, vbrFileContents);
	free(vbrFileContents);
	fclose(vbrFile);

	// Writes out the FAT table
	FAT16 fat16 = init_fat_16(&fat16Information);

	for (usize i = 0; i < arguments.numberOfOtherFiles; i++)
	{
		store_file(&fat16, arguments.imagePath, arguments.otherFiles[i]);
	}
	
	numberOfBlocksWritten += write_fat16_into(&fat16, extentFile);
	write_data_as_blocks(extentFile, nullptr, 0, (arguments.hardDiskSize / 512) - numberOfBlocksWritten);
	
	fclose(extentFile);

	// DEBUG
	void debug_list_fat(FAT16* fat16);
	// debug_list_fat(&fat16);

	printf("Info: Successfully wrote extent file...\n");
	return true;
}
