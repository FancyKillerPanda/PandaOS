#!/bin/bash
#  ===== Date Created: 03 September, 2020 ===== 

source ./buildCommon.sh

# Exits the script because of an error
exit_on_error()
{
	print $RED "\nBuild failed!\n"
	popd
	exit 1
}

# Directories
srcDir=$prjRoot/src
bootDir=$srcDir/boot
kernelDir=$srcDir/kernel

if [ ! -e $binDir/genVDisk/genVDisk ]; then
	$prjRoot/tools/buildGenVDisk.sh
fi

# Flags
kernelCompileFlags="-ffreestanding -nostdinc -nostdinc++ -nostdlib -funsigned-char -o pKernel.bin -target i386-pc-none-elf -I $kernelDir/system"
kernelLinkFlags="-Wl,--oformat=binary,-T$kernelDir/linkScript.ld"
kernelFiles="kernelEntry.o $kernelDir/*.cpp $kernelDir/system/*.cpp"

mkdir -p $binDir/PandaOS
pushd $binDir/PandaOS > /dev/null

print $BLUE "Cleaning..."
rm *.bin *.img *.iso *.o *.vmdk 2> /dev/null

print $BLUE "\nBuilding binaries..."
nasm -i $bootDir $bootDir/volumeBootRecord.asm -o masterBootRecord.bin || exit_on_error
nasm -i $bootDir $bootDir/volumeBootRecord.asm -o volumeBootRecord.bin || exit_on_error
nasm -i $bootDir $bootDir/kernelLoader.asm -o pkLoader.bin || exit_on_error
nasm -felf32 $kernelDir/kernelEntry.asm -o kernelEntry.o || exit_on_error
clang++ $kernelCompileFlags $kernelLinkFlags $kernelFiles || exit_on_error

print $BLUE "\nBuilding virtual hard disk..."
$binDir/genVDisk/genVDisk --image-name PandaHDD --image-path ./ \
						  --mbr masterBootRecord.bin \
						  --vbr volumeBootRecord.bin \
						  --size 64 \
						  --files pkLoader.bin pKernel.bin \
	|| exit_on_error

print $GREEN "\nBuild succeeded!\n"

popd > /dev/null
exit 0
