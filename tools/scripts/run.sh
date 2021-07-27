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

imageFile="PandaOS-flat.vmdk"

if [ "$1" == "--floppy" ]; then
	imageFile="PandaOS.img"
fi

cd $binDir/PandaOS > /dev/null 2> /dev/null || exit_on_error
qemu-system-i386 -boot order=adc \
				 -drive if=floppy,index=0,format=raw,file=$imageFile \
				 -debugcon stdio \
	|| exit_on_error

# Exit
cd $originalDir
exit 0
