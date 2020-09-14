//  ===== Date Created: 14 September, 2020 ===== 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utility.hpp"
#include "clargs.hpp"

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
	
	u8* extentFileName = (u8*) malloc(imageNameLength + strlen("-flat.vmdk") + 1);
	memcpy(extentFileName, arguments.imageName, imageNameLength);
	strcat(extentFileName, "-flat.vmdk");

	printf("Outputting to %s and %s.\n", descriptorFileName, extentFileName);

	free(extentFileName);
	free(descriptorFileName);
}
