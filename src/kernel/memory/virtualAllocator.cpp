//  ===== Date Created: 22 December, 2020 ===== 

#include "memory/physicalAllocator.hpp"
#include "memory/virtualAllocator.hpp"
#include "system/common.hpp"

using PageTableEntry = u32;
using PageTable = PageTableEntry*;
using PageDirectoryEntry = u32;
using PageDirectory = PageDirectoryEntry*;

// We create this in the bootloader
PageDirectory pageDirectory = (PageDirectory) 0x2000;

constexpr usize KERNEL_GENERAL_PURPOSE_MEMORY = 0xf0000000;

constexpr u32 NUMBER_OF_PAGE_DIRECTORY_ENTRIES = 1024;
constexpr u32 NUMBER_OF_PAGE_TABLE_ENTRIES = 1024;

constexpr u32 PRESENT = 0x01;
constexpr u32 READ_WRITE = 0x02;
constexpr u32 USER = 0x04;

void init_virtual_allocator()
{
	// TODO(fkp): Eventually the page directory table should not
	// be a single global, but there should be one for each process.
	
	// Sets up recursive paging. This works because the identity
	// mapping of the first page table still exists at this point.
	PageDirectoryEntry& lastPageDirectoryEntry = pageDirectory[NUMBER_OF_PAGE_DIRECTORY_ENTRIES - 1];
	lastPageDirectoryEntry = (u32) pageDirectory | PRESENT | READ_WRITE;
	pageDirectory = (PageDirectory) 0xfffff000;
	
	// Removes the identity mapping. We don't need to deallocate
	// any memory since it's just using bootloader memory.
	pageDirectory[0] = READ_WRITE;

	log_info("Initialised virtual page allocator.");
}

// NOTE(fkp): Declared in handleExceptions.hpp
INTERRUPT_FUNCTION
void handle_page_fault_exception(InterruptFrame* frame, u32 errorCode)
{
	UNUSED(frame);
	UNUSED(errorCode);

	// The address that was accessed is in cr2
	RegisterState registerState;
	get_registers(&registerState);

	void* physicalPageAddress = allocate_physical_page();
	void* virtualPageAddress = (void*) (registerState.cr2 & 0xfffff000);

	map_page_address(virtualPageAddress, physicalPageAddress);
}

u32 get_index_into_page_directory(void* virtualAddress)
{
	return (((u32) virtualAddress) & 0xffc00000) >> 22;
}

PageDirectoryEntry& get_page_directory_entry(PageDirectory& pageDirectory, void* virtualAddress)
{
	u32 index = get_index_into_page_directory(virtualAddress);
	return pageDirectory[index];
}

u32 get_index_into_page_table(void* virtualAddress)
{
	return (((u32) virtualAddress) & 0x003ff000) >> 12;
}

PageTableEntry& get_page_table_entry(PageTable& pageTable, void* virtualAddress)
{	
	u32 index = get_index_into_page_table(virtualAddress);
	return pageTable[index];
}

PageTable get_page_table(u32 indexIntoPageDirectory)
{
	PageTable pageTable = nullptr;
	PageDirectoryEntry& pageDirectoryEntry = pageDirectory[indexIntoPageDirectory];

	if (pageDirectoryEntry & PRESENT)
	{
		pageTable = (PageTable) (0xffc00000 + (indexIntoPageDirectory * 0x1000));
	}
	else
	{
		pageTable = (PageTable) allocate_physical_page();
		pageDirectoryEntry = (u32) pageTable | PRESENT | READ_WRITE | USER;
		
		// Accesses the page table through recursive paging
		pageTable = (PageTable) (0xffc00000 + (indexIntoPageDirectory * 0x1000));

		for (u32 i = 0; i < NUMBER_OF_PAGE_TABLE_ENTRIES; i++)
		{
			pageTable[i] = READ_WRITE;
		}
	}

	return pageTable;
}

void internal_map_unmap_page(void* virtualAddress, void* physicalAddress, bool map)
{
	// Ensures the addresses are page-aligned
	ASSERT(((u32) virtualAddress & 0xfff) == 0, "Virtual address is not page-aligned.");
	ASSERT(((u32) physicalAddress & 0xfff) == 0, "Physical address is not page-aligned.");

	// Uses the address to get the relevant entries
	u32 indexIntoPageDirectory = get_index_into_page_directory(virtualAddress);
	PageTable pageTable = get_page_table(indexIntoPageDirectory);
	PageTableEntry& pageTableEntry = get_page_table_entry(pageTable, virtualAddress);

	if (map)
	{
		// Checks if this page is already mapped
		if (pageTableEntry & PRESENT)
		{
			log_warning("Address %10x is already mapped, not overwriting.", virtualAddress);
			return;
		}

		pageTableEntry = (u32) physicalAddress | PRESENT | READ_WRITE | USER;
	}
	else
	{
		// Checks if the page isn't actually mapped
		if (!(pageTableEntry & PRESENT))
		{
			log_warning("Trying to unmap address %10x, which is not mapped.", virtualAddress);
			return;
		}

		pageTableEntry = READ_WRITE;
	}
}

void map_page_address(void* virtualAddress, void* physicalAddress)
{
	internal_map_unmap_page(virtualAddress, physicalAddress, true);
}

void unmap_page_address(void* virtualAddress)
{
	internal_map_unmap_page(virtualAddress, nullptr, false);
}

// TODO(fkp): We should maybe have this keep track of the physical
// addresses it has mapped to, so if we get another request for the
// same page we can just return that pointer.
void* map_to_physical(void* physicalAddress, usize numberOfPages)
{
	static usize generalPurposeMemory = KERNEL_GENERAL_PURPOSE_MEMORY;
	void* virtualAddress = (void*) generalPurposeMemory;
	
	allocate_virtual_range(virtualAddress, numberOfPages * PAGE_SIZE, physicalAddress);
	generalPurposeMemory += numberOfPages * PAGE_SIZE;

	return virtualAddress;
}

void allocate_virtual_range(void* virtualAddress, usize size, void* physicalAddress)
{
	if ((u32) virtualAddress & 0xfff)
	{
		log_warning("Start of virtual range must be page aligned for allocation.");
		return;
	}

	u32 numberOfPages = ((size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1)) / PAGE_SIZE;

	for (u32 i = 0; i < numberOfPages; i++)
	{
		void* page;

		if (physicalAddress == nullptr)
		{
			page = allocate_physical_page();
		}
		else
		{
			page = (u8*) physicalAddress + (i * PAGE_SIZE);
		}
		
		map_page_address((u8*) virtualAddress + (i * PAGE_SIZE), page);
	}
}
