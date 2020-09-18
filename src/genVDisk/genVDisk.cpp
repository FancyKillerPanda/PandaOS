//  ===== Date Created: 14 September, 2020 ===== 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utility.hpp"
#include "clargs.hpp"
#include "vmdk.hpp"

i32 main(i32 argc, const u8* argv[])
{
	CLArgs arguments;
	
	if (!handle_command_line_args(argc, argv, &arguments))
	{
		return 1;
	}

	// The descriptor file
	u8* imagePathWithoutEnd = concat_strings(arguments.imagePath, arguments.imageName);
	u8* descriptorFileName = concat_strings(imagePathWithoutEnd, ".vmdk");	
	u8* extentFileName = concat_strings(imagePathWithoutEnd, "-flat.vmdk");
	write_descriptor_file(descriptorFileName, extentFileName, arguments.hardDiskSize);
	free(descriptorFileName);
	
	// The extent file
	write_extent_file(extentFileName, arguments);
	free(extentFileName);
	free(imagePathWithoutEnd);
}
