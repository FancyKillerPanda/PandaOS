//  ===== Date Created: 14 September, 2020 ===== 

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "clargs.hpp"

constexpr const u8* options[6] = {
	"--help",
	"--image-name",
	"--image-path",
	"--vbr",
	"--size",
	"--files",
};
constexpr const u8* optionDescriptions[12] = {
	"--help", "Displays this help message.",
	"--image-name <name>", "Specifies the name of the output disk.",
	"--image-path <path>", "Specifies the location of the output disk.",
	"--vbr <file>", "Specifies a Volume Boot Record file.",
	"--size <number>", "Specifies the size of the hard disk in megabytes.",
	"--files [files...]", "Specifies a list of file names for the root directory.",
};
static_assert(STACK_ARRAY_LENGTH(optionDescriptions) % 2 == 0, "All options must have a description.");
static_assert(STACK_ARRAY_LENGTH(options) * 2 == STACK_ARRAY_LENGTH(optionDescriptions), "Size of options * 2 must equal size of descriptions.");

bool is_argument(const u8* string)
{
	for (s32 i = 0; i < STACK_ARRAY_LENGTH(options); i++)
	{
		if (strcmp(string, options[i]) == 0)
		{
			return true;
		}
	}

	return false;
}

bool handle_command_line_args(s32 argc, const u8* argv[], CLArgs* arguments)
{
	for (s32 i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "--help") == 0)
		{
			printf("Usage: genVDisk [options]\nOptions:\n");
			
			for (s32 i = 0; i < STACK_ARRAY_LENGTH(optionDescriptions); i += 2)
			{
				printf("\t[%-20s]: %s\n", optionDescriptions[i], optionDescriptions[i + 1]);
			}
			
			return false;
		}
		else if (strcmp(argv[i], "--image-name") == 0)
		{
			if (i + 1 < argc)
			{
				arguments->imageName = argv[i + 1];
				i += 1;
			}
			else
			{
				printf("Error: No disk name given.\n");
				return false;
			}
		}
		else if (strcmp(argv[i], "--image-path") == 0)
		{
			if (i + 1 < argc)
			{
				arguments->imagePath = argv[i + 1];
				i += 1;
			}
			else
			{
				printf("Error: No path to disk given.\n");
				return false;
			}
		}
		else if (strcmp(argv[i], "--vbr") == 0)
		{
			if (i + 1 < argc)
			{
				arguments->volumeBootRecordFile = argv[i + 1];
				i += 1;
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
				i += 1;
			}
			else
			{
				printf("Error: No size given.\n");
				return false;
			}
		}
		else if (strcmp(argv[i], "--files") == 0)
		{
			s32 indexOfFilesArgument = i;
			
			if (i + 1 < argc)
			{
				i += 1;

				while (i < argc)
				{
					if (is_argument(argv[i]))
					{
						if (i == indexOfFilesArgument + 1)
						{
							printf("Error: No additional files given.\n");
							return false;
						}
						else
						{
							break;
						}
					}

					i += 1;
				}
				
				i -= 1;
				arguments->numberOfOtherFiles = i - indexOfFilesArgument;
				arguments->otherFiles = (const u8**) malloc(arguments->numberOfOtherFiles * sizeof(const u8**));

				for (s32 j = 0; j < arguments->numberOfOtherFiles; j++)
				{
					arguments->otherFiles[j] = argv[j + indexOfFilesArgument + 1];
				}
			}
			else
			{
				printf("Error: No additional files given.\n");
				return false;
			}
		}
		else
		{
			if (i != 0)
			{
				printf("Warning: Ignoring argument '%s'.\n", argv[i]);
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
