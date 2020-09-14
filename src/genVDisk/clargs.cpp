//  ===== Date Created: 14 September, 2020 ===== 

#include <string.h>
#include <stdio.h>

#include "clargs.hpp"

constexpr const u8* optionDescriptions[6] = {
	"--help", "Displays this help message.",
	"--output <file>", "Specifies an output file.",
	"--vbr <file>", "Specifies a Volume Boot Record file."
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
		else if (strcmp(argv[i], "--output") == 0)
		{
			if (i + 1 < argc)
			{
				arguments->imageName = argv[i + 1];
			}
			else
			{
				printf("Error: No output file given.\n");
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

	return true;
}
