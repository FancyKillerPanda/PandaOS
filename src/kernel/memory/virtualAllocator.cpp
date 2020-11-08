//  ===== Date Created: 05 October, 2020 =====

#include "virtualAllocator.hpp"
#include "log.hpp"
#include "display.hpp"
#include "physicalAllocator.hpp"

using PageTableEntry = u32;
using PageTable = PageTableEntry*;
using PageDirectory = PageTable*;

constexpr u32 NUMBER_OF_PAGE_DIRECTORY_ENTRIES = 1024;
constexpr u32 NUMBER_OF_PAGE_TABLE_ENTRIES = 1024;
constexpr u32 KERNEL_FLAT_ADDRESS = 0x20000;
PageDirectory pageDirectory = nullptr;

constexpr u32 PRESENT_FLAG = 0x01;
constexpr u32 READ_WRITE_FLAG = 0x02;

extern "C" void load_page_directory(PageDirectory pageDirectory);
extern "C" void enable_paging();

void init_virtual_allocator()
{
	// Initialises an unmapped page directory
	pageDirectory = (PageDirectory) allocate_physical_page();

	for (u32 i = 0; i < NUMBER_OF_PAGE_DIRECTORY_ENTRIES; i++)
	{
		pageDirectory[i] = (PageTable) READ_WRITE_FLAG;
	}
	
	// Initialises the first page table
	PageTable firstPageTable = (PageTable) allocate_physical_page();

	for (u32 i = 0; i < NUMBER_OF_PAGE_TABLE_ENTRIES; i++)
	{
		firstPageTable[i] = (i * PAGE_SIZE) | PRESENT_FLAG | READ_WRITE_FLAG;
	}

	// Puts the first page table into the page directory and tells the
	// processor to load it
	pageDirectory[0] = (PageTable) (((u32) firstPageTable) | PRESENT_FLAG | READ_WRITE_FLAG);
	load_page_directory(pageDirectory);
	enable_paging();
	
	log_info("Initialised virtual page allocator.");
}
