if (UNIX)
 include (FindPkgConfig)
 pkg_check_modules (PKG_CONFIG_MODULE_SDL REQUIRED sdl)
 pkg_check_modules (PKG_CONFIG_MODULE_SDL_TTF REQUIRED SDL_ttf)
 pkg_check_modules (PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
elseif (WIN32)
endif ()

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
     map_parser.cpp)

set_source_files_properties (${${EXECUTABLE}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"C:\\\\\"")

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})

add_dependencies (${EXECUTABLE} RPG_Dice RPG_Chance RPG_Common RPG_Character RPG_Magic RPG_Item
                                RPG_Combat RPG_Player RPG_Monster RPG_Map RPG_Engine)

if (UNIX)
 target_link_libraries (${EXECUTABLE}
                        xerces-c
                        ACE
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
 set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

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

#----------- install files ---------------
install (TARGETS ${EXECUTABLE}
         RUNTIME DESTINATION bin)

########### next target ###############
set (EXECUTABLE map_generator)
set (${EXECUTABLE}_SRCS
     map_generator.cpp)

set_source_files_properties (${${EXECUTABLE}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"C:\\\\\"")

if (UNIX)
 include_directories (${PKG_CONFIG_MODULE_SDL_INCLUDE_DIRS}
                      ${PKG_CONFIG_MODULE_SDL_TTF_INCLUDE_DIRS}
                      ${PKG_CONFIG_MODULE_LIBPNG_INCLUDE_DIRS})
elseif (WIN32)
 include_directories ($ENV{LIB_ROOT}/SDL/include
                      $ENV{LIB_ROOT}/SDL_ttf/include
                      $ENV{LIB_ROOT}/libpng)
endif ()
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_party/xsd)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../graphics)

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})

add_dependencies (${EXECUTABLE}
                  RPG_Dice RPG_Common RPG_Character RPG_Magic RPG_Item
                  RPG_Monster RPG_Player RPG_Combat RPG_Map RPG_Graphics RPG_Engine)

if (UNIX)
 target_link_libraries (${EXECUTABLE}
                        ${PKG_CONFIG_MODULE_SDL_LIBRARIES}
                        ${PKG_CONFIG_MODULE_SDL_TTF_LIBRARIES}
                        ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES}
                        xerces-c
                        ACE
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
 set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

 target_link_libraries (${EXECUTABLE}
                        $ENV{LIB_ROOT}/SDL/lib/${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE}/SDLmain.lib
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

set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

#----------- install files ---------------
install (TARGETS ${EXECUTABLE}
         RUNTIME DESTINATION bin)

########### next target ###############
set (EXECUTABLE path_finder)
set (${EXECUTABLE}_SRCS
     path_finder.cpp)

set_source_files_properties (${${EXECUTABLE}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"C:\\\\\"")

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})

add_dependencies (${EXECUTABLE} RPG_Dice RPG_Common RPG_Map)

if (UNIX)
 target_link_libraries (${EXECUTABLE}
                        xerces-c
                        ACE
                        Common CommonError CommonLog CommonTimer
                        RPG_Dice RPG_Common RPG_Map)
elseif (WIN32)
 set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

 target_link_libraries (${EXECUTABLE}
                        Userenv.lib
                        $ENV{LIB_ROOT}/xerces-c/build/msvc/src/Debug/xerces-c_3D.lib
                        $ENV{LIB_ROOT}/ACE_TAO/ACE/lib/ACEd.lib
                        Common CommonError CommonLog CommonTimer
                        RPG_Dice RPG_Common RPG_Map)
endif ()

set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

#----------- install files ---------------
install (TARGETS ${EXECUTABLE}
         RUNTIME DESTINATION bin)
