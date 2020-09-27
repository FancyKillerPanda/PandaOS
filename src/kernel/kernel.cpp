/*  ===== Date Created: 11 September, 2020 =====  */

#include "system.hpp"
#include "display.hpp"
#include "log.hpp"
#include "display.hpp"
#include "interrupts/interruptDescriptorTable.hpp"
#include "interrupts/handleInterrupts.hpp"

extern "C" void start_kernel()
{
	init_interrupt_descriptor_table();
	
	// clear_screen();
	move_cursor(8, 0); // This is because the bootloader logged some text
	log_info("Starting kernel...");
	log_init();

	u8 a = 5;
	u8 b = 0;
	// u8 c = b / a;
	// u8 c = a / b;

	log_info("\nFinished, now hanging...");
	while (true);
}
