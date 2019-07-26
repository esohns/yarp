#!/bin/sh
# author:      Erik Sohns <eriksohns@123mail.org>
# this script copies config.h of the ACE framework to the source directory.
# --> allows reuse of the same ACE source tree on different platforms.
# *NOTE*: it is neither portable nor particularly stable !
# parameters:   - (UNIX) platform [linux|solaris]
# return value: - 0 success, 1 failure

# sanity checks
#command -v dirname >/dev/null 2>&1 || { echo "dirname is not installed, aborting" >&2; exit 1; }
#command -v readlink >/dev/null 2>&1 || { echo "readlink is not installed, aborting" >&2; exit 1; }

#DEFAULT_PLATFORM="linux"
#PLATFORM=${DEFAULT_PLATFORM}
#if [ $# -lt 1 ]; then
# echo "INFO: using default platform: \"${PLATFORM}\""
#else
# # parse any arguments
# if [ $# -ge 1 ]; then
#  PLATFORM="$1"
# fi
#fi
#echo "DEBUG: platform: \"${PLATFORM}\""

DEFAULT_PROJECT_DIRECTORY="$(dirname $(readlink -f $0))/.."
PROJECT_DIRECTORY=${DEFAULT_PROJECT_DIRECTORY}
# sanity check(s)
[ ! -d ${PROJECT_DIRECTORY} ] && echo "ERROR: invalid projects directory (was: \"${PROJECTS_DIRECTORY}\"), aborting" && exit 1
#echo "DEBUG: project directory: \"${PROJECT_DIRECTORY}\""
echo "set project directory: \"${PROJECT_DIRECTORY}\""
cd ${PROJECT_DIRECTORY}

MODULES="chance/dice
character
character/player
character/monster
client
combat
common
engine
graphics
item
magic
map
sound"
for MODULE in $MODULES
do
 MODULE_PATH="${PROJECT_DIRECTORY}/${MODULE}"
 [ ! -d ${MODULE_PATH} ] && echo "ERROR: invalid project directory (was: \"${MODULE_PATH}\"), aborting" && exit 1
 SCRIPT_PATH="${MODULE_PATH}/scripts"
 [ ! -d ${SCRIPT_PATH} ] && echo "ERROR: invalid build directory (was: \"${SCRIPT_PATH}\"), aborting" && exit 1
# cd $SCRIPT_PATH
# [ $? -ne 0 ] && echo "ERROR: failed to cd to \"${SCRIPT_PATH}\": $?, aborting" && exit 1

 SCRIPT_FILE="$SCRIPT_PATH/bindings.sh"
 [ ! -x ${SCRIPT_FILE} ] && echo "ERROR: invalid script (was: \"${SCRIPT_FILE}\"), aborting" && exit 1

 $(${SCRIPT_FILE})
 [ $? -ne 0 ] && echo "ERROR: failed to run script (was: \"${SCRIPT_FILE}\"): $?, aborting" && exit 1

 echo "processing ${MODULE}...DONE"
done

