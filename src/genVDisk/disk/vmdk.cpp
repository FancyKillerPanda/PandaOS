//  ===== Date Created: 18 September, 2020 ===== 

#include <stdio.h>

#include "vmdk.hpp"
#include "utility/geometry.hpp"
// #include "filesystem/fat16.hpp"
#include "filesystem/fatFilesystem.hpp"
#include "filesystem/mbr.hpp"

bool write_descriptor_file(const u8* descriptorFileName, const char* extentFileName, usize hardDiskSize, const DiskGeometry* diskGeometry)
{
	printf("Info: Writing VMDK descriptor to '%s'\n", descriptorFileName);
	
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
			diskGeometry->totalSectorsOnHardDisk, extentFileName);

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
			diskGeometry->numberOfCylinders,
			diskGeometry->numberOfHeads,
			diskGeometry->numberOfSectors);

	fclose(descriptorFile);
	printf("Info: Successfully wrote descriptor file...\n");
	
	return true;
}

bool write_extent_file(const u8* extentFileName, const CLArgs& arguments, const DiskGeometry* diskGeometry)
{
	printf("\nInfo: Writing flat extent to '%s'\n", extentFileName);
	FILE* extentFile = fopen(extentFileName, "wb");

	if (!extentFile)
	{
		printf("Error: Failed to open extent file (%s).\n", extentFileName);
		return false;
	}
	
	usize numberOfBlocksWritten = 0;
	
	// Writes the MBR file to the extent file
	MBR masterBootRecord;
	if (!init_mbr(&masterBootRecord, arguments.masterBootRecordFile))
	{
		fclose(extentFile);
		return false;
	}

	if (!add_partition_to_mbr(&masterBootRecord, diskGeometry, arguments.hardDiskSize / 512))
	{
		printf("Error: Failed to add a partition to the MBR.\n");
		fclose(extentFile);
		return false;
	}

	numberOfBlocksWritten += write_data_as_blocks(extentFile, masterBootRecord.data, masterBootRecord.size);
	free(masterBootRecord.data);
	printf("Info: Wrote Master Boot Record to extent file.\n");
	
	// Writes the VBR file to the extent file
	u8* vbrFileContents = nullptr;
	usize vbrFileSize = 0;

	if (!read_entire_file(arguments.volumeBootRecordFile, &vbrFileContents, &vbrFileSize))
	{
		fclose(extentFile);
		return false;
	}

	if (vbrFileSize != 512 ||
		vbrFileContents[510] != 0x55 || vbrFileContents[511] != 0xaa)
	{
		printf("Error: VBR file ('%s') is invalid.\n", arguments.volumeBootRecordFile);
		fclose(extentFile);
		return false;
	}

	numberOfBlocksWritten += write_data_as_blocks(extentFile, vbrFileContents, vbrFileSize);
	printf("Info: Wrote Volume Boot Record to extent file...\n");
	
	if (!arguments.isFat32)
	{
		/*
		// Gets information about the FAT from the BPB/EBPB in the VBR
		FAT16Information fat16Information;
		read_bios_parameter_block(&fat16Information, vbrFileContents);
		free(vbrFileContents);

		// Writes out the FAT table
		FAT16 fat16 = init_fat_16(&fat16Information);

		for (usize i = 0; i < arguments.numberOfOtherFiles; i++)
		{
			store_file(&fat16, arguments.imagePath, arguments.otherFiles[i]);
		}
	
		numberOfBlocksWritten += write_fat16_into(&fat16, extentFile);
		write_data_as_blocks(extentFile, nullptr, 0, (arguments.hardDiskSize / 512) - numberOfBlocksWritten);
		*/

		FATFilesystem fat = init_fat(FilesystemType::Fat16, vbrFileContents);
		free(vbrFileContents);

		for (usize i = 0; i < arguments.numberOfOtherFiles; i++)
		{
			store_file(&fat, arguments.imagePath, arguments.otherFiles[i]);
		}

		numberOfBlocksWritten += write_fat_into(&fat, extentFile);
		write_data_as_blocks(extentFile, nullptr, 0, (arguments.hardDiskSize / 512) - numberOfBlocksWritten);
	}
	
	fclose(extentFile);

	// DEBUG
	// void debug_list_fat(FAT16* fat16);
	// debug_list_fat(&fat16);

	printf("Info: Successfully wrote extent file...\n");
	return true;
}
