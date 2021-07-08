//  ===== Date Created: 02 December, 2020 ===== 

#include "display/graphicDisplay.hpp"
#include "display/textDisplay.hpp"
#include "display/videoMode.hpp"

#include "interrupts/interruptDescriptorTable.hpp"

#include "memory/heapAllocator.hpp"
#include "memory/memoryMap.hpp"
#include "memory/operations.hpp"
#include "memory/physicalAllocator.hpp"
#include "memory/virtualAllocator.hpp"

#include "system/common.hpp"

#include "utility/log.hpp"
#include "utility/magic.hpp"

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
	draw_rect(videoInfo.screenWidth - 2, 0, 50, videoInfo.screenHeight, 0x00ff00);
	draw_rect(0, videoInfo.screenHeight - 5, videoInfo.screenWidth, 50, 0x00ff00);
	
	// The end...
	log_info("\nFinished, now hanging...");
	while (true);
}
