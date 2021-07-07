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
