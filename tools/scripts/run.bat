@echo off
rem  ===== Date Created: 03 September, 2020 ===== 

set scriptDir=%~dp0
set prjRoot=%scriptDir%\..\..
set debug=false

if "%~1" == "-debug" set debug=true

if not exist %prjRoot%\bin\PandaOS\ (
	echo There is nothing to run...
) else (
	if %debug%==true (
		bochsdbg -q -f %prjRoot%\tools\debug\bochsrc-debug.bxrc
	) else (
		pushd %prjRoot%\bin\PandaOS\
		qemu-system-i386 -drive file=PandaHDD.vmdk,index=0,media=disk
		popd
	)
)
