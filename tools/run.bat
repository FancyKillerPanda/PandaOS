@echo off
rem  ===== Date Created: 03 September, 2020 ===== 

set prjRoot=..
set startingDirectory=%CD%

if not exist %prjRoot%\bin\ (
	echo There is nothing to run...
) else (
	cd %prjRoot%\bin\
	qemu-system-x86_64 --drive format=raw,file=BootPandaOS.bin
	cd %startingDirectory%
)

