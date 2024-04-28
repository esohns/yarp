if (UNIX)
 include (FindPkgConfig)
 pkg_check_modules (PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
endif (UNIX)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/sdl.cmake)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/log)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/timer)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../common)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../character)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../magic)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../item)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../combat)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../character/player)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../character/monster)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../map)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../engine)

############## next target ###############
set (EXECUTABLE map_parser)
set (${EXECUTABLE}_SRCS
     map_parser.cpp
     stdafx.h)

set_source_files_properties (${${EXECUTABLE}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"C:\\\\\"")

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})

add_dependencies (${EXECUTABLE} RPG_Dice RPG_Chance RPG_Common RPG_Character RPG_Magic RPG_Item
                                RPG_Combat RPG_Player RPG_Monster RPG_Map RPG_Engine)

if (UNIX)
 target_link_libraries (${EXECUTABLE}
                        xerces-c
                        ${ACE_LIBRARY}
                        Common CommonError CommonLog CommonTimer
                        ACEStream ACEStream_Network
                        ACENetwork
                        RPG_Dice RPG_Chance RPG_Common
                        RPG_Character RPG_Magic RPG_Item
                        RPG_Player RPG_Monster RPG_Combat
                        RPG_Map RPG_Net RPG_Net_Protocol RPG_Engine
                        RPG_Dice RPG_Common RPG_Player RPG_Monster RPG_Map
                        RPG_Dice RPG_Character RPG_Magic RPG_Item
                        RPG_Common RPG_Combat RPG_Player RPG_Chance)
elseif (WIN32)
# set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

 target_link_libraries (${EXECUTABLE}
                        $ENV{LIB_ROOT}/xerces-c/build/msvc/src/Debug/xerces-c_3D.lib
                        $ENV{LIB_ROOT}/ACE_TAO/ACE/lib/ACEd.lib
                        Common CommonError CommonLog CommonTimer
                        ACEStream ACEStream_Network
                        ACENetwork
                        RPG_Dice RPG_Chance RPG_Common
                        RPG_Character RPG_Magic RPG_Item
                        RPG_Player RPG_Monster RPG_Combat
                        RPG_Map RPG_Net RPG_Net_Protocol RPG_Engine)
endif ()

set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

# add PCH
target_precompile_headers (${EXECUTABLE}
                           PRIVATE stdafx.h)

#----------- install files ---------------
install (TARGETS ${EXECUTABLE}
         RUNTIME DESTINATION bin)

########### next target ###############
set (EXECUTABLE_2 map_generator)
set (${EXECUTABLE_2}_SRCS
     map_generator.cpp
     stdafx.h)

set_source_files_properties (${${EXECUTABLE_2}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"C:\\\\\"")

if (UNIX)
 include_directories (${PKG_CONFIG_MODULE_LIBPNG_INCLUDE_DIRS})
elseif (WIN32)
 include_directories ($ENV{LIB_ROOT}/libpng)
endif ()
if (SDL_SUPPORT)
 include_directories (${SDL_INCLUDE_DIRS})
endif (SDL_SUPPORT)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_party/xsd)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../graphics)

add_executable (${EXECUTABLE_2}
                ${${EXECUTABLE_2}_SRCS})

add_dependencies (${EXECUTABLE_2}
                  RPG_Dice RPG_Common RPG_Character RPG_Magic RPG_Item
                  RPG_Monster RPG_Player RPG_Combat RPG_Map RPG_Graphics RPG_Engine)

if (SDL_SUPPORT)
 target_link_libraries (${EXECUTABLE_2}
                        ${SDL_LIBRARIES})
endif (SDL_SUPPORT)
if (UNIX)
 target_link_libraries (${EXECUTABLE_2}
                        ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES}
                        xerces-c
                        ${ACE_LIBRARY}
                        Common CommonError CommonLog CommonTimer
                        ACEStream ACEStream_Network
                        ACENetwork
                        RPG_Chance RPG_Dice RPG_Common
                        RPG_Character RPG_Magic RPG_Item
                        RPG_Player RPG_Monster RPG_Combat
                        RPG_Map RPG_Graphics RPG_Net RPG_Net_Protocol RPG_Engine
                        RPG_Common RPG_Player RPG_Monster RPG_Map RPG_Graphics
                        RPG_Dice RPG_Character RPG_Magic RPG_Item
                        RPG_Common RPG_Combat RPG_Player RPG_Chance)
