#!/bin/sh
#//%%%FILE%%%////////////////////////////////////////////////////////////////////
#// File Name: scanner.sh
#//
#// History:
#//   Date   |Name | Description of modification
#// ---------|-----|-------------------------------------------------------------
#// 20/02/06 | soh | Creation.
#//%%%FILE%%%////////////////////////////////////////////////////////////////////

# sanity check(s)
command -v flex >/dev/null 2>&1 || { echo "flex is not installed, aborting" >&2; exit 1; }
command -v tee >/dev/null 2>&1 || { echo "tee is not installed, aborting" >&2; exit 1; }
SUBDIR=map
[ ! -d ./${SUBDIR} ] && echo "ERROR: invalid directory (was: ${SUBDIR}), aborting" && exit 1
[ ! -f ./${SUBDIR}/scripts/map_scanner.l ] && echo "ERROR: invalid file (was: ./${SUBDIR}/scripts/map_scanner.l), aborting" && exit 1

# generate a scanner for map (text) files
flex ./${SUBDIR}/scripts/map_scanner.l 2>&1 | tee ./${SUBDIR}/scripts/scanner_report.txt
[ $? -ne 0 ] && echo "ERROR: failed to flex, aborting" && exit 1

# move the files into the project directory
mv -f rpg_map_scanner.cpp ./${SUBDIR}
mv -f rpg_map_scanner.h ./${SUBDIR}
if [ $? -ne 0 ]; then
 echo "ERROR: failed to mv, aborting"
 exit 1
fi
