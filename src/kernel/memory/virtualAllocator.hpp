//  ===== Date Created: 05 October, 2020 ===== 

#if !defined(VIRTUALALLOCATOR_HPP)
#define VIRTUALALLOCATOR_HPP

void init_virtual_allocator();
void allocate_virtual_range(void* start, u32 length);

#endif