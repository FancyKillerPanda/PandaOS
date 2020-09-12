/*  ===== Date Created: 11 September, 2020 =====  */

#include "system.hpp"
#include "display.hpp"

extern "C" void start_kernel()
{
	clear_screen();
	print("Hello, it's\nFancyKillerPanda\b\b\b\b\b here!\n", 0x9f);
	print("This is a super\nlong line.\nIt should hopefully \nadvance to the\n next one.\n", 0x04);
	print("This\tis a long\tline\tthat is\tdelimited\tby\ttabs.\n");
	print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nHello\nthis is testing\nscrolling functionality.");
	while (true);
}
