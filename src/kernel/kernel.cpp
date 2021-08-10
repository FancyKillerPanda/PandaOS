//  ===== Date Created: 02 December, 2020 ===== 

#include "display/graphicDisplay.hpp"
#include "display/videoMode.hpp"

#include "interrupts/interruptDescriptorTable.hpp"

#include "memory/heapAllocator.hpp"
#include "memory/memoryMap.hpp"
#include "memory/operations.hpp"
#include "memory/physicalAllocator.hpp"
#include "memory/virtualAllocator.hpp"

#include "multitasking/scheduler.hpp"

#include "system/common.hpp"

#include "utility/dynamicArray.hpp"
#include "utility/log.hpp"
#include "utility/magic.hpp"
#include "utility/textDisplay.hpp"

extern const usize bssBlockStart;
extern const usize bssBlockEnd;
static const usize* const kernelBSSBlockStart = &bssBlockStart;
static const usize* const kernelBSSBlockEnd = &bssBlockEnd;

extern "C" void kmain(u32 bootloaderLinesPrinted, MemoryMap* memoryMap, VideoMode* videoMode)
{
	// Zeroes the BSS block
	const usize kernelBSSBlockSize = (usize) kernelBSSBlockEnd - (usize) kernelBSSBlockStart;
	memset((void*) kernelBSSBlockStart, 0, kernelBSSBlockSize);

	move_cursor(bootloaderLinesPrinted + 1, 0);
	log_info("PandaOS kernel!");
	log_info("Zeroed BSS block from %10x to %10x (%x bytes).",
			 kernelBSSBlockStart, kernelBSSBlockEnd, kernelBSSBlockSize);

	// Other initialisation functions
	check_magic_string();
	init_interrupt_descriptor_table();
	read_memory_map(memoryMap);
	init_physical_allocator(memoryMap);
	init_virtual_allocator();
	init_heap_allocator();
	init_video(videoMode);

	// Testing grounds
	clear_screen(0x555555);

	DynamicArray<u32> array {};
	array.push(1);
	array.push(10);
	printf("Size: %d, index 0: %d, index 1: %d\n", array.size, array[0], array[1]);
	printf("Value popped: %d\n", array.pop());
	printf("Size: %d, index 0: %d\n", array.size, array[0]);
	array[1] = 5; // Should error
	
	switch_process();
	
	// The end...
	log_info("\nFinished, now hanging...");
	while (true);
}
