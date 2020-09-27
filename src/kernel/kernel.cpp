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

	print_char('\n');
	print_integer(a);
	print("\t\t\t");
	print_integer(b);
	print("\t\t\t");
	print_integer(c);
	print("\t\t\t");
	print_integer(d);
	print("\t\t\t");
	print_char('\n');
	
	print_hex_integer(a);
	print("\t\t");
	print_hex_integer(b);
	print("\t\t");
	print_hex_integer(c);
	print("\t\t");
	print_hex_integer(d);
	print("\t\t\t");
	print_char('\n');
	
	log_info("\nFinished, now hanging...");
	while (true);
}
