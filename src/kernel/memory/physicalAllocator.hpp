//  ===== Date Created: 05 October, 2020 ===== 

#if !defined(PHYSICALALLOCATOR_HPP)
#define PHYSICALALLOCATOR_HPP

#include "system.hpp"
#include "memoryMap.hpp"

constexpr u32 PAGE_SIZE = 4096;

void init_physical_allocator(MemoryMap* memoryMap);
u8* allocate_physical_page();
void free_physical_page(u8* pageFrame);

#endif
