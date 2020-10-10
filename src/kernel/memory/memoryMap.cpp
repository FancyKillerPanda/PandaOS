//  ===== Date Created: 11 October, 2020 =====

#include "memory/memoryMap.hpp"
#include "log.hpp"
#include "system/display.hpp"

void read_memory_map(MemoryMap* memoryMap)
{
	log_info("===== Memory Map =====");
	
	for (u32 i = 0; i < memoryMap->numberOfEntries; i++)
	{
		MemoryMapEntry entry = memoryMap->entries[i];

		if (entry.regionLength == 0)
		{
			continue;
		}

		const char* type;

		switch (entry.regionType)
		{
		case MemoryType::Free:
		{
			type = "Free";
		} break;
		
		case MemoryType::Reserved:
		{
			type = "Reserved";
		} break;
		
		case MemoryType::Reclaimable:
		{
			type = "Reclaimable";
		} break;
		
		case MemoryType::NonVolatile:
		{
			type = "Non-Volatile";
		} break;
		
		case MemoryType::Bad:
		{
			type = "Bad";
		} break;

		default:
		{
			type = "Reserved (unknown)";
			entry.regionType = MemoryType::Reserved;
		} break;
		}
		
		// TODO(fkp): We shouldn't really be down-casting, but
		// hopefully it doesn't cause too much of a headache since
		// it's just for logging.
		printf("%x to %x: %s\n", (u32) entry.baseAddress, (u32) (entry.baseAddress + entry.regionLength), type);
	}
	
	log_info("======================\n");
}

