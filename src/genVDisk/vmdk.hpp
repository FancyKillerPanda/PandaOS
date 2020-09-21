//  ===== Date Created: 18 September, 2020 ===== 

#if !defined(VMDK_HPP)
#define VMDK_HPP

#include "utility.hpp"
#include "clargs.hpp"
#include "geometry.hpp"

bool write_descriptor_file(const u8* descriptorFileName, const char* extentFileName, usize hardDiskSize, const DiskGeometry* diskGeometry);
bool write_extent_file(const u8* extentFileName, const CLArgs& arguments, const DiskGeometry* diskGeometry);

#endif
