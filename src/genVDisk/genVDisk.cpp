//  ===== Date Created: 02 December, 2020 ===== 

#include <stdio.h>
#include "utility/common.hpp"
#include "utility/clArgs.hpp"
#include "utility/fileIO.hpp"
int main(s32 argc, const u8* argv[])
{
	CLArgs clArgs {};
	
	if (!handle_command_line_args(argc, argv, clArgs))
	{
		return 1;
	}

	// Opens the output file for writing
	FILE* outputFile = fopen(clArgs.outputName, "w");

	if (!outputFile)
	{
		printf("Error: Failed to create output file '%s'.\n", clArgs.outputName);
		return 1;
	}

	// Reads the entire bootloader file
	u8* bootloaderData = nullptr;
	usize bootloaderSize = 0;

	if (!read_entire_file(clArgs.bootloaderFile, &bootloaderData, &bootloaderSize))
	{
		fclose(outputFile);
		return 1;
	}

	// Handles disk-type specific logic
	switch (clArgs.diskType)
	{
	case DiskType::FloppyDisk:
	{
		usize numberOfBlocksWritten = 0;
		numberOfBlocksWritten += write_data_as_blocks(outputFile, bootloaderData, bootloaderSize, 0);
	} break;
	}

	fclose(outputFile);
}
