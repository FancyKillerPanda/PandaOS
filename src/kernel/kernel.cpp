//  ===== Date Created: 02 December, 2020 ===== 

#include "display/textDisplay.hpp"
#include "interrupts/interruptDescriptorTable.hpp"
#include "memory/memoryMap.hpp"
#include "memory/operations.hpp"
#include "memory/physicalAllocator.hpp"
#include "system/common.hpp"
#include "utility/log.hpp"

#define PANDA_OS_MAGIC_STRING "PandaOS Magic!"
static const u8* pandaOSCorrectString; // Will be set later
extern const u8 pandaOSMagicString[sizeof(PANDA_OS_MAGIC_STRING)];

extern const usize bssBlockStart;
extern const usize bssBlockEnd;
static const usize* const kernelBSSBlockStart = &bssBlockStart;
static const usize* const kernelBSSBlockEnd = &bssBlockEnd;

extern "C" void kmain(u32 bootloaderLinesPrinted, MemoryMap* memoryMap)
{
	// Zeroes the BSS block
	const usize kernelBSSBlockSize = (usize) kernelBSSBlockEnd - (usize) kernelBSSBlockStart;
	memset((void*) kernelBSSBlockStart, 0, kernelBSSBlockSize);

	move_cursor(bootloaderLinesPrinted + 1, 0);
	log_info("PandaOS kernel!");
	log_info("Zeroed BSS block from %x to %x (%x bytes).",
			 kernelBSSBlockStart, kernelBSSBlockEnd, kernelBSSBlockSize);

	// Ensures that the entire kernel image was loaded
	pandaOSCorrectString = PANDA_OS_MAGIC_STRING;

	for (u8 i = 0; pandaOSCorrectString[i] != 0; i++)
	{
		if (pandaOSMagicString[i] != pandaOSCorrectString[i])
		{
			log_error("Magic string index %d (%d/'%c') is not correct (should be %c)!",
					  i, (u32) pandaOSMagicString[i],
					  pandaOSMagicString[i], pandaOSCorrectString[i]);
			while (true);
		}
	}

	log_info("Magic string is correct, entire kernel present!");

	// Other initialisation functions
	init_interrupt_descriptor_table();
	read_memory_map(memoryMap);
	init_physical_allocator(memoryMap);

	// Testing grounds
	printf("Char: %c\n", 'A');
	printf("Int: %d, %1d, %6d, %10d\n", 5, 5, 5, 5);
	printf("Hex: %x, %1x, %3x, %6x, %10x\n", 5, 5, 5, 5, 5);

	// The end...
	log_info("\nFinished, now hanging...");
	while (true);
}
