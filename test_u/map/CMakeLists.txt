if (UNIX)
 include (FindPkgConfig)
 pkg_check_modules (PKG_CONFIG_MODULE_SDL REQUIRED sdl)
 pkg_check_modules (PKG_CONFIG_MODULE_SDL_TTF REQUIRED SDL_ttf)
 pkg_check_modules (PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
elseif (WIN32)
endif ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/log)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../common)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../map)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../engine)

############## next target ###############
set (EXECUTABLE map_parser)
set (${EXECUTABLE}_SRCS
     map_parser.cpp)

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})

add_dependencies (${EXECUTABLE} RPG_Dice RPG_Common RPG_Map)

target_link_libraries (${EXECUTABLE}
                       pthread xerces-c
                       ACE
                       RPG_Dice RPG_Common RPG_Map)

#----------- install files ---------------
install (TARGETS ${EXECUTABLE}
         RUNTIME DESTINATION bin)

########### next target ###############
set (EXECUTABLE map_generator)
set (${EXECUTABLE}_SRCS
     map_generator.cpp)

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

#----------- install files ---------------
install (TARGETS ${EXECUTABLE}
         RUNTIME DESTINATION bin)

########### next target ###############
set (EXECUTABLE path_finder)
set (${EXECUTABLE}_SRCS
     path_finder.cpp)

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})

add_dependencies (${EXECUTABLE} RPG_Dice RPG_Common RPG_Map)

target_link_libraries (${EXECUTABLE}
                       pthread xerces-c
                       ACE 
                       RPG_Dice RPG_Common RPG_Map)

#----------- install files ---------------
install (TARGETS ${EXECUTABLE}
         RUNTIME DESTINATION bin)
