//  ===== Date Created: 20 September, 2020 ===== 

#if !defined(MBR_HPP)
#define MBR_HPP

#include "utility/utility.hpp"
#include "utility/geometry.hpp"

enum class PartitionType : u8
{
	Empty = 0x00,
	Fat16Chs = 0x06,
	Fat32Lba = 0x0c,
};

enum class PartitionStatus : u8
{
	Inactive = 0x00,
	Invalid = 0x7f,
	Bootable = 0x80,
};

struct MBR
{
	u8* data = nullptr;
	usize size = 0;
	usize partitionNumber = 0;
	usize lbaOffset = 1;
};

bool init_mbr(MBR* mbr, const u8* path);
bool add_partition_to_mbr(MBR* mbr, const DiskGeometry* geometry, usize numberOfSectors, PartitionType type, bool bootable = false);

struct CHS
{
	usize cylinder = 0;
	usize head = 0;
	usize sector = 0;
};

CHS convert_lba_to_chs(usize lba, const DiskGeometry* geometry);

constexpr usize partitionBaseStart = 446;
constexpr usize partitionEntrySize = 16;

#endif
