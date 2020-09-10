#!/bin/bash
#  ===== Date Created: 03 September, 2020 ===== 

prjRoot=..
srcDir=$prjRoot/src

mkdir -p $prjRoot/bin
pushd $prjRoot/bin > /dev/null

echo "Cleaning..."
rm *.bin *.img *.iso 2> /dev/null

echo
echo "Building binaries..."
nasm -i $srcDir/boot $srcDir/boot/bootPandaOS.asm -o bootPandaOS.bin
nasm -i $srcDir/boot $srcDir/boot/loadPandaOS.asm -o pkLoader.bin
nasm -i $srcDir/boot $srcDir/boot/kernel.asm -o pKernelA.bin

echo
echo "Building floppies..."
../tools/ffc_linux -b bootPandaOS.bin -s pkLoader.bin pKernelA.bin -o pandaFloppy.img --ls-fat

echo
echo "Building ISO image..."
genisoimage -V 'PandaVolume' -input-charset iso8859-1 -o pandaOS.iso -b pandaFloppy.img .

echo
echo "Done!"

popd > /dev/null
