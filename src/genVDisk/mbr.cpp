//  ===== Date Created: 20 September, 2020 ===== 

#include "mbr.hpp"

bool init_mbr(MBR* mbr, const u8* path)
{
	if (!read_entire_file(path, &mbr->data, &mbr->size))
	{
		return false;
	}

	if (mbr->size != 512 ||
		mbr->data[510] != 0x55 || mbr->data[511] != 0xaa)
	{
		printf("Error: MBR file ('%s') is invalid.\n", path);
		return false;
	}

	return true;
}

bool serialise_partition(u8* into, const DiskGeometry* geometry,
						 usize numberOfSectors, usize lbaBaseOffset,
						 PartitionType type, PartitionStatus status)
{
	switch (type)
	{
	case PartitionType::Fat16Chs:
	{
		CHS chsBegin = convert_lba_to_chs(lbaBaseOffset, geometry);
		CHS chsEnd = convert_lba_to_chs(lbaBaseOffset + numberOfSectors, geometry);

		if (chsBegin.cylinder > ((1 << 10) - 1) || chsEnd.head > ((1 << 10) - 1))
		{
			printf("Error: CHS cylinder cannot use more than 10 bits.\n");
			return false;
		}

		if (chsBegin.head > ((1 << 8) - 1) || chsEnd.head > ((1 << 8) - 1))
		{
			printf("Error: CHS head cannot use more than 8 bits.\n");
			return false;
		}

		if (chsBegin.head > ((1 << 6) - 1) || chsEnd.head > ((1 << 6) - 1))
		{
			printf("Error: CHS sector cannot use more than 6 bits.\n");
			return false;
		}

		// Bits 9 and 10 of the cylinder go in bits 7 and 8 of the sector
		u8 sectorWithCylinderBitsBegin = (u8) chsBegin.sector;
		sectorWithCylinderBitsBegin |= (chsBegin.cylinder & 0x300) >> 2;
		u8 sectorWithCylinderBitsEnd = (u8) chsEnd.sector;
		sectorWithCylinderBitsEnd |= (chsEnd.cylinder & 0x300) >> 2;

		u8 cylinderBegin = (u8) (chsBegin.cylinder & 0xff);
		u8 cylinderEnd = (u8) (chsEnd.cylinder & 0xff);

		into[1] = (u8) chsBegin.head;
		into[2] = sectorWithCylinderBitsBegin;
		into[3] = cylinderBegin;
		into[5] = (u8) chsEnd.head;
		into[6] = sectorWithCylinderBitsEnd;
		into[7] = cylinderEnd;		
	} break;

	default:
	{
	} break;
	}

	into[0] = (u8) status;
	into[4] = (u8) type;
	*((u32*) &into[8]) = lbaBaseOffset;
	*((u32*) &into[12]) = numberOfSectors;

	return true;
}

bool add_partition_to_mbr(MBR* mbr, const DiskGeometry* geometry, usize numberOfSectors)
{
	usize partitionOffset = partitionBaseStart + (mbr->partitionNumber * partitionEntrySize);
	usize partitionLBAOffset = mbr->lbaOffset;
	mbr->lbaOffset += numberOfSectors;
	mbr->partitionNumber += 1;

	return serialise_partition(mbr->data + partitionOffset, geometry,
							   numberOfSectors, partitionLBAOffset,
							   PartitionType::Fat16Chs, PartitionStatus::Bootable);
}
	
CHS convert_lba_to_chs(usize lba, const DiskGeometry* geometry)
{
	CHS chs;
	chs.head = (lba / geometry->numberOfSectors) % geometry->numberOfHeads;
	chs.cylinder = (lba / geometry->numberOfSectors) / geometry->numberOfHeads;
	chs.sector = (lba % geometry->numberOfSectors) + 1;

	return chs;
}
