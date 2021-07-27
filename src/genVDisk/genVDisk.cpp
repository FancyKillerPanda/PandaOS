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

	usize numberOfBlocksWritten = 0;

	if (clArgs.diskType == DiskType::HardDiskDrive)
	{
		printf("Info: Writing hard disk.\n");
		
		// Reads the entire MBR file
		u8* mbrData = nullptr;
		usize mbrSize = 0;

		if (!read_entire_file(clArgs.mbrFile, &mbrData, &mbrSize))
		{
			fclose(outputFile);
			return 1;
		}

		// Writes the MBR to the extent file
		// TODO(fkp): Write partition data
		usize mbrSectors = write_data_as_blocks(outputFile, mbrData, mbrSize, 0);
		ASSERT(mbrSectors == 1, "Error: Should not have more than one MBR sector.");
		numberOfBlocksWritten += mbrSectors;
		printf("Info: Wrote single MBR sector to disk.\n");
	}
	else
	{
		printf("Info: Writing floppy disk.\n");
	}

	// Writes the bootloader
	usize bootloaderStartSector = numberOfBlocksWritten;
	usize bootloaderSectors = write_data_as_blocks(outputFile, bootloaderData, bootloaderSize, 0);
	numberOfBlocksWritten += bootloaderSectors;
	printf("Info: Wrote bootloader to disk (%zu sector(s)).\n", bootloaderSectors);

	// Writes the kernel
	usize kernelStartSector = numberOfBlocksWritten;
	usize kernelSectors = write_data_as_blocks(outputFile, kernelData, kernelSize, 0);
	numberOfBlocksWritten += kernelSectors;
	printf("Info: Wrote kernel to disk (%zu sector(s)).\n", kernelSectors);

	// Writes magic numbers for the bootloader
	// NOTE(fkp): The minus 1 is because we don't want to include
	// the first sector in what we load.
	bootloaderSectors -= 1;
	u16 magicBootloaderSize = ENDIAN_SWAP_16(bootloaderSectors);
	u16 magicKernelSize = ENDIAN_SWAP_16(kernelSectors);
	u16 magicKernelStart = ENDIAN_SWAP_16(bootloaderStartSector + bootloaderSectors + 1);
	
	fseek(outputFile, bootloaderStartSector + 503, SEEK_SET);
	fwrite(&magicBootloaderSize, 1, 2, outputFile);
	fwrite(&magicKernelStart, 1, 2, outputFile);
	fwrite(&magicKernelSize, 1, 2, outputFile);
	
	printf("Info: Wrote magic numbers for bootloader.\n");
	
	fclose(outputFile);
}
