#!/bin/bash
#  ===== Date Created: 14 September, 2020 ===== 

source ./buildCommon.sh

exit_on_error()
{
	print $RED "genVDisk build failed!\n";
	popd
	exit 1
}

# Directories
srcDir=$prjRoot/src/genVDisk/

# Flags
compileFlags="-o genVDisk -I $srcDir"
linkFlags=""
files="$srcDir/genVDisk.cpp"

mkdir -p $binDir/genVDisk
pushd $binDir/genVDisk > /dev/null

print $BLUE "Building genVDisk...";
clang++ $compileFlags $linkFlags $files || exit_on_error
print $GREEN "genVDisk build succeeded!\n"

popd > /dev/null
exit 0
