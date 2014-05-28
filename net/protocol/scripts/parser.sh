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
SUBDIR=net/protocol
[ ! -d ./${SUBDIR} ] && echo "ERROR: invalid directory (was: ${SUBDIR}), aborting" && exit 1
[ ! -f ./${SUBDIR}/scripts/IRCparser.y ] && echo "ERROR: invalid file (was: ./${SUBDIR}/scripts/IRCparser.y), aborting" && exit 1

# generate a scanner for bisecting IRC messages from the input stream
bison --verbose --graph=parser_graph.txt --xml=parser_graph.xml ./${SUBDIR}/scripts/IRCparser.y --report=all --report-file=parser_report.txt --warnings=all
[ $? -ne 0 ] && echo "ERROR: failed to bison, aborting" && exit 1

# move the files into the project directory
#mv -f position.hh ./..
#mv -f stack.hh ./..
#mv -f location.hh ./..
#mv -f rpg_net_protocol_IRCparser.h ./..
# *NOTE*: need to add a specific method to the parser class
# --> copy a patched version back into the project directory
# *IMPORTANT NOTE*: needs to be updated after every change
cp -f ./net/protocol/scripts/rpg_net_protocol_IRCparser_patched.h ./net/protocol/rpg_net_protocol_IRCparser.h
if [ $? -ne 0 ]; then
 echo "ERROR: failed to cp, aborting"
 exit 1
fi
mv -f rpg_net_protocol_IRCparser.cpp ./..
if [ $? -ne 0 ]; then
 echo "ERROR: failed to mv, aborting"
 exit 1
fi
