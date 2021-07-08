//  ===== Date Created: 22 December, 2020 ===== 

#if !defined(VIRTUALALLOCATOR_HPP)
#define VIRTUALALLOCATOR_HPP

void init_virtual_allocator();

void map_page_address(void* virtualAddress, void* physicalAddress);
void unmap_page_address(void* virtualAddress);

void allocate_virtual_range(void* virtualAddress, usize size, void* physicalAddress = nullptr);

#endif
