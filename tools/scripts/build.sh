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
kernelDir=$srcDir/kernel

# Flags
kernelCompileFlags="-ffreestanding -nostdinc -nostdinc++ -funsigned-char \
					-Wall -Wextra -Wno-char-subscripts \
					-o kernel.bin -target i386-pc-none-elf \
					-I $kernelDir -I $kernelDir/system \
					-DPANDAOS_DEBUG -DPROJECT_ROOT_STRING_LENGTH=${#prjRoot}"
kernelLinkFlags="-nostdlib -Wl,--oformat=binary,-T$kernelDir/linkScript.ld"
kernelFiles="$kernelDir/unityBuild.cpp kernelEntryPoint.o registers.o usermode.o"

# Builds genVDisk if necessary
if [ ! -e $binDir/genVDisk/genVDisk ]; then
	$prjRoot/tools/scripts/buildGenVDisk.sh || exit_on_error
	echo
fi

# Build
mkdir -p $binDir/PandaOS
cd $binDir/PandaOS > /dev/null

print $BLUE "Cleaning..."
rm *.bin *.img *.iso *.o *.vmdk 2> /dev/null

print $BLUE "\nBuilding binaries..."
nasm -i $bootDir $bootDir/masterBootRecord.asm -o masterBootRecord.bin || exit_on_error
nasm -i $bootDir $bootDir/volumeBootRecord.asm -o volumeBootRecord.bin || exit_on_error
nasm -felf32 $kernelDir/entryPoint.asm -o kernelEntryPoint.o || exit_on_error
nasm -felf32 $kernelDir/memory/registers.asm -o registers.o || exit_on_error
nasm -felf32 $kernelDir/multitasking/usermode.asm -o usermode.o || exit_on_error
clang++ $kernelCompileFlags $kernelLinkFlags $kernelFiles || exit_on_error

if [ "$1" == "--floppy" ]; then
	print $BLUE "\nGenerating virtual floppy disk..."
	$binDir/genVDisk/genVDisk --output PandaOS \
							  --floppy \
							  --bootloader volumeBootRecord.bin \
							  --kernel kernel.bin \
		|| exit_on_error
else
	print $BLUE "\nGenerating virtual hard disk..."
	$binDir/genVDisk/genVDisk --output PandaOS \
							  --hdd \
							  --mbr masterBootRecord.bin \
							  --bootloader volumeBootRecord.bin \
							  --kernel kernel.bin \
		|| exit_on_error
fi

print $GREEN "\nBuild succeeded!\n"

# Run
$scriptDir/run.sh "$@"
echo

# Exit
cd $originalDir
exit 0
