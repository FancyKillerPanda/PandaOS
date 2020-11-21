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
	usize length = 0;
};

FreeRegion* freeRegionsList = nullptr;
u32 freeRegionsCount = 0;

struct MallocInfoBlock
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
		usize allocationSize = size + sizeof(MallocInfoBlock);

		// TODO(fkp): Handle if region is exactly the right size
		if (region.length > allocationSize)
		{
			void* allocation = region.address;
			*((MallocInfoBlock*) allocation) = MallocInfoBlock { size };
			
			region.address = (void*) (((u8*) region.address) + allocationSize);
			region.length -= allocationSize;
			
			return (void*) ((MallocInfoBlock*) allocation + 1);
		}
	}

	log_error("Unable to allocate region of length %d bytes.", size);
	while (true);
}
