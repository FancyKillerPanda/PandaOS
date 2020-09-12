@echo off
rem  ===== Date Created: 03 September, 2020 ===== 

set prjRoot=..
set startingDirectory=%CD%
set debug=false

if "%~1" == "-debug" set debug=true

if not exist %prjRoot%\bin\ (
	echo There is nothing to run...
) else (
	if %debug%==true (
		bochsdbg -q -f bochsrc-debug.bxrc
	) else (
		cd %prjRoot%\bin\
		qemu-system-i386 -cdrom pandaOS.iso
		cd %startingDirectory%
	)
)
