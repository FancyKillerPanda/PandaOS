//  ===== Date Created: 02 December, 2020 ===== 

#include <stdio.h>
#include "utility/common.hpp"
#include "utility/clArgs.hpp"

int main(s32 argc, const u8* argv[])
{
	CLArgs clArgs {};
	
	if (!handle_command_line_args(argc, argv, clArgs))
	{
		return 1;
	}
}
