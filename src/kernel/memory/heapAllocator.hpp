//  ===== Date Created: 17 November, 2020 ===== 

#if !defined(HEAPALLOCATOR_HPP)
#define HEAPALLOCATOR_HPP

#include "system/system.hpp"

void init_heap_allocator();
void* malloc(usize size);
void free(void* pointer);

#endif
