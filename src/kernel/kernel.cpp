/*  ===== Date Created: 11 September, 2020 =====  */

#include "display.hpp"

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
	move_cursor(14, 0); // This is because the bootloader logged some text
	log_info("Starting kernel...");
	log_init();
	print("\n");

	printf("Video Mode: %dx%dpx, %dbpp, framebuffer at %x\n", videoMode->screenWidth, videoMode->screenHeight, videoMode->bitsPerPixel, videoMode->frameBufferPointer);
	
	read_memory_map(memoryMap);
	init_physical_allocator(memoryMap);
	init_virtual_allocator();
	init_heap_allocator();
	
	log_info("\nFinished, now hanging...");
	while (true);
}
