#!/bin/bash
#  ===== Date Created: 01 December, 2020 ===== 

scriptDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" > /dev/null && pwd )"
source $scriptDir/buildCommon.sh

exit_on_error()
{
	print $RED "Run failed!\n"
	cd $originalDir
	exit 1
}

cd $binDir/PandaOS > /dev/null 2> /dev/null || exit_on_error
qemu-system-i386 --drive format=raw,file=volumeBootRecord.bin || exit_on_error

# Exit
cd $originalDir
exit 0
