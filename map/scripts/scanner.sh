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
flex ./${PROJECT}_scanner.l 2>&1 | tee ./scanner_report.txt

# move generated files into the project directory
mv -f lex.yy.cc ./..

# ...go back to where we came from
popd >/dev/null 2>&1
