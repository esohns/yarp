#!/bin/sh
#//%%%FILE%%%////////////////////////////////////////////////////////////////////
#// File Name: parser.sh
#//
#// History:
#//   Date   |Name | Description of modification
#// ---------|-----|-------------------------------------------------------------
#// 20/02/06 | soh | Creation.
#//%%%FILE%%%////////////////////////////////////////////////////////////////////

# sanity check(s)
command -v bison >/dev/null 2>&1 || { echo "bison is not installed, aborting" >&2; exit 1; }
SUBDIR=map
[ ! -d ./${SUBDIR} ] && echo "ERROR: invalid directory (was: ${SUBDIR}), aborting" && exit 1
[ ! -f ./${SUBDIR}/scripts/map_parser.y ] && echo "ERROR: invalid file (was: ./${SUBDIR}/scripts/map_parser.y), aborting" && exit 1

# generate parser for map (text) files
bison --verbose --graph=parser_graph.txt --xml=parser_graph.xml ./${SUBDIR}/scripts/map_parser.y --report=all --report-file=parser_report.txt --warnings=all
[ $? -ne 0 ] && echo "ERROR: failed to bison, aborting" && exit 1

# move the files into the project directory
mv -f position.hh ./..
mv -f stack.hh ./..
mv -f location.hh ./..
mv -f rpg_map_parser.h ./..
mv -f rpg_map_parser.cpp ./..
if [ $? -ne 0 ]; then
 echo "ERROR: failed to mv, aborting"
 exit 1
fi