elseif (WIN32)
# set_target_properties (${EXECUTABLE_2} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

 target_link_libraries (${EXECUTABLE_2}
                        $ENV{LIB_ROOT}/xerces-c/build/msvc/src/Debug/xerces-c_3D.lib
                        $ENV{LIB_ROOT}/ACE_TAO/ACE/lib/ACEd.lib
                        Common CommonError CommonLog CommonTimer
                        ACEStream ACEStream_Network
                        ACENetwork
                        RPG_Chance RPG_Dice RPG_Common
                        RPG_Character RPG_Magic RPG_Item
                        RPG_Player RPG_Monster RPG_Combat
                        RPG_Map RPG_Graphics RPG_Net RPG_Net_Protocol RPG_Engine)
endif ()

set_property (TARGET ${EXECUTABLE_2} PROPERTY FOLDER "test_u")

# add PCH
target_precompile_headers (${EXECUTABLE_2}
                           REUSE_FROM ${EXECUTABLE})

#----------- install files ---------------
install (TARGETS ${EXECUTABLE_2}
         RUNTIME DESTINATION bin)

########### next target ###############
set (EXECUTABLE_3 path_finder)
set (${EXECUTABLE_3}_SRCS
     path_finder.cpp
     stdafx.h)

set_source_files_properties (${${EXECUTABLE_3}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"C:\\\\\"")

add_executable (${EXECUTABLE_3}
                ${${EXECUTABLE_3}_SRCS})

add_dependencies (${EXECUTABLE_3} RPG_Dice RPG_Common RPG_Map)

if (UNIX)
 target_link_libraries (${EXECUTABLE_3}
                        xerces-c
                        ${ACE_LIBRARY}
                        Common CommonError CommonLog CommonTimer
                        RPG_Dice RPG_Common RPG_Map)
elseif (WIN32)
# set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

 target_link_libraries (${EXECUTABLE_3}
                        Userenv.lib
                        $ENV{LIB_ROOT}/xerces-c/build/msvc/src/Debug/xerces-c_3D.lib
                        $ENV{LIB_ROOT}/ACE_TAO/ACE/lib/ACEd.lib
                        Common CommonError CommonLog CommonTimer
                        RPG_Dice RPG_Common RPG_Map)
endif ()

set_property (TARGET ${EXECUTABLE_3} PROPERTY FOLDER "test_u")

# add PCH
target_precompile_headers (${EXECUTABLE_3}
                           REUSE_FROM ${EXECUTABLE})

#----------- install files ---------------
install (TARGETS ${EXECUTABLE_3}
         RUNTIME DESTINATION bin)

########### launcher ###############
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
                        ARGS "-t"
                        FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${LIB_ROOT}/zlib/build/Debug
                                             ${LIB_ROOT}/xerces-c/build/msvc/src/Debug
                                             ${LIB_ROOT}/libpng/build/Debug
                                             ${LIB_ROOT}/ACE_TAO/ACE/lib
                                             ${SDL_LIB_PATH}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                        ENVIRONMENT foo=bar)
if (UNIX)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_2}" COMMAND)
elseif (WIN32)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_2}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
endif ()
create_target_launcher (${EXECUTABLE_2}
                        COMMAND ${COMMAND}
                        ARGS "-l -t"
                        FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${LIB_ROOT}/zlib/build/Debug
                                             ${LIB_ROOT}/xerces-c/build/msvc/src/Debug
                                             ${LIB_ROOT}/libpng/build/Debug
                                             ${LIB_ROOT}/ACE_TAO/ACE/lib
                                             ${SDL_LIB_PATH}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                        ENVIRONMENT foo=bar)
if (UNIX)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_3}" COMMAND)
elseif (WIN32)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_3}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
endif ()
create_target_launcher (${EXECUTABLE_3}
                        COMMAND ${COMMAND}
                        ARGS "-t"
                        FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${LIB_ROOT}/zlib/build/Debug
                                             ${LIB_ROOT}/xerces-c/build/msvc/src/Debug
                                             ${LIB_ROOT}/libpng/build/Debug
                                             ${LIB_ROOT}/ACE_TAO/ACE/lib
                                             ${SDL_LIB_PATH}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                        ENVIRONMENT foo=bar)
