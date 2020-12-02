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
bootDir=$srcDir/boot

# Builds genVDisk if necessary
# NOTE(fkp): Rebuild genVDisk every time for easier debugging
# if [ ! -e $binDir/genVDisk/genVDisk ]; then
	$prjRoot/tools/scripts/buildGenVDisk.sh
	echo
# fi

# Build
mkdir -p $binDir/PandaOS
cd $binDir/PandaOS > /dev/null

print $BLUE "Cleaning..."
rm *.bin *.img *.iso *.o *.vmdk 2> /dev/null

print $BLUE "\nBuilding binaries..."
nasm -i $bootDir $bootDir/volumeBootRecord.asm -o volumeBootRecord.bin || exit_on_error

print $BLUE "\nGenerating virtual disk..."
$binDir/genVDisk/genVDisk --output PandaOS.img \
						  --floppy \
						  --bootloader volumeBootRecord.bin \
	|| exit_on_error

print $GREEN "\nBuild succeeded!\n"

# Run
$scriptDir/run.sh

# Exit
cd $originalDir
exit 0
