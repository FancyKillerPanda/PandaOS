//  ===== Date Created: 22 December, 2020 ===== 

#include "memory/physicalAllocator.hpp"
#include "memory/virtualAllocator.hpp"
#include "system/common.hpp"

using PageTableEntry = u32;
using PageTable = PageTableEntry*;
using PageDirectoryEntry = u32;
using PageDirectoryTable = PageDirectoryEntry*;

// We create this in the bootloader
PageDirectoryTable pageDirectoryTable = (PageDirectoryTable) 0x2000;

constexpr u32 KERNEL_PHYSICAL_ADDRESS = 0x00100000;
constexpr u32 KERNEL_VIRTUAL_ADDRESS = 0xc0100000;
constexpr u32 KERNEL_SIZE = 0x40000000;

constexpr u32 NUMBER_OF_PAGE_DIRECTORY_ENTRIES = 1024;
constexpr u32 NUMBER_OF_PAGE_TABLE_ENTRIES = 1024;

constexpr u32 PRESENT_FLAG = 0x01;
constexpr u32 READ_WRITE_FLAG = 0x02;

void init_virtual_allocator()
{
	// TODO(fkp): Eventually the page directory table should not
	// be a single global, but there should be one for each process.
	
	// Sets up recursive paging. This works because the identity
	// mapping of the first page table still exists at this point.
	PageDirectoryEntry& lastPageDirectoryEntry = pageDirectoryTable[NUMBER_OF_PAGE_DIRECTORY_ENTRIES - 1];
	lastPageDirectoryEntry = (PageDirectoryEntry) ((u32) pageDirectoryTable | PRESENT_FLAG | READ_WRITE_FLAG);
	
	// Removes the identity mapping. We don't need to deallocate
	// any memory since it's just using bootloader memory.
	pageDirectoryTable[0] = READ_WRITE_FLAG;
	
	log_info("Initialised virtual page allocator.");
}

PageDirectoryEntry& get_page_directory_entry(PageDirectoryTable& pageDirectory, void* virtualAddress)
{
	u32 index = (((u32) virtualAddress) & 0xffc00000) >> 22;
	return pageDirectory[index];
}

PageTableEntry& get_page_table_entry(PageTable& pageTable, void* virtualAddress)
{	
	u32 index = (((u32) virtualAddress) & 0x003ff000) >> 12;
	return pageTable[index];
}

PageTable get_page_table(PageDirectoryEntry& pageDirectoryEntry)
{
	PageTable pageTable = nullptr;

	if (pageDirectoryEntry & PRESENT_FLAG)
	{
		pageTable = (PageTable) ((u32) pageDirectoryEntry & 0xfffff000);
	}
	else
	{
		pageTable = (PageTable) allocate_physical_page();

		for (u32 i = 0; i < NUMBER_OF_PAGE_TABLE_ENTRIES; i++)
		{
			pageTable[i] = (PageTableEntry) READ_WRITE_FLAG;
		}
		
		pageDirectoryEntry = (PageDirectoryEntry) ((u32) pageTable | PRESENT_FLAG | READ_WRITE_FLAG);
	}

	return pageTable;
}

void internal_map_unmap_page(void* virtualAddress, void* physicalAddress, bool map)
{
	// Ensures the addresses are page-aligned
	ASSERT(((u32) virtualAddress & 0xfff) == 0, "Virtual address is not page-aligned.");
	ASSERT(((u32) physicalAddress & 0xfff) == 0, "Physical address is not page-aligned.");

	// Uses the address to get the relevant entries
	PageDirectoryEntry& pageDirectoryEntry = get_page_directory_entry(pageDirectoryTable, virtualAddress);
	PageTable pageTable = get_page_table(pageDirectoryEntry);
	PageTableEntry& pageTableEntry = get_page_table_entry(pageTable, virtualAddress);

	if (map)
	{
		// Checks if this page is already mapped
		if (pageTableEntry & PRESENT_FLAG)
		{
			log_warning("Address %10x is already mapped, not overwriting.", virtualAddress);
			return;
		}

		pageTableEntry = (PageTableEntry) ((u32) physicalAddress | PRESENT_FLAG | READ_WRITE_FLAG);
	}
	else
	{
		// Checks if the page isn't actually mapped
		if (!(pageTableEntry & PRESENT_FLAG))
		{
			log_warning("Trying to unmap address %10x, which is not mapped.", virtualAddress);
			return;
		}

		pageTableEntry = (PageTableEntry) READ_WRITE_FLAG;
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
