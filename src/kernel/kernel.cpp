//  ===== Date Created: 02 December, 2020 ===== 

#include "display/textDisplay.hpp"

#include "interrupts/interruptDescriptorTable.hpp"

#include "memory/memoryMap.hpp"
#include "memory/operations.hpp"
#include "memory/physicalAllocator.hpp"
#include "memory/virtualAllocator.hpp"
#include "memory/heapAllocator.hpp"

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
	log_info("Zeroed BSS block from %10x to %10x (%x bytes).",
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
	init_virtual_allocator();
	init_heap_allocator();

	// Testing grounds
	u32* address0 = (u32*) malloc(sizeof(address0));
	u32* address1 = (u32*) malloc(sizeof(address1) * 2);
	u32* address2 = (u32*) malloc(sizeof(address2));

	*address0 = 5;
	address1[0] = 10;
	address1[1] = 15;
	*address2 = 20;

	printf("address0: %x\t*address0: %d\n"
		   "address1: %x\taddress1[0]: %d    address1[1]: %d\n"
		   "address2: %x\t*address2: %d\n",
		   address0, *address0,
		   address1, address1[0], address1[1],
		   address2, *address2);
	
	// The end...
	log_info("\nFinished, now hanging...");
	while (true);
}
