/*  ===== Date Created: 11 September, 2020 =====  */

#include "log.hpp"
#include "display.hpp"
#include "interrupts/interruptDescriptorTable.hpp"
#include "memory/physicalAllocator.hpp"

extern "C" void start_kernel()
{
	init_interrupt_descriptor_table();
	
	// clear_screen();
	move_cursor(8, 0); // This is because the bootloader logged some text
	log_info("Starting kernel...");
	log_init();
	print("\n");

	u8* p1 = allocate_physical_page();
	u8* p2 = allocate_physical_page();
	u8* p3 = allocate_physical_page();
	printf("P1: %x | P2: %x | P3: %x\n", p1, p2, p3);

	free_physical_page(p2);
	u8* p4 = allocate_physical_page();
	u8* p5 = allocate_physical_page();
	printf("P4: %x | P5: %x\n", p4, p5);

	free_physical_page(p4 + 1);

	for (u32 i = 0; i < 253; i++)
	{
		allocate_physical_page();
	}
	
	log_info("\nFinished, now hanging...");
	while (true);
}
