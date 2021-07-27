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

if [ "$1" == "--floppy" ]; then
	qemu-system-i386 -drive if=floppy,index=0,format=raw,file="PandaOS.img" \
					 -debugcon stdio \
		|| exit_on_error
else
	qemu-system-i386 -drive index=0,media=disk,file="PandaOS.vmdk" \
					 -debugcon stdio \
		|| exit_on_error
fi

# Exit
cd $originalDir
exit 0
