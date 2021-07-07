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

constexpr u32 NUMBER_OF_PAGE_DIRECTORY_ENTRIES = 1024;
constexpr u32 NUMBER_OF_PAGE_TABLE_ENTRIES = 1024;

constexpr u32 PRESENT = 0x01;
constexpr u32 READ_WRITE = 0x02;

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

bool pageFault = false;

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
//	void* virtualPageAddress = (void*) (registerState.cr2 & 0xfffff000);
	void* virtualPageAddress = (void*) registerState.cr2;

	pageFault = true;
	printf("PAGE FAULT: %x\n", (u32) virtualPageAddress);
	
	map_page_address(virtualPageAddress, physicalPageAddress);
	printf("Handled page fault\n");
}

u32 get_page_directory_index(void* virtualAddress)
{
	return (((u32) virtualAddress) & 0xffc00000) >> 22;
}

PageDirectoryEntry& get_page_directory_entry(PageDirectory& pageDirectory, void* virtualAddress)
{
	u32 index = get_page_directory_index(virtualAddress);
	return pageDirectory[index];
}

u32 get_page_table_index(void* virtualAddress)
{
	return (((u32) virtualAddress) & 0x003ff000) >> 12;
}

PageTableEntry& get_page_table_entry(PageTable& pageTable, void* virtualAddress)
{	
	u32 index = get_page_table_index(virtualAddress);
	return pageTable[index];
}

PageTable get_page_table(PageDirectoryEntry& pageDirectoryEntry, void* virtualAddress)
{
	PageTable pageTable = nullptr;
//	void* virtualAddress = (void*) (pageDirectoryEntry & 0xfffff000);
//	printf("    HERE %x     ", (u32) virtualAddress);
	// TODO(fkp): Name change
	u32 pageTableIndex = get_page_directory_index(virtualAddress);

	if (pageDirectoryEntry & PRESENT)
	{
		pageTable = (PageTable) (0xffc00000 + (pageTableIndex * 0x1000));
	}
	else
	{
		if (pageFault) { printf("%x %x ", &pageDirectoryEntry, pageTableIndex); }
	
		pageTable = (PageTable) allocate_physical_page();
		// BREAK_POINT();
		pageDirectoryEntry = (u32) pageTable | PRESENT | READ_WRITE;
		
		// Accesses the page table through recursive paging
		pageTable = (PageTable) (0xffc00000 + (pageTableIndex * 0x1000));

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
//	ASSERT(((u32) virtualAddress & 0xfff) == 0, "Virtual address is not page-aligned.");
//	ASSERT(((u32) physicalAddress & 0xfff) == 0, "Physical address is not page-aligned.");

	if (pageFault) { printf("1 "); }
	
	// Uses the address to get the relevant entries
	PageDirectoryEntry& pageDirectoryEntry = get_page_directory_entry(pageDirectory, virtualAddress);
	BREAK_POINT();
	PageTable pageTable = get_page_table(pageDirectoryEntry, virtualAddress);
	PageTableEntry& pageTableEntry = get_page_table_entry(pageTable, virtualAddress);

	if (map)
	{
		// Checks if this page is already mapped
		if (pageTableEntry & PRESENT)
		{
			log_warning("Address %10x is already mapped, not overwriting.", virtualAddress);
			return;
		}

		pageTableEntry = (u32) physicalAddress | PRESENT | READ_WRITE;
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

	BREAK_POINT();
}

void map_page_address(void* virtualAddress, void* physicalAddress)
{
	internal_map_unmap_page(virtualAddress, physicalAddress, true);
}

void unmap_page_address(void* virtualAddress)
{
	internal_map_unmap_page(virtualAddress, nullptr, false);
}
