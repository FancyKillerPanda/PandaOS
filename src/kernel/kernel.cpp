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

	u32 a = 5;
	u32 b = 10;
	u32 c = 14;
	u32 d = 123456789;

	printf("Hello, this is a test string.\n");
	printf("String: %s\n", "hello");
	printf("Char: %c\n", 'f');
	printf("Int: %d\n", a);
	printf("Hex: %x\n", d);
	printf("Percent: %%\n");
	
	log_info("\nFinished, now hanging...");
	while (true);
}
