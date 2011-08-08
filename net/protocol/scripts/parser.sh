#!/bin/sh
#//%%%FILE%%%////////////////////////////////////////////////////////////////////
#// File Name: parser.sh
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
/usr/bin/bison --language=C++ --skeleton="lalr1.cc" --debug --locations --no-lines --token-table --verbose --defines=rpg_net_protocol_IRCparser.h --output=rpg_net_protocol_IRCparser.cpp --graph=parser_graph.txt --xml=parser_graph.xml ./IRCparser.y --report=all --report-file=report.txt --warnings=all

# move the file into our project directory
mv -f position.hh ./..
mv -f stack.hh ./..
mv -f location.hh ./..
mv -f rpg_net_protocol_IRCparser.h ./..
mv -f rpg_net_protocol_IRCparser.cpp ./..

# go back
popd >/dev/null 2>&1
