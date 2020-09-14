//  ===== Date Created: 14 September, 2020 ===== 

#if !defined(GEOMETRY_HPP)
#define GEOMETRY_HPP

#include "utility.hpp"

struct DiskGeometry
{
	usize totalSectorsOnHardDisk = 0;
	usize numberOfCylinders = 0;
	usize numberOfHeads = 0;
	usize numberOfSectors = 0;
};

bool calculate_geometry(DiskGeometry* geometry, usize hardDiskSize);

#endif
