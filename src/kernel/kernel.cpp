//  ===== Date Created: 02 December, 2020 ===== 

#include "display/textDisplay.hpp"
#include "interrupts/interruptDescriptorTable.hpp"
#include "memory/memoryMap.hpp"
#include "memory/physicalAllocator.hpp"
#include "system/common.hpp"
#include "utility/log.hpp"

static const u8* pandaOSCorrectString = "PandaOS Magic!";
extern const u8* pandaOSMagicString;

extern "C" void kmain(u32 bootloaderLinesPrinted, MemoryMap* memoryMap)
{
	move_cursor(bootloaderLinesPrinted + 1, 0);
	log_info("PandaOS kernel!");

	// Ensures that the entire kernel image was loaded
	for (u8 i = 0; pandaOSCorrectString[i] != 0; i++)
	{
		if (pandaOSMagicString[i] != pandaOSCorrectString[i])
		{
			log_error("Magic string index %d (char: %c) is not correct (should be %c)!",
					  i, pandaOSMagicString[i], pandaOSCorrectString[i]);
			while (true);
		}
	}

	log_info("Magic string is correct, entire kernel present!");

	init_interrupt_descriptor_table();
	read_memory_map(memoryMap);
	init_physical_allocator(memoryMap);
	
	void* ptr0 = allocate_physical_page();
	void* ptr1 = allocate_physical_page();
	void* ptr2 = allocate_physical_page();
	free_physical_page(ptr1);
	void* ptr3 = allocate_physical_page();

	printf("%x, %x, %x, %x\n", ptr0, ptr1, ptr2, ptr3);
	
	log_info("\nFinished, now hanging...");
	while (true);
}
