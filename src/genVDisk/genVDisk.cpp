//  ===== Date Created: 14 September, 2020 ===== 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utility.hpp"
#include "clargs.hpp"
#include "vmdk.hpp"

s32 main(s32 argc, const u8* argv[])
{
	CLArgs arguments;
	
	if (!handle_command_line_args(argc, argv, &arguments))
	{
		return 1;
	}

	DiskGeometry diskGeometry;
	
	if (!calculate_geometry(&diskGeometry, arguments.hardDiskSize))
	{
		return 1;
	}
	
	// The descriptor file
	u8* imagePathWithoutEnd = concat_strings(arguments.imagePath, arguments.imageName);
	u8* extentFileName = concat_strings(imagePathWithoutEnd, "-flat.vmdk");
	u8* descriptorFileName = concat_strings(imagePathWithoutEnd, ".vmdk");	

	if (!write_descriptor_file(descriptorFileName, extentFileName, arguments.hardDiskSize, &diskGeometry))
	{
		free(descriptorFileName);
		free(extentFileName);
		free(imagePathWithoutEnd);
		
		return 1;
	}
	
	free(descriptorFileName);
	
	// The extent file
	if (!write_extent_file(extentFileName, arguments, &diskGeometry))
	{
		free(extentFileName);
		free(imagePathWithoutEnd);

		return 1;
	}
	
	free(extentFileName);
	free(imagePathWithoutEnd);

	return 0;
}
