//  ===== Date Created: 11 October, 2020 =====

#include "memory/memoryMap.hpp"
#include "memory/operations.hpp"
#include "log.hpp"
#include "system/display.hpp"

// TODO(fkp): Better sort algorithm
void sort_memory_map(MemoryMap* memoryMap)
{
	for (u32 i = 0; i < memoryMap->numberOfEntries - 1; i++)
	{
		u32 indexOfSmallest = i;
		
		for (u32 j = i + 1; j < memoryMap->numberOfEntries; j++)
		{
			if (memoryMap->entries[j].baseAddress < memoryMap->entries[i].baseAddress)
			{
				indexOfSmallest = j;
			}
		}

		// Swap
		MemoryMapEntry temp;
		copy_memory(&memoryMap->entries[i], &temp, sizeof(MemoryMapEntry));
		copy_memory(&memoryMap->entries[indexOfSmallest], &memoryMap->entries[i], sizeof(MemoryMapEntry));
		copy_memory(&temp, &memoryMap->entries[indexOfSmallest], sizeof(MemoryMapEntry));
	}
}

u32 get_memory_priority(MemoryMapEntry& entry)
{
	switch (entry.regionType)
	{
	case MemoryType::Reserved:		return 5;
	case MemoryType::Bad:			return 4;
	case MemoryType::NonVolatile:	return 3;
	case MemoryType::Reclaimable:	return 2;
	case MemoryType::Free:			return 1;
	case MemoryType::Unknown:		return 0;
	default:						return 0;
	}
}

void adjust_memory_map(MemoryMap* memoryMap)
{
	sort_memory_map(memoryMap);

	// Overlapping regions
	for (u32 i = 0; i < memoryMap->numberOfEntries - 1; i++)
	{
		MemoryMapEntry& currentEntry = memoryMap->entries[i];
		MemoryMapEntry& nextEntry = memoryMap->entries[i + 1];
		s64 overlapSize = (currentEntry.baseAddress + currentEntry.regionLength) - nextEntry.baseAddress;
		
		if (overlapSize > 0)
		{
			if (get_memory_priority(currentEntry) > get_memory_priority(nextEntry))
			{
				// Checks if the next entry is completely inside this one
				if ((u64) overlapSize > nextEntry.regionLength)
				{
					copy_memory(&nextEntry + 1, &nextEntry, sizeof(MemoryMapEntry) * (memoryMap->numberOfEntries - i + 2));
					memoryMap->numberOfEntries -= 1;
				}
				else
				{
					nextEntry.baseAddress += overlapSize;
					nextEntry.regionLength -= overlapSize;
				}
			}
			else if (get_memory_priority(currentEntry) < get_memory_priority(nextEntry))
			{
				// Checks if this entry and the next one are exactly the same region
				if ((u64) overlapSize == currentEntry.regionLength)
				{
					copy_memory(&nextEntry, &currentEntry, sizeof(MemoryMapEntry) * (memoryMap->numberOfEntries - i));
					memoryMap->numberOfEntries -= 1;
				}
				else
				{
					currentEntry.regionLength -= (u64) overlapSize;
				}
			}
		}
	}

	
}

void print_memory_map(MemoryMap* memoryMap)
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
		} break;
		}
		
		// TODO(fkp): We shouldn't really be down-casting, but
		// hopefully it doesn't cause too much of a headache since
		// it's just for logging.
		printf("%x to %x: %s\n", (u32) entry.baseAddress, (u32) (entry.baseAddress + entry.regionLength), type);
	}
		
	log_info("======================\n");
}

void read_memory_map(MemoryMap* memoryMap)
{
	if (memoryMap->numberOfEntries == 0)
	{
		log_warning("No memory map entries.");
		return;
	}
	
	memoryMap->entries[0].baseAddress = 0x0110;
	memoryMap->entries[0].regionLength = 0x0010;
	memoryMap->entries[0].regionType = MemoryType::Free;
	
	memoryMap->entries[1].baseAddress = 0x0110;
	memoryMap->entries[1].regionLength = 0x0010;
	memoryMap->entries[1].regionType = MemoryType::Reserved;
	
	memoryMap->entries[2].baseAddress = 0x0120;
	memoryMap->entries[2].regionLength = 0x0010;
	memoryMap->entries[2].regionType = MemoryType::Free;
	
	memoryMap->entries[3].baseAddress = 0x0130;
	memoryMap->entries[3].regionLength = 0x0010;
	memoryMap->entries[3].regionType = MemoryType::Free;
	
	memoryMap->entries[4].baseAddress = 0x0140;
	memoryMap->entries[4].regionLength = 0x0010;
	memoryMap->entries[4].regionType = MemoryType::Free;
	
	adjust_memory_map(memoryMap);
	print_memory_map(memoryMap);
}

