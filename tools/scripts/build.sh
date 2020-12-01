#!/bin/bash
#  ===== Date Created: 01 December, 2020 ===== 

scriptDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" > /dev/null && pwd )"
source $scriptDir/buildCommon.sh

# Exits the script because of an error
exit_on_error()
{
	print $RED "\nBuild failed!\n"
	cd $originalDir
	exit 1
}

# Directories
srcDir=$prjDir/src

# Build
mkdir -p $binDir/PandaOS
cd $binDir/PandaOS > /dev/null

print $BLUE "Cleaning..."
rm *.bin *.img *.iso *.o *.vmdk 2> /dev/null

print $BLUE "\nBuilding binaries..."

print $GREEN "\nBuild succeeded!\n"

cd $originalDir
exit 0
