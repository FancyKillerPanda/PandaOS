//  ===== Date Created: 05 October, 2020 =====

#include "physicalAllocator.hpp"
#include "log.hpp"

// TODO(fkp): These values are just temporary. Once we plan
// a proper memory map they will be changed to reflect that.
constexpr u32 NUMBER_OF_PAGE_FRAMES = 256;
constexpr u32 PHYSICAL_ALLOCATOR_BASE = 0xd0000000;

static_assert(NUMBER_OF_PAGE_FRAMES % 32 == 0, "Number of page frames must be a multiple of 32.");
// NOTE(fkp): Zero is free, one is used
u32 pageStatusBitmap[NUMBER_OF_PAGE_FRAMES / 32] = { 0 };

u8* allocate_physical_page()
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
					
					return (u8*) (PHYSICAL_ALLOCATOR_BASE + (index * PAGE_SIZE));
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

void free_physical_page(u8* pageFrame)
{
	u32 pageOffset = (u32) pageFrame - PHYSICAL_ALLOCATOR_BASE;

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
