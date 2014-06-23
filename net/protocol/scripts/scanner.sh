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
SUBDIR=net/protocol
[ ! -d ./${SUBDIR} ] && echo "ERROR: invalid directory (was: ${SUBDIR}), aborting" && exit 1
[ ! -f ./${SUBDIR}/scripts/IRCbisect.l ] && echo "ERROR: invalid file (was: ./${SUBDIR}/scripts/IRCbisect.l), aborting" && exit 1
[ ! -f ./${SUBDIR}/scripts/IRCbisect.l ] && echo "ERROR: invalid file (was: ./${SUBDIR}/scripts/IRCbisect.l), aborting" && exit 1

# generate a scanner for bisecting IRC messages from the input stream
flex ${SUBDIR}/scripts/IRCbisect.l 2>&1 | tee ${SUBDIR}/scripts/IRCbisect_scanner_report.txt
[ $? -ne 0 ] && echo "ERROR: failed to flex, aborting" && exit 1

# move the files into the project directory
mv -f rpg_net_protocol_IRCbisect.cpp ./${SUBDIR}
mv -f rpg_net_protocol_IRCbisect.h ./${SUBDIR}
[ $? -ne 0 ] && echo "ERROR: failed to mv, aborting" && exit 1

# -------------------------------------------------------------------

# generate a scanner for use by the IRC message parser
flex ${SUBDIR}/scripts/IRCscanner.l 2>&1 | tee ${SUBDIR}/scripts/IRCScanner_scanner_report.txt
[ $? -ne 0 ] && echo "ERROR: failed to flex, aborting" && exit 1

# move the files into the project directory
mv -f rpg_net_protocol_IRCscanner.cpp ./${SUBDIR}
mv -f rpg_net_protocol_IRCscanner.h ./${SUBDIR}
if [ $? -ne 0 ]; then
 echo "ERROR: failed to mv, aborting"
 exit 1
fi
