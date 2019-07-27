#!/bin/sh
# author:      Erik Sohns <erik.sohns@web.de>
# this script gathers the RPG libraries in one place for in-source-tree
# debugging
# *NOTE*: it is neither portable nor particularly stable !
# parameters:   - $1 [BUILD] {"debug" || "debug_tracing" || "release" || ...}
# return value: - 0 success, 1 failure

DEFAULT_PROJECT_DIR="$(dirname $(readlink -f $0))/.."
PROJECT_DIR=${DEFAULT_PROJECT_DIR}
# sanity check(s)
[ ! -d ${PROJECT_DIR} ] && echo "ERROR: invalid project dir (was: \"${PROJECT_DIR}\"), aborting" && exit 1

DEFAULT_BUILD="debug"
BUILD=${DEFAULT_BUILD}
if [ $# -lt 1 ]; then
 echo "INFO: using default build: \"${BUILD}\""
else
 # parse any arguments
 if [ $# -ge 1 ]; then
  BUILD="$1"
 fi
fi

# sanity check(s)
[ ${BUILD} != "debug" -a ${BUILD} != "debug_tracing" -a ${BUILD} != "release" ] && echo "WARNING: invalid/unknown build (was: \"${BUILD}\"), continuing"
BUILD_DIR="${PROJECT_DIR}/build/${BUILD}"
[ ! -d "${BUILD_DIR}" ] && echo "ERROR: invalid build dir (was: \"${BUILD_DIR}\"), aborting" && exit 1

LIB_DIR="lib"
TARGET_DIR="${BUILD_DIR}/${LIB_DIR}"
if [ ! -d "${TARGET_DIR}" ]; then
 mkdir ${TARGET_DIR}
 [ $? -ne 0 ] && echo "ERROR: failed to mkdir \"${TARGET_DIR}\", aborting" && exit 1
 echo "INFO: created directory \"${TARGET_DIR}\", continuing"
fi
VERSION="6.3.1"

echo "copying 3rd-party libraries"
LIB_DIR="lib"
MODULES_DIR="${PROJECT_DIR}/modules"
SUB_DIRS="ATCD/ACE/build/linux"
declare -a LIBS=("libACE.so")
i=0
for DIR in $SUB_DIRS
do
 LIB="${MODULES_DIR}/${DIR}/${LIB_DIR}/${LIBS[$i]}"
 [ ! -r "${LIB}.${VERSION}" ] && echo "ERROR: invalid library file (was: \"${LIB}.${VERSION}\"), aborting" && exit 1
 cp -f "${LIB}.${VERSION}" ${TARGET_DIR}
 [ $? -ne 0 ] && echo "ERROR: failed to copy \"${LIB}.${VERSION}\" to \"${TARGET_DIR}\": $?, aborting" && exit 1
 echo "copied \"$LIB.${VERSION}\"..."

 i=$i+1
done
VERSION="0"

echo "copying external module libraries"
LIB_DIR=".libs"
SUB_DIRS="modules/libCommon/src
modules/libCommon/src/ui
modules/libACEStream/src
modules/libACENetwork/src
modules/libACENetwork/src/client_server
modules/libACENetwork/src/client_server"
declare -a LIBS=("libCommon.so"
"libCommon_UI.so"
"libACEStream.so"
"libACENetwork.so"
"libACENetwork_Client.so"
"libACENetwork_Server.so")
i=0
for DIR in $SUB_DIRS
do
 LIB="${BUILD_DIR}/${DIR}/${LIB_DIR}/${LIBS[$i]}"
 [ ! -r "${LIB}.${VERSION}" ] && echo "ERROR: invalid library file (was: \"${LIB}.${VERSION}\"), aborting" && exit 1
 cp -f "${LIB}.${VERSION}" ${TARGET_DIR}
 [ $? -ne 0 ] && echo "ERROR: failed to copy \"${LIB}.${VERSION}\" to \"${TARGET_DIR}\": $?, aborting" && exit 1
 echo "copied \"$LIB.${VERSION}\"..."

 i=$i+1
done

echo "copying framework libraries"
LIB_DIR=".libs"
SUB_DIRS="chance/dice
chance
character/monster
character/player
character
client
combat
common
engine
graphics
item
magic
map
net
net/protocol
net/server
sound"
declare -a LIBS=("libRPG_Dice.so"
"libRPG_Chance.so"
"libRPG_Monster.so"
"libRPG_Player.so"
"libRPG_Character.so"
"libRPG_Client_GUI.so"
"libRPG_Combat.so"
"libRPG_Common.so"
"libRPG_Engine.so"
"libRPG_Graphics.so"
"libRPG_Item.so"
"libRPG_Magic.so"
"libRPG_Map.so"
"libRPG_Net.so"
"libRPG_Net_Protocol.so"
"libRPG_Net_Server.so"
"libRPG_Sound.so")
i=0
for DIR in $SUB_DIRS
do
 LIB="${BUILD_DIR}/${DIR}/${LIB_DIR}/${LIBS[$i]}"
 [ ! -r "${LIB}.${VERSION}" ] && echo "ERROR: invalid library file (was: \"${LIB}.${VERSION}\"), aborting" && exit 1
 cp -f "${LIB}.${VERSION}" ${TARGET_DIR}
 [ $? -ne 0 ] && echo "ERROR: failed to copy \"${LIB}.${VERSION}\" to \"${TARGET_DIR}\": $?, aborting" && exit 1
 echo "copied \"$LIB.${VERSION}\"..."

 i=$i+1
done

