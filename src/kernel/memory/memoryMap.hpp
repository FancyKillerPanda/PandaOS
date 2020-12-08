//  ===== Date Created: 08 December, 2020 ===== 

#if !defined(MEMORYMAP_HPP)
#define MEMORYMAP_HPP

#include "system/common.hpp"

enum class MemoryType : u32
{
	Free = 1,
	Reserved = 2,
	Reclaimable = 3,
	NonVolatile = 4,
	Bad = 5,
	Unknown = 6,

	Count,
};

// TODO(fkp): Packed struct?
struct MemoryMapEntry
{
	u64 baseAddress = 0;
	u64 regionLength = 0;
	MemoryType regionType = MemoryType::Reserved;
	u32 extendedAttributes = 0; // NOTE(fkp): May not exist
};
static_assert(sizeof(MemoryMapEntry) == 24, "MemoryMapEntry must be 24 bytes.");

struct MemoryMap
{
	MemoryMapEntry* entries = nullptr;
	u32 numberOfEntries = 0;
};

void read_memory_map(MemoryMap* memoryMap);

#endif
