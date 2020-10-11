//  ===== Date Created: 05 October, 2020 =====

#include "virtualAllocator.hpp"
#include "log.hpp"
#include "display.hpp"
#include "physicalAllocator.hpp"

constexpr u32 NUMBER_OF_PAGE_DIRECTORY_ENTRIES = 1024;
constexpr u32 NUMBER_OF_PAGE_TABLE_ENTRIES = 1024;
constexpr u32 KERNEL_FLAT_ADDRESS = 0x20000;
u32* pageDirectory = nullptr;

extern "C" void load_page_directory(u32* pageDirectory);
extern "C" void enable_paging();

void init_virtual_allocator()
{
	constexpr u32 presentFlag = 0x01;
	constexpr u32 readWriteFlag = 0x02;
	
	// Initialises an unmapped page directory
	pageDirectory = (u32*) allocate_physical_page();

	for (u32 i = 0; i < NUMBER_OF_PAGE_DIRECTORY_ENTRIES; i++)
	{
		pageDirectory[i] = readWriteFlag;
	}
	
	// Initialises the first page table
	u32* firstPageTable = (u32*) allocate_physical_page();

	for (u32 i = 0; i < NUMBER_OF_PAGE_TABLE_ENTRIES; i++)
	{
		firstPageTable[i] = (i * PAGE_SIZE) | presentFlag | readWriteFlag;
	}

	// Puts the first page table into the page directory and tells the
	// processor to load it
	pageDirectory[0] = ((u32) firstPageTable) | presentFlag | readWriteFlag;
	load_page_directory(pageDirectory);
	enable_paging();
	
	log_info("Initialised virtual page allocator.");
}
