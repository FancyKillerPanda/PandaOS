//  ===== Date Created: 02 December, 2020 ===== 

#include "system/common.hpp"
#include "display/textDisplay.hpp"

extern "C" void kmain()
{
	move_cursor(11, 0);
	print("Hello, world!\n");
	print("This is a message from the kernel...");
	
	while (true);
}
