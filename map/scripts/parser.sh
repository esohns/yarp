#!/bin/sh
#//%%%FILE%%%////////////////////////////////////////////////////////////////////
#// File Name: parser.sh
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

# generate a parser for map (text) files
/usr/bin/bison --language=C++ --skeleton="lalr1.cc" --debug --locations --no-lines --token-table --verbose --defines=rpg_${PROJECT}_parser.h --output=rpg_${PROJECT}_parser.cpp ./${PROJECT}_parser.y --report=all --report-file=report.txt --warnings=all

# move generated files into the project directory
mv -f position.hh ./..
mv -f stack.hh ./..
mv -f location.hh ./..
mv -f rpg_${PROJECT}_parser.h ./..
mv -f rpg_${PROJECT}_parser.cpp ./..

# ...go back to where we came from
popd >/dev/null 2>&1
