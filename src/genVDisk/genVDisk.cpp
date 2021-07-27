//  ===== Date Created: 02 December, 2020 ===== 

#include <stdio.h>
#include "utility/common.hpp"
#include "utility/clArgs.hpp"
#include "utility/fileIO.hpp"

bool write_descriptor_file(const CLArgs& clArgs);
bool write_extent_file(const CLArgs& clArgs);

int main(s32 argc, const u8* argv[])
{
	CLArgs clArgs {};
	
	if (!handle_command_line_args(argc, argv, clArgs))
	{
		return 1;
	}

	if (clArgs.diskType == DiskType::HardDiskDrive &&
		!write_descriptor_file(clArgs))
	{
		return 1;
	}
	
	if (!write_extent_file(clArgs))
	{
		return 1;
	}
}

bool write_descriptor_file(const CLArgs& clArgs)
{
	// Calculates disk geometry
	usize diskNumberOfSectors = clArgs.diskSize / 512;
	usize numberOfHeads = 16; // Equal to max for IDE
	usize numberOfSectors = 63;
	usize numberOfCylinders = diskNumberOfSectors / (numberOfHeads * numberOfSectors);

	// Max number of cylinders
	if (numberOfCylinders > 16383)
	{
		numberOfCylinders = 16383;
	}

	printf("Info: Cylinders: %zu, heads: %zu, sectors: %zu.\n",
		   numberOfCylinders, numberOfHeads, numberOfSectors);
	printf("Info: Writing descriptor file.\n");
	
	// Opens the descriptor file for writing
	const u8* extentFileName = concat_strings(clArgs.outputName, "-flat.vmdk");
	const u8* descriptorFileName = concat_strings(clArgs.outputName, ".vmdk");
	FILE* descriptorFile = fopen(descriptorFileName, "w");

	// Header
	fprintf(descriptorFile,
			"# Disk Descriptor File\n"
			"version=1\n"
			"encoding=\"UTF-8\"\n"
			"CID=12345678\n"
			"parentCID=ffffffff\n"
			"createType=\"monolithicFlat\"\n");
	
	// Extent description
	fprintf(descriptorFile,
		   "\n# Extent Description\n"
			"RW %lu FLAT \"%s\" 0\n",
			diskNumberOfSectors, extentFileName);

	// Disk database
	fprintf(descriptorFile,
			"\n# Disk Database\n"
			"ddb.virtualHWVersion=\"16\"\n"
			"ddb.geometry.cylinders=\"%lu\"\n"
			"ddb.geometry.heads=\"%lu\"\n"
			"ddb.geometry.sectors=\"%lu\"\n"
			"ddb.adapterType=\"ide\"\n"
			"ddb.toolsVersion=\"0\"\n",
			numberOfCylinders, numberOfHeads, numberOfSectors);
	
	// Cleanup
	fclose(descriptorFile);
	printf("Info: Wrote descriptor file.\n\n");
	return true;
}

