#!/bin/sh
#//%%%FILE%%%////////////////////////////////////////////////////////////////////
#// File Name: scanner.sh
#//
#// History:
#//   Date   |Name | Description of modification
#// ---------|-----|-------------------------------------------------------------
#// 20/02/06 | soh | Creation.
#//%%%FILE%%%////////////////////////////////////////////////////////////////////

# remember where we are...
pushd . >/dev/null 2>&1

# jump to project scripts dir
PROJECT=map
cd ${HOME}/Projects/RPG/${PROJECT}/scripts

# generate a scanner for map (text) files
/usr/bin/flex -L --never-interactive -d --header=rpg_${PROJECT}_scanner.h -o rpg_${PROJECT}_scanner.cpp ./${PROJECT}_scanner.l

# move generated files into the project directory
mv -f rpg_${PROJECT}_scanner.h ./..
mv -f rpg_${PROJECT}_scanner.cpp ./..

# ...go back to where we came from
popd >/dev/null 2>&1
