//  ===== Date Created: 05 October, 2020 =====

#include "virtualAllocator.hpp"
#include "log.hpp"
#include "display.hpp"
#include "physicalAllocator.hpp"

using PageTableEntry = u32;
using PageTable = PageTableEntry*;
using PageDirectoryEntry = u32;
using PageDirectoryTable = PageDirectoryEntry*;

PageDirectoryTable pageDirectoryTable = nullptr;

constexpr u32 NUMBER_OF_PAGE_DIRECTORY_ENTRIES = 1024;
constexpr u32 NUMBER_OF_PAGE_TABLE_ENTRIES = 1024;
constexpr u32 KERNEL_FLAT_ADDRESS = 0x20000;

constexpr u32 PRESENT_FLAG = 0x01;
constexpr u32 READ_WRITE_FLAG = 0x02;

extern "C" void load_page_directory(PageDirectoryTable pageDirectoryTable);
extern "C" void enable_paging();

void init_virtual_allocator()
{
	// Initialises an unmapped page directory
	pageDirectoryTable = (PageDirectoryTable) allocate_physical_page();

	for (u32 i = 0; i < NUMBER_OF_PAGE_DIRECTORY_ENTRIES; i++)
	{
		pageDirectoryTable[i] = (PageDirectoryEntry) READ_WRITE_FLAG;
	}
	
	// Initialises the first page table
	PageTable firstPageTable = (PageTable) allocate_physical_page();

	for (u32 i = 0; i < NUMBER_OF_PAGE_TABLE_ENTRIES; i++)
	{
		firstPageTable[i] = (PageTableEntry) ((i * PAGE_SIZE) | PRESENT_FLAG | READ_WRITE_FLAG);
	}

	// Puts the first page table into the page directory and tells the
	// processor to load it
	pageDirectoryTable[0] = (PageDirectoryEntry) (((u32) firstPageTable) | PRESENT_FLAG | READ_WRITE_FLAG);
	load_page_directory(pageDirectoryTable);
	enable_paging();
	
	log_info("Initialised virtual page allocator.");
}

PageDirectoryEntry& get_page_directory_entry(PageDirectoryTable& pageDirectoryTable, void* virtualAddress)
{
	u32 index = (((u32) virtualAddress) & 0xffc00000) >> 22;
	return pageDirectoryTable[index];
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
		pageTable = (PageTable) (((u32) pageDirectoryEntry) & 0xfffff000);
	}
	else
	{
		pageTable = (PageTable) allocate_physical_page();
		pageDirectoryEntry = (PageDirectoryEntry) (((u32) pageTable) | PRESENT_FLAG | READ_WRITE_FLAG);
	}

	return pageTable;
}

void map_address(void* virtualAddress, void* physicalAddress)
{
	// TODO(fkp): Make sure addressses are page aligned
	PageDirectoryEntry& pageDirectoryEntry = get_page_directory_entry(pageDirectoryTable, virtualAddress);
	PageTable pageTable = get_page_table(pageDirectoryEntry);
	PageTableEntry& pageTableEntry = get_page_table_entry(pageTable, virtualAddress);

	if (pageTableEntry & PRESENT_FLAG)
	{
		log_warning("Address is already mapped, not overwriting.");
		return;
	}

	pageTableEntry = (PageTableEntry) (((u32) physicalAddress) | PRESENT_FLAG | READ_WRITE_FLAG);
	
	log_info("Mapped virtual address %x to physical address %x.", virtualAddress, physicalAddress);
}
