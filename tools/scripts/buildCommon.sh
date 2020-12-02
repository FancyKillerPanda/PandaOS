#!/bin/bash
#  ===== Date Created: 01 December, 2020 ===== 

# Colours
RED="\033[0;31m"
GREEN="\033[0;92m"
BLUE="\033[0;36m"
DEFAULT_COLOUR="\033[0m"

# Argument one is the colour (ANSI escape sequence), argument two is
# the string to echo
print()
{
	echo -e "$1$2${DEFAULT_COLOUR}"
}

# Directories
prjDir=$scriptDir/../..
srcDir=$prjDir/src
binDir=$prjDir/bin
