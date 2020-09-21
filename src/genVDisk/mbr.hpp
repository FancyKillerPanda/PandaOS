//  ===== Date Created: 20 September, 2020 ===== 

#if !defined(MBR_HPP)
#define MBR_HPP

#include "utility.hpp"
#include "geometry.hpp"

enum PartitionType
{
	PARTITION_EMPTY = 0x00,
	PARTITION_FAT16_CHS = 0x06,
};

enum PartitionStatus
{
	PARTITION_INACTIVE = 0x00,
	PARTITION_INVALID = 0x7f,
	PARTITION_BOOTABLE = 0x80,
};

struct MBR
{
	u8* data = nullptr;
	usize size = 0;
	usize partitionNumber = 0;
	usize lbaOffset = 1;
};

bool init_mbr(MBR* mbr, const u8* path);
bool add_partition_to_mbr(MBR* mbr, const DiskGeometry* geometry, usize numberOfSectors);

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