bool write_extent_file(const CLArgs& clArgs)
{
	const u8* extentFileName;
	
	if (clArgs.diskType == DiskType::FloppyDisk)
	{
		extentFileName = concat_strings(clArgs.outputName, ".img");
	}
	else if (clArgs.diskType == DiskType::HardDiskDrive)
	{
		extentFileName = concat_strings(clArgs.outputName, "-flat.vmdk");
	}
	
	// Opens the output file for writing
	FILE* extentFile = fopen(extentFileName, "w");

	if (!extentFile)
	{
		printf("Error: Failed to create extent file '%s'.\n", clArgs.outputName);
		return false;
	}

	// Reads the entire bootloader file
	u8* bootloaderData = nullptr;
	usize bootloaderSize = 0;

	if (!read_entire_file(clArgs.bootloaderFile, &bootloaderData, &bootloaderSize))
	{
		fclose(extentFile);
		return false;
	}

	// Reads the entire kernel file
	u8* kernelData = nullptr;
	usize kernelSize = 0;

	if (!read_entire_file(clArgs.kernelFile, &kernelData, &kernelSize))
	{
		fclose(extentFile);
		return false;
	}

	usize numberOfBlocksWritten = 0;

	if (clArgs.diskType == DiskType::HardDiskDrive)
	{
		printf("Info: Writing hard disk extent.\n");
		
		// Reads the entire MBR file
		u8* mbrData = nullptr;
		usize mbrSize = 0;

		if (!read_entire_file(clArgs.mbrFile, &mbrData, &mbrSize))
		{
			fclose(extentFile);
			return false;
		}

		// Writes the MBR to the extent file
		usize mbrSectors = write_data_as_blocks(extentFile, mbrData, mbrSize, 0);
		ASSERT(mbrSectors == 1, "Error: Should not have more than one MBR sector.");
		numberOfBlocksWritten += mbrSectors;

		const u32 unused = 0xffffffff;
		const u32 attributes = 0x80; // Bootable
		const u32 type = 0x0c; // FAT32 LBA
		constexpr u32 MBR_PARTITION_ENTRY_OFFSET = 446;
		
		fseek(extentFile, MBR_PARTITION_ENTRY_OFFSET, SEEK_SET);
		fwrite(&attributes, 1, 1, extentFile);
		fwrite(&unused, 1, 3, extentFile);
		fwrite(&type, 1, 1, extentFile);
		fwrite(&unused, 1, 3, extentFile);
		fwrite(&numberOfBlocksWritten, 4, 1, extentFile); // Start of partition (LBA)
		fwrite(&clArgs.diskSize, 4, 1, extentFile); // Number of sectors
		fseek(extentFile, 0, SEEK_END);
		
		printf("Info: Wrote single MBR sector to disk.\n");
	}
	else
	{
		printf("Info: Writing floppy disk extent.\n");
	}

	// Writes the bootloader
	usize bootloaderStartSector = numberOfBlocksWritten;
	usize bootloaderSectors = write_data_as_blocks(extentFile, bootloaderData, bootloaderSize, 0);
	numberOfBlocksWritten += bootloaderSectors;
	printf("Info: Wrote bootloader to disk (start: %zu, %zu sector(s)).\n",
		   bootloaderStartSector, bootloaderSectors);

	// Writes the kernel
	usize kernelStartSector = numberOfBlocksWritten;
	usize kernelSectors = write_data_as_blocks(extentFile, kernelData, kernelSize, 0);
	numberOfBlocksWritten += kernelSectors;
	printf("Info: Wrote kernel to disk (start: %zu, %zu sector(s)).\n",
		   kernelStartSector, kernelSectors);

	// Writes magic numbers for the bootloader
	// NOTE(fkp): The minus 1 is because we don't want to include
	// the first sector in what we load.
	bootloaderSectors -= 1;
	u16 magicBootloaderStart = ENDIAN_SWAP_16(bootloaderStartSector);
	u16 magicBootloaderSize = ENDIAN_SWAP_16(bootloaderSectors);
	u16 magicKernelSize = ENDIAN_SWAP_16(kernelSectors);
	u16 magicKernelStart = ENDIAN_SWAP_16(bootloaderStartSector + bootloaderSectors + 1);

	fseek(extentFile, (bootloaderStartSector * 512) + 501, SEEK_SET);
	fwrite(&magicBootloaderStart, 1, 2, extentFile);
	fwrite(&magicBootloaderSize, 1, 2, extentFile);
	fwrite(&magicKernelStart, 1, 2, extentFile);
	fwrite(&magicKernelSize, 1, 2, extentFile);
	fseek(extentFile, 0, SEEK_END);
	
	printf("Info: Wrote magic numbers for bootloader.\n");

	usize paddingSectorSize = (clArgs.diskSize / 512) - numberOfBlocksWritten;
	write_data_as_blocks(extentFile, nullptr, 0, paddingSectorSize);
	printf("Info: Wrote %zu sectors of padding (%zu kiB).\n", paddingSectorSize, (paddingSectorSize * 512) / 1024);
	
	fclose(extentFile);
	printf("Info: Wrote extent file.\n");
	return true;
}
