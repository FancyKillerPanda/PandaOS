//  ===== Date Created: 02 December, 2020 ===== 

#include "display/textDisplay.hpp"
#include "memory/memoryMap.hpp"
#include "memory/physicalAllocator.hpp"
#include "system/common.hpp"
#include "utility/log.hpp"

extern "C" void kmain(u32 bootloaderLinesPrinted, MemoryMap* memoryMap)
{
	move_cursor(bootloaderLinesPrinted + 1, 0);
	log_info("PandaOS kernel!");
	
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
