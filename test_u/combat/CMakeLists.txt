set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../../scripts")
include (xerces-c)

include (sdl)

if (UNIX)
 include (FindPkgConfig)
 pkg_check_modules (PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
endif (UNIX)

if (SDL_SUPPORT)
 include_directories (${SDL_INCLUDE_DIRS})
endif (SDL_SUPPORT)
if (UNIX)
 include_directories (${PKG_CONFIG_MODULE_LIBPNG_INCLUDE_DIRS})
elseif (WIN32)
 include_directories ($ENV{LIB_ROOT}/libpng/include)
endif ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/log)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_party/xsd
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../common
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../item
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../character
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../magic
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../combat
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../character/player
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../character/monster
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../map
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../engine
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../graphics)

########### next target ###############
set (EXECUTABLE combat_simulator)
set (${EXECUTABLE}_SRCS
     combat_simulator.cpp
     stdafx.h)

#set_source_files_properties (${${EXECUTABLE}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"C:\\\\\";DATADIR=\"/usr/local/share/RPG\"")

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})

if (UNIX)
 target_link_libraries (${EXECUTABLE} 
#                        ${PKG_CONFIG_MODULE_SDL_LIBRARIES}
#                        ${PKG_CONFIG_MODULE_SDL_TTF_LIBRARIES}
#                        ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES}
                        xerces-c
                        ${ACE_LIBRARY}
                        Common CommonLog CommonTimer
                        RPG_Dice RPG_Chance RPG_Common
                        RPG_Character RPG_Magic RPG_Item
                        RPG_Player RPG_Monster
                        RPG_Combat RPG_Engine
                        RPG_Chance RPG_Dice RPG_Common
                        RPG_Character RPG_Magic RPG_Item
                        RPG_Player
                        RPG_Map
                        RPG_Common
                        Common)
elseif (WIN32)
# set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

 target_link_libraries (${EXECUTABLE}
                        ${XERCESC_LIBRARIES}
                        ${ACE_LIBRARY}
                        Common CommonLog CommonTimer
                        RPG_Chance RPG_Dice RPG_Common RPG_Magic RPG_Item
                        RPG_Character RPG_Monster RPG_Player RPG_Combat
                        RPG_Map
                        RPG_Engine)
endif ()

set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

# add PCH
target_precompile_headers (${EXECUTABLE}
                           PRIVATE stdafx.h)

########### install files ###############
install (TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)

file (TO_NATIVE_PATH ${PROJECT_SOURCE_DIR} WORKING_DIRECTORY)
if (UNIX)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}" COMMAND)
 create_target_launcher (${EXECUTABLE}
                         COMMAND ${COMMAND}
                         ARGS "-l -t"
                         FORWARD_ARGS
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                         ENVIRONMENT foo=bar)
elseif (WIN32)
# get_filename_component (ACE_LIB_PATH ${ACE_LIBRARY} DIRECTORY)
 file (TO_CMAKE_PATH "$ENV{LIB_ROOT}" LIB_ROOT)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
 string (REPLACE "\\" "\\\\" WORKING_DIRECTORY ${WORKING_DIRECTORY})
 create_target_launcher (${EXECUTABLE}
                         COMMAND ${COMMAND}
                         ARGS "-l -t"
                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${XERCESC_LIB_DIR}
                                              ${LIB_ROOT}/ACE_TAO/ACE/lib
                         WORKING_DIRECTORY ${WORKING_DIRECTORY}
                         ENVIRONMENT foo=bar)
endif ()
