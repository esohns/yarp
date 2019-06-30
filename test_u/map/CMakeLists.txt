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

add_dependencies (${EXECUTABLE} RPG_Dice RPG_Common RPG_Map)

target_link_libraries (${EXECUTABLE}
                       pthread xerces-c
                       ACE
                       RPG_Dice RPG_Common RPG_Map)

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

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})

add_dependencies (${EXECUTABLE}
                  RPG_Dice RPG_Common RPG_Map RPG_Engine)

target_link_libraries (${EXECUTABLE}
                       ${PKG_CONFIG_MODULE_SDL_LIBRARIES}
                       ${PKG_CONFIG_MODULE_SDL_TTF_LIBRARIES}
                       ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES})
target_link_libraries (${EXECUTABLE}
                       pthread xerces-c
                       ACE
                       RPG_Chance RPG_Dice RPG_Common RPG_Map RPG_Character RPG_Magic RPG_Item
                       RPG_Monster RPG_Player RPG_Combat RPG_Graphics RPG_Engine)

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

set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

target_link_libraries (${EXECUTABLE}
                       Userenv.lib
#                       xerces-c
                       $ENV{LIB_ROOT}/ACE_TAO/ACE/lib/ACEd.lib
                       Common CommonError CommonLog CommonTimer
                       RPG_Dice RPG_Common RPG_Map)

set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

#----------- install files ---------------
install (TARGETS ${EXECUTABLE}
         RUNTIME DESTINATION bin)
