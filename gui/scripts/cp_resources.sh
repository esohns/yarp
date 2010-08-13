#!/bin/sh
# author:      Erik Sohns <erik.sohns@web.de>
# this script makes/installs RPG
# *TODO*: it is neither portable nor particularly stable !
# parameters:   - $1 ["debug"] || "optimized"
#               - $2 "clean" (optional)
# return value: - 0 success, 1 failure

DEFAULT_BUILD="debug"
BUILD=${DEFAULT_BUILD}
[ "$#" -lt "1" ] && echo "using default build: \"${BUILD}\""

# parse any arguments
[ "$#" -ge "1" ] && BUILD=$1

DEFAULT_PROJECT_ROOT="${HOME}/Projects/RPG"
PROJECT_ROOT=${DEFAULT_PROJECT_ROOT}
# sanity check(s)
[ ! -d ${PROJECT_ROOT} ] && echo "ERROR: invalid project dir \"${PROJECT_ROOT}\" (not a directory), aborting" && exit 1

# remember current dir...
pushd . >/dev/null 2>&1

# goto build dir
BUILD_DIR=${PROJECT_ROOT}/gui/${BUILD}
# sanity check(s)
[ ! -d ${BUILD_DIR} ] && echo "ERROR: invalid build dir \"${BUILD_DIR}\" (not a directory), aborting" && exit 1
cd ${BUILD_DIR}

PROJECT=graphics
# cp graphics data
GRAPHICS_DIR=${PROJECT_ROOT}/${PROJECT}
# sanity check(s)
[ ! -d ${GRAPHICS_DIR} ] && echo "ERROR: invalid graphics dir \"${GRAPHICS_DIR}\" (not a directory), aborting" && exit 1
[ ! -f ${GRAPHICS_DIR}/rpg_graphics.xsd ] && echo "ERROR: invalid graphics XSD \"${GRAPHICS_DIR}/rpg_graphics.xsd\" (not a file), aborting" && exit 1
[ ! -f ${GRAPHICS_DIR}/rpg_graphics.xml ] && echo "ERROR: invalid graphics XML \"${GRAPHICS_DIR}/rpg_graphics.xml\" (not a file), aborting" && exit 1
cp -f ${GRAPHICS_DIR}/rpg_graphics.xsd .
cp -f ${GRAPHICS_DIR}/rpg_graphics.xml .
GRAPHICS_RES_DIR=${GRAPHICS_DIR}/data
# sanity check(s)
[ ! -d ${GRAPHICS_RES_DIR} ] && echo "ERROR: invalid graphics resource dir \"${GRAPHICS_RES_DIR}\" (not a directory), aborting" && exit 1
cp -rf ${GRAPHICS_RES_DIR}/* ./data/graphics

# cp sounds data
PROJECT=sound
SOUND_DIR=${PROJECT_ROOT}/${PROJECT}
# sanity check(s)
[ ! -d ${SOUND_DIR} ] && echo "ERROR: invalid sounds dir \"${SOUND_DIR}\" (not a directory), aborting" && exit 1
[ ! -f ${SOUND_DIR}/rpg_sound.xsd ] && echo "ERROR: invalid sound XSD \"${SOUND_DIR}/rpg_sound.xsd\" (not a file), aborting" && exit 1
[ ! -f ${SOUND_DIR}/rpg_sound.xml ] && echo "ERROR: invalid sound XML \"${SOUND_DIR}/rpg_sound.xml\" (not a file), aborting" && exit 1
cp -f ${SOUND_DIR}/rpg_sound.xsd .
cp -f ${SOUND_DIR}/rpg_sound.xml .
SOUNDS_RES_DIR=${SOUND_DIR}/data
# sanity check(s)
[ ! -d ${SOUNDS_RES_DIR} ] && echo "ERROR: invalid sounds resource dir \"${SOUNDS_RES_DIR}\" (not a directory), aborting" && exit 1
cp -rf ${SOUNDS_RES_DIR}/* ./data/sounds

# ...go back where we came from
popd >/dev/null 2>&1
