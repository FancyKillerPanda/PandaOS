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
kernelCompileFlags="-ffreestanding -nostdinc -nostdinc++ -nostdlib -funsigned-char -o pKernelA.bin -target i386-pc-none-elf -I $kernelDir/system"
kernelLinkFlags="-Wl,--oformat=binary,-T$prjRoot/tools/kernelLinker.ld"
# kernelFiles="kernel_entry.o $kernelDir/kernel.cpp $kernelDir/system/display.cpp $kernelDir/system/memory.cpp"
kernelFiles="kernel_entry.o $kernelDir/*.cpp $kernelDir/system/*.cpp"

mkdir -p $binDir/PandaOS
pushd $binDir/PandaOS > /dev/null

print $BLUE "Cleaning..."
rm *.bin *.img *.iso *.o *.vmdk 2> /dev/null

print $BLUE "\nBuilding binaries..."
nasm -i $bootDir $bootDir/bootPandaOS.asm -o bootPandaOS.bin || exit_on_error
nasm -i $bootDir $bootDir/loadPandaOS.asm -o pkLoader.bin || exit_on_error
nasm -felf32 $kernelDir/kernel_entry.asm -o kernel_entry.o || exit_on_error
clang++ $kernelCompileFlags $kernelLinkFlags $kernelFiles || exit_on_error

print $BLUE "\nBuilding floppies..."
$prjRoot/tools/ffc_linux -b bootPandaOS.bin \
						 -s pkLoader.bin pKernelA.bin \
						 -o pandaFloppy.img \
	|| exit_on_error

print $BLUE "\nBuilding ISO image..."
genisoimage -V "PandaVolume" -input-charset iso8859-1 -o pandaOS.iso -b pandaFloppy.img . || exit_on_error

print $BLUE "\nBuilding virtual hard disk..."
$binDir/genVDisk/genVDisk --image-name PandaHDD --image-path ./ \
						  --vbr bootPandaOS.bin \
						  --size 64 \
						  --files pkLoader.bin pKernelA.bin \
	|| exit_on_error

print $GREEN "\nBuild succeeded!\n"

popd > /dev/null
exit 0
