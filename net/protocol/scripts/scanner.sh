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
/usr/bin/flex -b -B -d --header=rpg_net_protocol_IRCbisect.h -L -o rpg_net_protocol_IRCbisect.cpp -p -v ./IRCbisect.l 2>&1 | tee ./scanner_report.txt

# move the file into our project directory
mv -f rpg_net_protocol_IRCbisect.h ./..
mv -f rpg_net_protocol_IRCbisect.cpp ./..

# -------------------------------------------------------------------

# generate a scanner for use by our IRC message parser
/usr/bin/flex -b -B -d --header=rpg_net_protocol_IRCscanner.h -L -o rpg_net_protocol_IRCscanner.cpp -p -v ./IRCscanner.l 2>&1 | tee -a ./scanner_report.txt

# move the file into the project directory
mv -f rpg_net_protocol_IRCscanner.h ./..
mv -f rpg_net_protocol_IRCscanner.cpp ./..

# go back
popd >/dev/null 2>&1
