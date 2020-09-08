#!/bin/bash
#  ===== Date Created: 03 September, 2020 ===== 

prjRoot=..
binDir=$prjRoot/bin

echo "Building binaries..."
mkdir -p $binDir/
nasm -i $prjRoot/src/boot/ -f bin $prjRoot/src/boot/bootPandaOS.asm -o $binDir/bootPandaOS.bin
nasm -i $prjRoot/src/boot/ -f bin $prjRoot/src/boot/loadPandaOS.asm -o $binDir/2ndstage.bin

echo "Building floppies..."
dd if=/dev/zero of=$binDir/bootPandaOS.flp bs=1024 count=1440
dd if=$binDir/bootPandaOS.bin of=$binDir/bootPandaOS.flp seek=0 count=1 conv=notrunc

echo "Building ISO image..."
genisoimage -V "PandaVolume" -input-charset iso8859-1 -o $binDir/pandaOS.iso \
			-b bootPandaOS.flp -hide bootPandaOS.flp $binDir
