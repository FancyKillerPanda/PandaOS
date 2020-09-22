/*  ===== Date Created: 11 September, 2020 =====  */

#include "system.hpp"
#include "display.hpp"
#include "log.hpp"

extern "C" void start_kernel()
{
	// clear_screen();
	move_cursor(8, 0); // This is because the bootloader logged some text
	log_info("Starting kernel...");
	log_init();

	print("\n");
	log_info("This is information.");
	log_warning("This is a warning.");
	log_error("This is an error.");
	
	while (true);
}
