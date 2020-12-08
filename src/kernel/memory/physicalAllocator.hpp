//  ===== Date Created: 08 December, 2020 ===== 

#if !defined(PHYSICALALLOCATOR_HPP)
#define PHYSICALALLOCATOR_HPP

#include "system/common.hpp"

constexpr u32 PAGE_SIZE = 4096;

void init_physical_allocator(MemoryMap* memoryMap);
void* allocate_physical_page();
void free_physical_page(void* pageFrame);

#endif
