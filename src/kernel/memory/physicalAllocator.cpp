//  ===== Date Created: 08 December, 2020 ===== 

#include "memory/physicalAllocator.hpp"

constexpr u32 NUMBER_OF_PAGE_FRAMES = 2048;
static_assert(NUMBER_OF_PAGE_FRAMES % 32 == 0,
			  "Number of page frames must be a multiple of 32.");

// NOTE(fkp): Zero is free, one is used
static u32 pageStatusBitmap[NUMBER_OF_PAGE_FRAMES / 32] = { 0 };

// Will be read from the memory map
u32 physicalAllocatorBase = 0x00;

// NOTE(fkp): Requirement is that we have an 8MB block of
// contiguous memory somewhere above the 8MB physical mark.
void init_physical_allocator(MemoryMap* memoryMap)
{
	constexpr u32 BLOCK_START = 0x00800000;
	constexpr u32 BLOCK_LENGTH = 0x00800000;
	
	// Picks the largest free region
	u64 largestRegionLength = 0;
	
	for (u32 i = 0; i < memoryMap->numberOfEntries; i++)
	{
		MemoryMapEntry& entry = memoryMap->entries[i];
		
		if (entry.baseAddress < BLOCK_START &&
			entry.baseAddress + entry.regionLength < BLOCK_LENGTH + BLOCK_LENGTH)
		{
			continue;
		}

		if (entry.regionType == MemoryType::Free &&
			entry.regionLength > largestRegionLength)
		{
			if (entry.baseAddress < BLOCK_START)
			{
				physicalAllocatorBase = BLOCK_START;
				largestRegionLength = entry.regionLength - (BLOCK_START - entry.baseAddress);
			}
			else
			{
				physicalAllocatorBase = entry.baseAddress;
				largestRegionLength = entry.regionLength;
			}
			
		}
	}
	
	if (!physicalAllocatorBase)
	{
		log_error("Failed to pick a base address for the physical allocator.");
		while (true);
	}

	if (largestRegionLength < NUMBER_OF_PAGE_FRAMES * PAGE_SIZE)
	{
		log_error("Failed to find a large enough region of physical memory.");
		while (true);
	}

	log_info("Physical allocator using memory from %x to %x (%x bytes).",
			 physicalAllocatorBase,
			 (u32) (physicalAllocatorBase + largestRegionLength),
			 (u32) largestRegionLength);
}
