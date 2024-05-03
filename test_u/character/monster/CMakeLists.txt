
########### next target ###############
set (EXECUTABLE monster_parser)
set (${EXECUTABLE}_SRCS
     monster_parser.cpp
     stdafx.h)

set_source_files_properties (${${EXECUTABLE}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"C:\\\\\";DATADIR=\"/usr/local/share/RPG\"")

add_executable(${EXECUTABLE} ${${EXECUTABLE}_SRCS})

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../../Common/src/log)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../chance/dice
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../common
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../item
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../character
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../magic
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../combat
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../character/monster
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../engine)

if (UNIX)
 target_link_libraries (${EXECUTABLE}
                        xerces-c
                        ${ACE_LIBRARY}
                        Common CommonLog CommonTimer
                        RPG_Dice RPG_Chance RPG_Common
                        RPG_Character RPG_Magic RPG_Item
                        RPG_Combat RPG_Player RPG_Monster
                        Common
                        RPG_Dice RPG_Chance RPG_Common
                        RPG_Character RPG_Magic RPG_Item
                        RPG_Combat RPG_Player)
elseif (WIN32)
# set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

 target_link_libraries (${EXECUTABLE}
                        $ENV{LIB_ROOT}/xerces-c/build/msvc/src/Debug/xerces-c_3D.lib
                        $ENV{LIB_ROOT}/ACE_TAO/ACE/lib/ACEd.lib
                        Common CommonLog CommonTimer
                        RPG_Dice RPG_Chance RPG_Common
                        RPG_Character RPG_Magic RPG_Item
                        RPG_Combat RPG_Player RPG_Monster)
endif ()

set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

# add PCH
target_precompile_headers (${EXECUTABLE}
                           PRIVATE stdafx.h)

########### install files ################
install(TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)

########### launcher #####################
file (TO_CMAKE_PATH "$ENV{LIB_ROOT}" LIB_ROOT)
file (TO_CMAKE_PATH "${SDL_LIB_DIR}" SDL_LIB_PATH)
if (UNIX)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}" COMMAND)
elseif (WIN32)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
endif ()
create_target_launcher (${EXECUTABLE}
                        COMMAND ${COMMAND}
                        ARGS "-d -l -t"
                        FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${LIB_ROOT}/zlib/build/Debug
                                             ${LIB_ROOT}/xerces-c/build/msvc/src/Debug
                                             ${LIB_ROOT}/libpng/build/Debug
                                             ${LIB_ROOT}/ACE_TAO/ACE/lib
                                             ${SDL_LIB_PATH}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                        ENVIRONMENT foo=bar)
