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
	
	extern void print_integer(u32, u8 = 0x07);
	print_integer(small);
	print("\n");
	print_integer(medium);
	print("\n");
	print_integer(large);
	print("\n");
	
	extern void print_hex_integer(u32, u8 = 0x07);
	print_hex_integer(small);
	print("\n");
	print_hex_integer(medium);
	print("\n");
	print_hex_integer(large);
	print("\n");
	
	while (true);
}
