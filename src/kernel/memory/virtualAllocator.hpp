//  ===== Date Created: 22 December, 2020 ===== 

#if !defined(VIRTUALALLOCATOR_HPP)
#define VIRTUALALLOCATOR_HPP

void init_virtual_allocator();

void map_page_address(void* virtualAddress, void* physicalAddress);
void unmap_page_address(void* virtualAddress);

// If the virtual or physical address is nullptr, it will be chosen to
// be *something*.
// Returns the virtual address.
void* allocate_virtual_range(usize size, void* virtualAddress = nullptr, void* physicalAddress = nullptr);

// Returns the physical address that
// this virtual address is mapped to.
void* get_mapping(void* virtualAddress);

#endif
