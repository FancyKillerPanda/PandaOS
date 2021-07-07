//  ===== Date Created: 07 July, 2021 ===== 

#include "heapAllocator.hpp"
#include "virtualAllocator.hpp"

constexpr u32 HEAP_START = 0xc0500000;
constexpr u32 HEAP_SIZE = 0x00400000;

struct FreeRegion
{
	void* address = nullptr;
	usize size = 0;
};

// We currently allocate a single page for the list of free regions.
// Should this be more? Dynamic?
constexpr u32 FREE_REGIONS_LIST_ADDRESS = 0xc0400000;
constexpr u32 MAX_NUMBER_OF_FREE_REGIONS = PAGE_SIZE / sizeof(FreeRegion);

FreeRegion* freeRegionsList = (FreeRegion*) FREE_REGIONS_LIST_ADDRESS;
u32 freeRegionsCount = 0;

struct AllocationInfoBlock
{
	usize size;
};

void debug_print_free_regions_list()
{
	printf("\n===== Free Regions ====\n");
	
	for (u32 i = 0; i < freeRegionsCount; i++)
	{
		printf("\t%d: %x (%d bytes)\n", i, freeRegionsList[i].address, freeRegionsList[i].size);
	}

	printf("\n");
}

void init_heap_allocator()
{
	// Maps a single page where the list of free regions will go
	allocate_virtual_range(freeRegionsList, PAGE_SIZE);
	memset(freeRegionsList, 0, PAGE_SIZE);

	// Allocates the heap block
	allocate_virtual_range((void*) HEAP_START, HEAP_SIZE);

	// The heap block is currently the only free region
	freeRegionsList[0] = FreeRegion { (void*) HEAP_START, HEAP_SIZE };
	freeRegionsCount += 1;

	log_info("Initialised heap allocator.");
}

void* malloc(usize size)
{
	for (u32 i = 0; i < freeRegionsCount; i++)
	{
		FreeRegion& region = freeRegionsList[i];
		usize allocationSize = size + sizeof(AllocationInfoBlock);

		// Look for a free region big enough for the allocation
		if (region.size >= allocationSize)
		{
			void* allocation = region.address;
			*((AllocationInfoBlock*) allocation) = AllocationInfoBlock { allocationSize };

			region.address = (void*) (((u8*) region.address) + allocationSize);
			region.size -= allocationSize;

			// The free region block was exactly the right size
			if (region.size == 0)
			{
				freeRegionsCount -= 1;

				// Moves all the other elements forward
				memcpy(freeRegionsList + i, freeRegionsList + i + 1,
					   (freeRegionsCount - i) * sizeof(FreeRegion));
			}

			return (void*) ((u8*) allocation + sizeof(AllocationInfoBlock));
		}
	}

	log_error("Unable to allocate a heap region of length %d bytes.", size);
	while (true);
}

void* calloc(usize count, usize size)
{
	void* allocation = malloc(count * size);
	memset(allocation, 0, count * size);

	return allocation;
}

void free(void* pointer)
{
	if (!pointer)
	{
		return;
	}

	// Free region starts where the information block was
	AllocationInfoBlock& infoBlock = *(((AllocationInfoBlock*) pointer) - 1);
	FreeRegion newFreeRegion { &infoBlock, infoBlock.size };

	ASSERT(freeRegionsCount >= 1, "There should always be at least one free region.");
	FreeRegion& firstFreeRegion = freeRegionsList[0];

	// TODO(fkp): Code duplication (sorta)
	if ((u32) firstFreeRegion.address > (u32) newFreeRegion.address)
	{
		ASSERT((u32) newFreeRegion.address + newFreeRegion.size <= (u32) firstFreeRegion.address,
			   "Trying to free an already free region.");
		ASSERT(freeRegionsCount < MAX_NUMBER_OF_FREE_REGIONS, "Too many free regions.");

		memcpy(freeRegionsList + 1, freeRegionsList, freeRegionsCount * sizeof(FreeRegion));
		freeRegionsList[0] = newFreeRegion;
		freeRegionsCount += 1;
	}
	else
	{
		for (u32 i = 0; i < freeRegionsCount; i++)
		{
			FreeRegion& region = freeRegionsList[i];
			
			if (region.address < newFreeRegion.address)
			{
				ASSERT((u32) region.address + region.size <= (u32) newFreeRegion.address,
					   "Trying to free an already free region.");
				ASSERT(freeRegionsCount < MAX_NUMBER_OF_FREE_REGIONS, "Too many free regions.");
				
				memcpy(freeRegionsList + i + 2, freeRegionsList + i + 1,
					   (freeRegionsCount - i - 1) * sizeof(FreeRegion));
				freeRegionsList[i + 1] = newFreeRegion;
				freeRegionsCount += 1;

				break;
			}
		}
	}
}
