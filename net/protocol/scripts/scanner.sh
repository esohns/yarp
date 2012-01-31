#!/bin/sh
#//%%%FILE%%%////////////////////////////////////////////////////////////////////
#// File Name: scanner.sh
#//
#// History:
#//   Date   |Name | Description of modification
#// ---------|-----|-------------------------------------------------------------
#// 20/02/06 | soh | Creation.
#//%%%FILE%%%////////////////////////////////////////////////////////////////////

# remember where we are
pushd . >/dev/null 2>&1

# jump to project scripts dir
PROJECT=net/protocol
cd ${HOME}/Projects/RPG/${PROJECT}/scripts

# generate a scanner for bisecting IRC messages from the input stream
flex ./IRCbisect.l 2>&1 | tee ./IRCbisect_scanner_report.txt

# move the file into our project directory
mv -f rpg_net_protocol_IRCbisect.cpp ./..
mv -f rpg_net_protocol_IRCbisect.h ./..

# -------------------------------------------------------------------

# generate a scanner for use by our IRC message parser
flex ./IRCscanner.l 2>&1 | tee -a ./IRCScanner_scanner_report.txt

# move the file into the project directory
mv -f rpg_net_protocol_IRCscanner.cpp ./..
mv -f rpg_net_protocol_IRCscanner.h ./..

# go back
popd >/dev/null 2>&1
