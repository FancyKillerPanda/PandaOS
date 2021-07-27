//  ===== Date Created: 02 December, 2020 ===== 

#include <string.h>
#include "clArgs.hpp"

constexpr const u8* options[] = {
	"--help",
	"--output",

	"--floppy",
	"--hdd",
	
	"--mbr",
	"--bootloader",
	"--kernel",
};
constexpr const u8* optionDescriptions[] = {
	"--help", "Displays this help message.",
	"--output <path>", "The name of the output file.",
	
	"--floppy", "Sets the disk type to be \"Floppy Disk\".",
	"--hdd", "Sets thes disk type to be \"Hard Disk Drive\".",
	
	"--mbr <path>", "Specifies the MBR file.",
	"--bootloader <path>", "Specifies the bootloader file.",
	"--kernel <path>", "Specifies the kernel file.",
};
static_assert(STACK_ARRAY_LENGTH(optionDescriptions) % 2 == 0,
			  "All options must have a description.");
static_assert(STACK_ARRAY_LENGTH(options) * 2 == STACK_ARRAY_LENGTH(optionDescriptions),
			  "Size of options * 2 must equal size of descriptions.");

bool is_argument(const u8* string)
{
	for (u32 i = 0; i < STACK_ARRAY_LENGTH(options); i++)
	{
		if (strcmp(string, options[i]) == 0)
		{
			return true;
		}
	}

	return false;
}

bool handle_command_line_args(s32 argc, const u8* argv[], CLArgs& arguments)
{
	for (s32 i = 0; i < argc; i++)
	{
		if (strcmp(argv[i], "--help") == 0)
		{
			printf("Usage: genVDisk [options]\nOptions:\n");
			
			for (u32 i = 0; i < STACK_ARRAY_LENGTH(optionDescriptions); i += 2)
			{
				printf("\t[%-20s]: %s\n", optionDescriptions[i], optionDescriptions[i + 1]);
			}
			
			return false;
		}
		else if (strcmp(argv[i], "--output") == 0)
		{
			if (i + 1 < argc)
			{
				arguments.outputName = argv[i + 1];
				i += 1;
			}
			else
			{
				printf("Error: No output name given.\n");
				return false;
			}
		}
		else if (strcmp(argv[i], "--floppy") == 0)
		{
			arguments.diskType = DiskType::FloppyDisk;
		}
		else if (strcmp(argv[i], "--hdd") == 0)
		{
			arguments.diskType = DiskType::HardDiskDrive;
		}
		else if (strcmp(argv[i], "--mbr") == 0)
		{
			if (i + 1 < argc)
			{
				arguments.mbrFile = argv[i + 1];
				i += 1;
			}
			else
			{
				printf("Error: No MBR file given.\n");
				return false;
			}
		}
		else if (strcmp(argv[i], "--bootloader") == 0)
		{
			if (i + 1 < argc)
			{
				arguments.bootloaderFile = argv[i + 1];
				i += 1;
			}
			else
			{
				printf("Error: No bootloader file given.\n");
				return false;
			}
		}
		else if (strcmp(argv[i], "--kernel") == 0)
		{
			if (i + 1 < argc)
			{
				arguments.kernelFile = argv[i + 1];
				i += 1;
			}
			else
			{
				printf("Error: No kernel file given.\n");
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

	// Makes sure the required arguments were given
	if (strcmp(arguments.bootloaderFile, "") == 0)
	{
		printf("Error: Bootloader file must be supplied.\n");
		return false;
	}
	else if (strcmp(arguments.kernelFile, "") == 0)
	{
		printf("Error: Kernel file must be supplied.\n");
		return false;
	}
	else if (strcmp(arguments.outputName, "") == 0)
	{
		printf("Error: Output file name must be supplied.\n");
		return false;
	}

	if (arguments.diskType == DiskType::HardDiskDrive &&
	   strcmp(arguments.mbrFile, "") == 0)
	{
		printf("Error: Hard Disk must have an MBR.\n");
		return false;
	}

	return true;
}
