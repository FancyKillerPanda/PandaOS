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

	// Reads the entire kernel file
	u8* kernelData = nullptr;
	usize kernelSize = 0;

	if (!read_entire_file(clArgs.kernelFile, &kernelData, &kernelSize))
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

		usize bootloaderSectors = write_data_as_blocks(outputFile, bootloaderData, bootloaderSize, 0);
		numberOfBlocksWritten += bootloaderSectors;
		printf("Info: Wrote bootloader to disk (%zu sector(s)).\n", bootloaderSectors);

		usize kernelSectors = write_data_as_blocks(outputFile, kernelData, kernelSize, 0);
		numberOfBlocksWritten += kernelSectors;
		printf("Info: Wrote kernel to disk (%zu sector(s)).\n", kernelSectors);
	} break;
	}

	// Writes magic numbers for the bootloader
	// NOTE(fkp): The minus 1 is because we don't want to include
	// the first sector in what we load.
	// TODO(fkp): Make a macro for the little-endian stuff
	u16 bootloaderSectors = ((bootloaderSize + 511) / 512) - 1;
	u16 magicBootloaderSize = (bootloaderSectors & 0x00ff) << 8;
	magicBootloaderSize |= (bootloaderSectors & 0xff00) >> 8;

	u16 kernelSectors = ((kernelSize + 511) / 512) - 1;
	u16 magicKernelSize = (kernelSectors & 0x00ff) << 8;
	magicKernelSize |= (kernelSectors & 0xff00) >> 8;
	
	// Adds 1 (but in little endian)
	u16 magicKernelStart = magicBootloaderSize + 0x0100;
	
	fseek(outputFile, 503, SEEK_SET);
	fwrite(&magicBootloaderSize, 1, 2, outputFile);
	fwrite(&magicKernelStart, 1, 2, outputFile);
	fwrite(&magicKernelSize, 1, 2, outputFile);
	
	printf("Info: Wrote magic numbers for bootloader.\n");
	
	fclose(outputFile);
}
