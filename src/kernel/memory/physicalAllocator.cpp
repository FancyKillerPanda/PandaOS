//  ===== Date Created: 05 October, 2020 =====

#include "physicalAllocator.hpp"
#include "utility/log.hpp"

// TODO(fkp): These values are just temporary. Once we plan
// a proper memory map they will be changed to reflect that.
// NOTE(fkp): This assumes we have 8 MB of free RAM available in one
// location, which we probably shouldn't assume.
constexpr u32 NUMBER_OF_PAGE_FRAMES = 2048;
static_assert(NUMBER_OF_PAGE_FRAMES % 32 == 0, "Number of page frames must be a multiple of 32.");

// NOTE(fkp): Zero is free, one is used
u32 pageStatusBitmap[NUMBER_OF_PAGE_FRAMES / 32] = { 0 };

// Will be read from the memory map
u32 physicalAllocatorBase = 0x00;

void init_physical_allocator(MemoryMap* memoryMap)
{
	// Picks the largest free region above 1 MB
	u64 largestRegionLength = 0;
	
	for (u32 i = 0; i < memoryMap->numberOfEntries; i++)
	{
		if (memoryMap->entries[i].baseAddress < 0x100000)
		{
			continue;
		}

		if (memoryMap->entries[i].regionType == MemoryType::Free &&
			memoryMap->entries[i].regionLength > largestRegionLength)
		{
			largestRegionLength = memoryMap->entries[i].regionLength;
			physicalAllocatorBase = memoryMap->entries[i].baseAddress;
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

void* allocate_physical_page()
{
	for (u32 byte = 0; byte < NUMBER_OF_PAGE_FRAMES / 32; byte++)
	{
		u32 bitmapByte = pageStatusBitmap[byte];
		
		// This will reveal if any of the bits are 0
		if (bitmapByte ^ 0xffffffff)
		{
			for (u32 bit = 0; bit < 32; bit++)
			{
				u32 pageBit = 1 << (32 - bit + 1);
				bool pageTaken = bitmapByte & pageBit;
				
				if (!pageTaken)
				{
					u32 index = (byte * 32) + bit;
					pageStatusBitmap[byte] |= pageBit;
					
					return (void*) (physicalAllocatorBase + (index * PAGE_SIZE));
				}
			}

			log_error("Bitmap found byte but not bit. How?");
			while (true);
		}
	}

	// NOTE(fkp): We should hopefully not get here
	log_error("Failed to find free physical page.");
	while (true);
}

void free_physical_page(void* pageFrame)
{
	u32 pageOffset = (u32) pageFrame - physicalAllocatorBase;

	if (pageOffset % PAGE_SIZE != 0)
	{
		log_warning("Page frame pointer must be aligned to the page size. Nothing freed...");
		return;
	}
	
	u32 index = pageOffset / PAGE_SIZE;
	
	u32 byte = index / 32;
	u32 bit = index % 32;
	pageStatusBitmap[byte] ^= 1 << (32 - bit + 1);

	log_info("Freeing physical page at bit %d.", index);
}
