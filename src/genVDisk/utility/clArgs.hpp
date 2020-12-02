//  ===== Date Created: 02 December, 2020 ===== 

#if !defined(CLARGS_HPP)
#define CLARGS_HPP

#include "utility/common.hpp"

struct CLArgs
{
	const u8* outputName = "";
	const u8* bootloaderFile = "";
};

bool handle_command_line_args(s32 argc, const u8* argv[], CLArgs& arguments);

#endif
