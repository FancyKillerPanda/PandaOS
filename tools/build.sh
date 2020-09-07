#!/bin/bash
#  ===== Date Created: 03 September, 2020 ===== 

prjRoot=..

mkdir -p $prjRoot/bin/
nasm -i $prjRoot/src/boot/ -f bin $prjRoot/src/boot/bootPandaOS.asm -o $prjRoot/bin/bootPandaOS.bin
