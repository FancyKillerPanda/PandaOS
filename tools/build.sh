#!/bin/bash
#  ===== Date Created: 03 September, 2020 ===== 

RED="\033[0;31m"
GREEN="\033[0;92m"
BLUE="\033[0;36m"

DEFAULT_COLOUR="\033[0m"

# Argument one is the colour (ANSI escape sequence), argument two is
# the string to echo
print()
{
	echo -e "$1$2${DEFAULT_COLOUR}"
}

# Exits the script because of an error
exit_on_error()
{
	print $RED "\nBuild failed!\n"
	popd
	exit 1
}

# Directories
prjRoot=..
srcDir=$prjRoot/src
bootDir=$srcDir/boot
kernelDir=$srcDir/kernel

# Flags
kernelCompileFlags="-ffreestanding -nostdinc -nostdinc++ -nostdlib -funsigned-char -o pKernelA.bin -target i386-pc-none-elf -I $kernelDir/system"
kernelLinkFlags="-Wl,--oformat=binary,-T$prjRoot/tools/kernelLinker.ld"
# kernelFiles="kernel_entry.o $kernelDir/kernel.cpp $kernelDir/system/display.cpp $kernelDir/system/memory.cpp"
kernelFiles="kernel_entry.o $kernelDir/*.cpp $kernelDir/system/*.cpp"

mkdir -p $prjRoot/bin
pushd $prjRoot/bin > /dev/null

print $BLUE "Cleaning..."
rm *.bin *.img *.iso *.o 2> /dev/null

print $BLUE "\nBuilding binaries..."
nasm -i $bootDir $bootDir/bootPandaOS.asm -o bootPandaOS.bin || exit_on_error
nasm -i $bootDir $bootDir/loadPandaOS.asm -o pkLoader.bin || exit_on_error
nasm -felf32 $kernelDir/kernel_entry.asm -o kernel_entry.o || exit_on_error
clang $kernelCompileFlags $kernelLinkFlags $kernelFiles || exit_on_error

print $BLUE "\nBuilding floppies..."
../tools/ffc_linux -b bootPandaOS.bin \
				   -s pkLoader.bin pKernelA.bin \
				   -o pandaFloppy.img \
				   --ls-fat \
	|| exit_on_error

print $BLUE "\nBuilding ISO image..."
genisoimage -V "PandaVolume" -input-charset iso8859-1 -o pandaOS.iso -b pandaFloppy.img . || exit_on_error

print $GREEN "\nBuild succeeded!\n"

popd > /dev/null
exit 0
