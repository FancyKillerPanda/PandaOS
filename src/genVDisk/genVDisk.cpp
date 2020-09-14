//  ===== Date Created: 14 September, 2020 ===== 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utility.hpp"
#include "clargs.hpp"
#include "geometry.hpp"

bool write_descriptor_file(u8* descriptorFileName, usize hardDiskSize);

i32 main(i32 argc, const u8* argv[])
{
	CLArgs arguments;
	
	if (!handle_command_line_args(argc, argv, &arguments))
	{
		return 1;
	}

	// Generates the file names
	usize imageNameLength = strlen(arguments.imageName);
	u8* descriptorFileName = (u8*) malloc(imageNameLength + strlen(".vmdk") + 1);
	memcpy(descriptorFileName, arguments.imageName, imageNameLength);
	strcat(descriptorFileName, ".vmdk");
	
	write_descriptor_file(descriptorFileName, 64 * 1024 * 1024);
	free(descriptorFileName);
	
	u8* extentFileName = (u8*) malloc(imageNameLength + strlen("-flat.vmdk") + 1);
	memcpy(extentFileName, arguments.imageName, imageNameLength);
	strcat(extentFileName, "-flat.vmdk");

	free(extentFileName);
}

bool write_descriptor_file(u8* descriptorFileName, usize hardDiskSize)
{
	DiskGeometry diskGeometry;
	
	if (!calculate_geometry(&diskGeometry, hardDiskSize))
	{
		return false;
	}
	
	FILE* descriptorFile = fopen(descriptorFileName, "w");

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
