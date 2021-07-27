@echo off
rem  ===== Date Created: 02 December, 2020 ===== 

set scriptDir=%~dp0
set prjRoot=%scriptDir%\..\..

if not exist %prjRoot%\bin\PandaOS\ (
	echo There is nothing to run...
) else (
	cls

	if exist %prjRoot%\bin\PandaOS\PandaOS.img (
		bochsdbg -q -f %prjRoot%\tools\debug\bochsrc-fdd.bxrc
	) else (
		bochsdbg -q -f %prjRoot%\tools\debug\bochsrc-hdd.bxrc
	)
)
