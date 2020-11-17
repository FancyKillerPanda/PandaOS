//  ===== Date Created: 17 November, 2020 ===== 

#include "heapAllocator.hpp"
#include "virtualAllocator.hpp"

constexpr u32 HEAP_START = 0x400000;
// constexpr u32 HEAP_SIZE = 0x400000;
constexpr u32 HEAP_SIZE = 0x400000;

u32 nextToAllocate = HEAP_START;

void init_heap_allocator()
{
	allocate_virtual_range((void*) HEAP_START, HEAP_SIZE);
}

void* malloc(usize size)
{
	if (nextToAllocate > HEAP_START + HEAP_SIZE)
	{
		log_error("Next address to allocate is after the heap.");
		while (true);
	}

	if (nextToAllocate + size > HEAP_START + HEAP_SIZE)
	{
		log_error("Trying to allocate too much memory.");
		while (true);
	}

	void* allocation = (void*) nextToAllocate;
	nextToAllocate += size;

	return allocation;
}
