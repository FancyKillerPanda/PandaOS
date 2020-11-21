//  ===== Date Created: 17 November, 2020 ===== 

#include "heapAllocator.hpp"
#include "virtualAllocator.hpp"

constexpr u32 HEAP_START = 0x500000;
// TODO(fkp): This causes a page fault
// constexpr u32 HEAP_SIZE = 0x400000;
constexpr u32 HEAP_SIZE = 0x100000;
constexpr u32 FREE_REGIONS_LIST_ADDRESS = 0x400000;

struct FreeRegion
{
	void* address = nullptr;
	usize size = 0;
};

// Because we allocate a page for the free regions list
constexpr u32 MAX_NUMBER_OF_FREE_REGIONS = PAGE_SIZE / sizeof(FreeRegion);

FreeRegion* freeRegionsList = nullptr;
u32 freeRegionsCount = 0;

struct AllocationInfoBlock
{
	usize size;
};

void init_heap_allocator()
{
	// Allocates space for the list of free regions
	allocate_virtual_range((void*) FREE_REGIONS_LIST_ADDRESS, PAGE_SIZE);
	freeRegionsList = (FreeRegion*) FREE_REGIONS_LIST_ADDRESS;
	memset(freeRegionsList, 0, PAGE_SIZE);
	
	// Allocates the heap block
	allocate_virtual_range((void*) HEAP_START, HEAP_SIZE);

	// Adds the heap block as the (currently) only free region
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

		if (region.size >= allocationSize)
		{
			void* allocation = region.address;
			*((AllocationInfoBlock*) allocation) = AllocationInfoBlock { allocationSize };
			
			region.address = (void*) (((u8*) region.address) + allocationSize);
			region.size -= allocationSize;
			
			if (region.size == 0)
			{
				// The free region block was exactly the right size
				freeRegionsCount -= 1;
				memcpy(freeRegionsList + i, freeRegionsList + i + 1, (freeRegionsCount - i) * sizeof(FreeRegion));
			}
			
			return (void*) ((AllocationInfoBlock*) allocation + 1);
		}
	}

	log_error("Unable to allocate region of length %d bytes.", size);
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

	// TODO(fkp): Combining adjacent free regions
	AllocationInfoBlock& infoBlock = *(((AllocationInfoBlock*) pointer) - 1);
	FreeRegion newFreeRegion = { &infoBlock, infoBlock.size };
	FreeRegion& firstFreeRegion = freeRegionsList[0];

	ASSERT(freeRegionsCount >= 1);
	
	// TODO(fkp): Code duplication. Might be able to use an
	// i == -1 in the loop.
	if ((u32) firstFreeRegion.address > (u32) newFreeRegion.address)
	{
		ASSERT((u32) newFreeRegion.address + newFreeRegion.size <= (u32) firstFreeRegion.address);
		ASSERT(freeRegionsCount < MAX_NUMBER_OF_FREE_REGIONS);
		
		memcpy(freeRegionsList + 1, freeRegionsList, freeRegionsCount);
		firstFreeRegion = newFreeRegion;
		freeRegionsCount += 1;
	}
	else
	{
		for (u32 i = 0; i < freeRegionsCount; i++)
		{
			FreeRegion& region = freeRegionsList[i];
			
			if (region.address < newFreeRegion.address)
			{
				ASSERT((u32) region.address + region.size <= (u32) newFreeRegion.address);
				ASSERT(freeRegionsCount < MAX_NUMBER_OF_FREE_REGIONS);
				
				memcpy(freeRegionsList + i + 2, freeRegionsList + i + 1, (freeRegionsCount - i - 1) * sizeof(FreeRegion));
				freeRegionsList[i + 1] = newFreeRegion;
				freeRegionsCount += 1;

				break;
			}
		}
	}
}
