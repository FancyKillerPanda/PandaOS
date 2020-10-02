//  ===== Date Created: 14 September, 2020 ===== 

#include <stdio.h>
#include "utility/geometry.hpp"

bool calculate_geometry(DiskGeometry* geometry, usize hardDiskSize)
{
	constexpr usize maxCylinders = 16383;
	constexpr usize ideHeads = 16;
	constexpr usize ideSectors = 63;
	constexpr usize sectorSize = 512;

	if (hardDiskSize % sectorSize != 0)
	{
		printf("Error: Hard disk size (%lu) must be a multiple of the sector size (512B).\n", hardDiskSize);
		return false;
	}

	geometry->totalSectorsOnHardDisk = hardDiskSize / sectorSize;
	geometry->numberOfHeads = ideHeads;
	geometry->numberOfSectors = ideSectors;
	geometry->numberOfCylinders = geometry->totalSectorsOnHardDisk / (geometry->numberOfHeads * geometry->numberOfSectors);

	if (geometry->numberOfCylinders > maxCylinders)
	{
		geometry->numberOfCylinders = maxCylinders;
	}

	return true;
}
