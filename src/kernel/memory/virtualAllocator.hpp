//  ===== Date Created: 22 December, 2020 ===== 

#if !defined(VIRTUALALLOCATOR_HPP)
#define VIRTUALALLOCATOR_HPP

namespace PageFlag
{

enum
{
	Present = 0x01,
	ReadWrite = 0x02,
	User = 0x04,
	Uncacheable = 0x10,
};

};

void init_virtual_allocator();

void map_page_address(void* virtualAddress, void* physicalAddress, u32 extraFlags = 0);
void unmap_page_address(void* virtualAddress);

// If the virtual or physical address is nullptr, it will be chosen to
// be *something*.
// Returns the virtual address.
void* allocate_virtual_range(usize size, void* virtualAddress = nullptr, void* physicalAddress = nullptr, u32 extraFlags = 0);

// Returns the physical address that
// this virtual address is mapped to.
void* get_mapping(void* virtualAddress);

#endif
