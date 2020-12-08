//  ===== Date Created: 02 December, 2020 ===== 

#include "display/textDisplay.hpp"
#include "system/common.hpp"
#include "utility/log.hpp"

extern "C" void kmain()
{
	move_cursor(11, 0);
	print("Hello, world!\n");
	print("This is a message from the kernel...\n\n");
	
	// Testing grounds:
	u32 large = 0x12345678;
	
	log_info("Hello there!");
	log_warning("This is a number: %d", 5);
	log_error("This is another number: %x", large);

	ASSERT(large == 0x12345678, "Large equal!");
	ASSERT(large != 0x12345678, "Large not equal!");
	
	log_info("Finished, now hanging...");
	while (true);
}
