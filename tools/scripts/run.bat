@echo off
rem  ===== Date Created: 03 September, 2020 ===== 

set scriptDir=%~dp0
set prjRoot=%scriptDir%\..\..
set debug=false
set gui=true

for %%a in (%*) do (
	if [%%a]==[-debug] (
		set debug=true
	) else if [%%a]==[-nogui] (
		set gui=false
	)
)

if not exist %prjRoot%\bin\PandaOS\ (
	echo There is nothing to run...
) else (
	if %debug%==true (
		if %gui%==true (
			bochsdbg -q -f %prjRoot%\tools\debug\bochsrc-debug.bxrc
		) else (
			bochsdbg -q -f %prjRoot%\tools\debug\bochsrc.bxrc
		)
	) else (
		pushd %prjRoot%\bin\PandaOS\
		qemu-system-i386 -drive file=PandaHDD.vmdk,index=0,media=disk -debugcon stdio -serial file:pandaLog.txt
		popd
	)
)
