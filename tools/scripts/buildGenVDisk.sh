#!/bin/bash
#  ===== Date Created: 14 September, 2020 ===== 

scriptDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" > /dev/null && pwd )"
source $scriptDir/buildCommon.sh

exit_on_error()
{
	print $RED "\ngenVDisk build failed!\n";
	popd
	cd $originalDir
	exit 1
}

# Directories
srcDir=$prjRoot/src/genVDisk/

# Flags
compileFlags="-o genVDisk -I $srcDir -funsigned-char -O3"
linkFlags=""
files="$srcDir/unityBuild.cpp"

mkdir -p $binDir/genVDisk
pushd $binDir/genVDisk > /dev/null

print $BLUE "Cleaning genVDisk..."
rm genVDisk 2> /dev/null
print $BLUE "\nBuilding...";
clang++ $compileFlags $linkFlags $files || exit_on_error
print $GREEN "\ngenVDisk build succeeded!\n"

popd > /dev/null
cd $originalDir
exit 0
