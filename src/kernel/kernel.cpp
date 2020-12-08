//  ===== Date Created: 02 December, 2020 ===== 

#include "display/textDisplay.hpp"
#include "memory/memoryMap.hpp"
#include "system/common.hpp"
#include "utility/log.hpp"

extern "C" void kmain(MemoryMap* memoryMap)
{
	// TODO(fkp): Get number of lines logged from bootloader
	move_cursor(12, 0);
	log_info("PandaOS kernel!");
	
	// Testing grounds:
	printf("Pointer: %x, number: %d\n", memoryMap->entries, memoryMap->numberOfEntries);
	read_memory_map(memoryMap);
	
	log_info("\nFinished, now hanging...");
	while (true);
}
