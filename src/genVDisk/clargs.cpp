//  ===== Date Created: 14 September, 2020 ===== 

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "clargs.hpp"

constexpr const u8* optionDescriptions[8] = {
	"--help", "Displays this help message.",
	"--name <image name>", "Specifies the name of the output disk.",
	"--vbr <file>", "Specifies a Volume Boot Record file."
	"--size <number>", "Specifies the size of the hard disk in megabytes."
};
static_assert(STACK_ARRAY_LENGTH(optionDescriptions) % 2 == 0, "All options must have a description.");

bool handle_command_line_args(i32 argc, const u8* argv[], CLArgs* arguments)
{
	for (i32 i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "--help") == 0)
		{
			printf("Usage: genVDisk [options]\nOptions:\n");
			
			for (i32 i = 0; i < STACK_ARRAY_LENGTH(optionDescriptions); i += 2)
			{
				printf("\t[%s]: %s\n", optionDescriptions[i], optionDescriptions[i + 1]);
			}
			
			return false;
		}
		else if (strcmp(argv[i], "--name") == 0)
		{
			if (i + 1 < argc)
			{
				arguments->imageName = argv[i + 1];
			}
			else
			{
				printf("Error: No disk name given.\n");
				return false;
			}
		}
		else if (strcmp(argv[i], "--vbr") == 0)
		{
			if (i + 1 < argc)
			{
				arguments->volumeBootRecordFile = argv[i + 1];
			}
			else
			{
				printf("Error: No VBR file given.\n");
				return false;
			}
		}
		else if (strcmp(argv[i], "--size") == 0)
		{
			if (i + 1 < argc)
			{
				unsigned long size = strtoul(argv[i + 1], nullptr, 10);

				if (size == 0 || size == ULONG_MAX)
				{
					printf("Error: Size passed in was invalid.\n");
					return false;
				}
				
				arguments->hardDiskSize = (usize) MB(size);
			}
		}
	}

	// Makes sure required arguments were passed
	if (strcmp(arguments->imageName, "") == 0)
	{
		printf("Error: Output file must be supplied.\n");
		return false;
	}
	else if (strcmp(arguments->volumeBootRecordFile, "") == 0)
	{
		printf("Error: A volume boot record file must be supplied.\n");
		return false;
	}
	else if (arguments->hardDiskSize == 0)
	{
		printf("Error: A hard disk size must be supplied.\n");
		return false;
	}

	return true;
}
