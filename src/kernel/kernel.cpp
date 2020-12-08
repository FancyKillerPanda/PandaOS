//  ===== Date Created: 02 December, 2020 ===== 

#include "system/common.hpp"
#include "display/textDisplay.hpp"

extern "C" void kmain()
{
	move_cursor(11, 0);
	print("Hello, world!\n");
	print("This is a message from the kernel...\n");
	
	// Testing grounds:
	u8 small = 4;
	u16 medium = 250;
	u32 large = 0x12345678;
	
	printf("Small: %d\nMedium: %d\nLarge: %d\n\n"
		   "Small: %x\nMedium: %x\nLarge: %x\n",
		   small, medium, large, small, medium, large);
	
	while (true);
}
