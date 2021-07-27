//  ===== Date Created: 02 December, 2020 ===== 

#if !defined(CLARGS_HPP)
#define CLARGS_HPP

#include "utility/common.hpp"

enum class DiskType
{
	FloppyDisk,
	HardDiskDrive,
};

struct CLArgs
{
	const u8* outputName = "";
	DiskType diskType = DiskType::HardDiskDrive;
	const u8* mbrFile = "";
	const u8* bootloaderFile = "";
	const u8* kernelFile = "";
};

bool handle_command_line_args(s32 argc, const u8* argv[], CLArgs& arguments);

#endif
