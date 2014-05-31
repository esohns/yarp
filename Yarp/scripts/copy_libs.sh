#!/bin/sh
# author:      Erik Sohns <erik.sohns@web.de>
# this script gathers the RPG libraries in one plac for in-source-tree debugging
# *NOTE*: it is neither portable nor particularly stable !
# parameters:   - $1 [BUILD] {"debug" || "debug_tracing" || "release" || ...}
# return value: - 0 success, 1 failure

DEFAULT_PROJECT_DIR="/mnt/win_c/Temp/yarp/Yarp"
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
net/client
net/protocol
net/server
net/stream
net
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
"libRPG_Net_Client.so"
"libRPG_Net_Protocol.so"
"libRPG_Net_Server.so"
"libRPG_Stream.so"
"libRPG_Net.so"
"libRPG_Sound.so")
i=0
for DIR in $SUB_DIRS
do
 LIB="${BUILD_DIR}/${DIR}/${LIB_DIR}/${LIBS[$i]}"
 [ ! -r "${LIB}" ] && echo "ERROR: invalid library file (was: \"${LIB}\"), aborting" && exit 1
 i=$i+1

 cp ${LIB} ${TARGET_DIR}
 [ $? -ne 0 ] && echo "ERROR: failed to copy \"${LIB}\" to \"${TARGET_DIR}\": $?, aborting" && exit 1

 echo "copied \"$LIB\"..."
done

