/*  ===== Date Created: 11 September, 2020 =====  */

#include "log.hpp"
#include "display.hpp"
#include "interrupts/interruptDescriptorTable.hpp"
#include "memory/virtualAllocator.hpp"

extern "C" void start_kernel()
{
	init_interrupt_descriptor_table();
	
	// clear_screen();
	move_cursor(8, 0); // This is because the bootloader logged some text
	log_info("Starting kernel...");
	log_init();
	print("\n");

	init_virtual_allocator();
	
	log_info("\nFinished, now hanging...");
	while (true);
}
