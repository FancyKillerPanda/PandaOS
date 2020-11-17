/*  ===== Date Created: 11 September, 2020 =====  */

#include "log.hpp"
#include "display.hpp"
#include "interrupts/interruptDescriptorTable.hpp"
#include "memory/physicalAllocator.hpp"
#include "memory/virtualAllocator.hpp"
#include "memory/heapAllocator.hpp"
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
	init_heap_allocator();

	u32* memory0 = (u32*) malloc(32);
	u32* memory1 = (u32*) malloc(11);
	u32* memory2 = (u32*) malloc(57);
	u32* memory3 = (u32*) malloc(1004);

	*memory0 = 1234;
	*memory1 = 1234;
	*memory2 = 1234;
	*memory3 = 1234;
	
	log_info("\nFinished, now hanging...");
	while (true);
}
