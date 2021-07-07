//  ===== Date Created: 02 December, 2020 ===== 

#include "display/textDisplay.hpp"

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

extern "C" void kmain(u32 bootloaderLinesPrinted, MemoryMap* memoryMap)
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

	// Testing grounds
	void* address0 = malloc(128);
	void* address1 = malloc(16);
	free(address0);
	void* address2 = malloc(96); // should use same pointer as address0
	void* address3 = malloc(64); // after address1
	void* address4 = malloc(28); // after address2

	printf("0: %x\t1: %x\t2: %x\n3: %x\t4: %x\n", address0, address1, address2, address3, address4);

	u32* test = (u32*) calloc(1, sizeof(test));
	printf("Test (%x): %d\n", test, *test);
	
	// The end...
	log_info("\nFinished, now hanging...");
	while (true);
}
