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
	
	log_info("\nFinished, now hanging...");
	while (true);
}
