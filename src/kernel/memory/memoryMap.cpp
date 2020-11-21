//  ===== Date Created: 11 October, 2020 =====

#include "memory/memoryMap.hpp"
#include "memory/operations.hpp"
#include "utility/log.hpp"
#include "system/display.hpp"

u32 get_memory_priority(MemoryMapEntry& entry)
{
	switch (entry.regionType)
	{
	case MemoryType::Bad:			return 5;
	case MemoryType::NonVolatile:	return 4;
	case MemoryType::Reserved:		return 3;
	case MemoryType::Reclaimable:	return 2;
	case MemoryType::Free:			return 1;
	case MemoryType::Unknown:		return 0;
	default:						return 0;
	}
}

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
		memcpy(&temp, &memoryMap->entries[i], sizeof(MemoryMapEntry));
		memcpy(&memoryMap->entries[i], &memoryMap->entries[indexOfSmallest], sizeof(MemoryMapEntry));
		memcpy(&memoryMap->entries[indexOfSmallest], &temp, sizeof(MemoryMapEntry));
	}
}

void identify_unknown_regions(MemoryMap* memoryMap)
{
	for (u32 i = 0; i < memoryMap->numberOfEntries; i++)
	{
		if ((u32) memoryMap->entries[i].regionType == 0 ||
			memoryMap->entries[i].regionType >= MemoryType::Count)
		{
			memoryMap->entries[i].regionType = MemoryType::Unknown;
		}
	}
}

void remove_overlapping_regions(MemoryMap* memoryMap)
{
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
					memcpy(&nextEntry, &nextEntry + 1, sizeof(MemoryMapEntry) * (memoryMap->numberOfEntries - i + 2));
					memoryMap->numberOfEntries -= 1;
					
					// NOTE(fkp): This should be ok as overflow is well defined for unsigned types
					i -= 1;
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
					memcpy(&currentEntry, &nextEntry, sizeof(MemoryMapEntry) * (memoryMap->numberOfEntries - i));
					memoryMap->numberOfEntries -= 1;
					i -= 1;
				}
				else
				{
					currentEntry.regionLength -= (u64) overlapSize;
				}
			}
		}
	}
}

void combine_adjacent_regions(MemoryMap* memoryMap)
{
	for (u32 i = 0; i < memoryMap->numberOfEntries - 1; i++)
	{
		MemoryMapEntry& currentEntry = memoryMap->entries[i];
		MemoryMapEntry& nextEntry = memoryMap->entries[i + 1];

		if (currentEntry.regionType == nextEntry.regionType &&
			currentEntry.baseAddress + currentEntry.regionLength == nextEntry.baseAddress)
		{
			currentEntry.regionLength += nextEntry.regionLength;
			memoryMap->numberOfEntries -= 1;
			i -= 1;
			
			memcpy(&nextEntry, &nextEntry + 1, sizeof(MemoryMapEntry) * (memoryMap->numberOfEntries - i + 2));
		}
	}
}

void adjust_memory_map(MemoryMap* memoryMap)
{
	identify_unknown_regions(memoryMap);
	sort_memory_map(memoryMap);
	remove_overlapping_regions(memoryMap);
	combine_adjacent_regions(memoryMap);
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
	
	// TODO(fkp): Check ACPI attributes
	adjust_memory_map(memoryMap);
	print_memory_map(memoryMap);
}

