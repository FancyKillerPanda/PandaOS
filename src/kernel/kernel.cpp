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

	u32 a = 123456789;

	log_plain("\nPlain old logging.");
	log_info_plain("Plain old information.");
	log_warning_plain("Plain old warning.");
	log_error_plain("Plain old error.");

	log("\nFormatted logging: %c", 'a');
	log_info("Formatted information: %d", a);
	log_warning("Formatted warning: %x", a);
	log_error("Formatted error: %s", "hello");

	
	log_info("\nFinished, now hanging...");
	while (true);
}
