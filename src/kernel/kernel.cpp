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

// NOTE(fkp): Testing only, not important
u8 testBitmap[] = {
	0b01010100, 0b00000000, 0b00000000,
	0b01101001, 0b00000000, 0b00000000,
	0b01101010, 0b01000000, 0b00000000,
	0b01101010, 0b10010000, 0b00000000,
	0b01101010, 0b10100100, 0b00000000,
	0b01101010, 0b10101001, 0b00000000,
	0b01101010, 0b10101010, 0b01000000,
	0b01101010, 0b10101001, 0b00000000,
	0b01101010, 0b10100100, 0b00000000,
	0b01101010, 0b10101001, 0b00000000,
	0b01101010, 0b01101010, 0b01000000,
	0b01101001, 0b00011010, 0b10010000,
	0b01100100, 0b00000110, 0b10100100,
	0b01010000, 0b00000001, 0b10010000,
	0b00000000, 0b00000000, 0b01000000,
	0b00000000, 0b00000000, 0b00000000,
};

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
	u32 colours[] = { 0x000000, 0xffffff };
	draw_bitmap_extended(testBitmap, 100, 100, 12, 16, colours, 2);
	
	// The end...
	log_info("\nFinished, now hanging...");
	while (true);
}
