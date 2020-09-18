@echo off
rem  ===== Date Created: 03 September, 2020 ===== 

set prjRoot=%CD%\..
set debug=false

if "%~1" == "-debug" set debug=true

if not exist %prjRoot%\bin\PandaOS\ (
	echo There is nothing to run...
) else (
	if %debug%==true (
		bochsdbg -q -f bochsrc-debug.bxrc
	) else (
		pushd %prjRoot%\bin\PandaOS\
		qemu-system-i386 -drive file=PandaHDD-flat.vmdk,index=0,media=disk,format=raw
		popd
	)
)
