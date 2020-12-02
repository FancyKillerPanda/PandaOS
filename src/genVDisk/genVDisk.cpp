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
		printf("Info: Writing floppy disk.\n");
		
		numberOfBlocksWritten += write_data_as_blocks(outputFile, bootloaderData, bootloaderSize, 0);
		printf("Info: Wrote bootloader to disk (%zu sectors).\n", numberOfBlocksWritten);
	} break;
	}

	// Writes magic numbers for the bootloader
	// NOTE(fkp): The minus 1 is because we don't want to include
	// the first sector in what we load.
	u16 bootloaderSectors = ((bootloaderSize + 511) / 512) - 1;
	u16 magicBootloaderSize = (bootloaderSectors & 0x00ff) << 8;
	magicBootloaderSize |= (bootloaderSectors & 0xff00) >> 8;
		
	fseek(outputFile, 503, SEEK_SET);
	fwrite(&magicBootloaderSize, 1, 2, outputFile);
	printf("Info: Wrote magic numbers for bootloader.\n");
	
	fclose(outputFile);
}
