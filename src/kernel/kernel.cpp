/*  ===== Date Created: 11 September, 2020 =====  */

#include "log.hpp"
#include "display.hpp"
#include "interrupts/interruptDescriptorTable.hpp"
#include "memory/physicalAllocator.hpp"
#include "memory/virtualAllocator.hpp"
#include "memory/memoryMap.hpp"

extern "C" void start_kernel(MemoryMap* memoryMap)
{
	init_interrupt_descriptor_table();

	// clear_screen();
	move_cursor(14, 0); // This is because the bootloader logged some text
	log_info("Starting kernel...");
	log_init();
	print("\n");

	read_memory_map(memoryMap);
	init_physical_allocator(memoryMap);
	init_virtual_allocator();

	// Debug testing
	u32 a = 1;
	u32 b = 2;
	u32 c = 2;
	ASSERT(b == c);
	ASSERT(a == b);
	
	allocate_virtual_range((void*) 0x400000, 9000);
	
	log_info("\nFinished, now hanging...");
	while (true);
}
