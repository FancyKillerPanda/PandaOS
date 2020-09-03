#!/bin/bash
#  ===== Date Created: 03 September, 2020 ===== 

prjRoot=..

mkdir -p $prjRoot/bin/
nasm -f bin $prjRoot/src/boot/BootPandaOS.asm -o $prjRoot/bin/BootPandaOS.bin
