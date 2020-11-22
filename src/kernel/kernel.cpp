/*  ===== Date Created: 11 September, 2020 =====  */

#include "display/textDisplay.hpp"
#include "display/graphicalDisplay.hpp"

#include "utility/log.hpp"
#include "interrupts/interruptDescriptorTable.hpp"
#include "display/videoMode.hpp"

#include "memory/physicalAllocator.hpp"
#include "memory/virtualAllocator.hpp"
#include "memory/heapAllocator.hpp"
#include "memory/memoryMap.hpp"

extern "C" void start_kernel(MemoryMap* memoryMap, VideoMode* videoMode)
{
	init_interrupt_descriptor_table();

	// clear_screen();
	// move_cursor(14, 0); // This is because the bootloader logged some text
	move_cursor(16, 0); // This is because the bootloader logged some text
	log_info("Starting kernel...");
	log_init();
	print("\n");

	
	read_memory_map(memoryMap);
	init_physical_allocator(memoryMap);
	init_virtual_allocator();
	init_heap_allocator();

	init_video(videoMode);
	draw_rect(100, 100, 200, 100, 0xff0000);
	// draw_rect(0, 0, 300, 300, 0x00ff0000);
	
	log_info("\nFinished, now hanging...");
	while (true);
}
