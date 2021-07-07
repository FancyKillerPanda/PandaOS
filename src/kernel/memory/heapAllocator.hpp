//  ===== Date Created: 07 July, 2021 ===== 

#if !defined(HEAPALLOCATOR_HPP)
#define HEAPALLOCATOR_HPP

#include "system/common.hpp"

void init_heap_allocator();

void* malloc(usize size);
void* calloc(usize count, usize size);
void free(void* pointer);

#endif
