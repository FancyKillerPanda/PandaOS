//  ===== Date Created: 14 September, 2020 ===== 

#if !defined(CLARGS_HPP)
#define CLARGS_HPP

#include "utility.hpp"

#define STACK_ARRAY_LENGTH(array) (sizeof(array) / sizeof(*array))

struct CLArgs
{
	const u8* imageName = "";
	const u8* imagePath = "";
	const u8* volumeBootRecordFile = "";
	usize hardDiskSize = 0;
	// TODO(fkp): Array of files
};

bool handle_command_line_args(i32 argc, const u8* argv[], CLArgs* arguments);

#endif
