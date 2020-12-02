#!/bin/bash
#  ===== Date Created: 02 December, 2020 ===== 

scriptDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" > /dev/null && pwd )"
source $scriptDir/buildCommon.sh

# Exits the script because of an error
exit_on_error()
{
	print $RED "\ngenVDisk build failed!\n"
	cd $originalDir
	exit 1
}

# Directories
genVDiskDir=$srcDir/genVDisk

# Flags
compileFlags="-o genVDisk -I $srcDir -funsigned-char -O3 -Wall -Wextra"
linkFlags=""
files="$genVDiskDir/unityBuild.cpp"

# Build
mkdir -p $binDir/genVDisk
cd $binDir/genVDisk > /dev/null

print $BLUE "Cleaning genVDisk..."
rm genVDisk 2> /dev/null
print $BLUE "\nBuilding...";
clang++ $compileFlags $linkFlags $files || exit_on_error
print $GREEN "\ngenVDisk build succeeded!\n"

# Exit
cd $originalDir
exit 0
