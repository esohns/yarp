if (UNIX)
 include(FindPkgConfig)
 pkg_check_modules(PKG_CONFIG_MODULE_SDL REQUIRED sdl)
 pkg_check_modules(PKG_CONFIG_MODULE_SDL_MIXER REQUIRED SDL_mixer)
 pkg_check_modules(PKG_CONFIG_MODULE_SDL_TTF REQUIRED SDL_ttf)
 pkg_check_modules(PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
 pkg_check_modules(PKG_CONFIG_MODULE_LIBGLADE REQUIRED libglade-2.0)
 pkg_check_modules(PKG_CONFIG_MODULE_LIBGNOMEUI REQUIRED libgnomeui-2.0>=1.110.0)
elseif (WIN32)
endif ()

if (UNIX)
 include_directories(${PKG_CONFIG_MODULE_SDL_INCLUDE_DIRS}
                     ${PKG_CONFIG_MODULE_SDL_MIXER_INCLUDE_DIRS}
                     ${PKG_CONFIG_MODULE_SDL_TTF_INCLUDE_DIRS}
                     ${PKG_CONFIG_MODULE_LIBPNG_INCLUDE_DIRS})
elseif (WIN32)
 include_directories($ENV{LIB_ROOT}/SDL/include
                     $ENV{LIB_ROOT}/SDL_mixer/include
                     $ENV{LIB_ROOT}/SDL_ttf/include
                     $ENV{LIB_ROOT}/gtk2/include)
endif ()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../chance/dice
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../common
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../item
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../character
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../magic
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../combat
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../character/player
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../character/monster
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../map
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../engine
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../client
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../graphics)

########### next target ###############
set (EXECUTABLE character_parser)
set (${EXECUTABLE}_SRCS
     character_parser.cpp)

set_source_files_properties (${${EXECUTABLE}_SRCS} PROPERTIES COMPILE_DEFINITIONS DATADIR="/usr/local/share/RPG/config")

# add_definitions(-DDATADIR="/usr/local/share/RPG/config")

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})

target_link_libraries (${EXECUTABLE}
                       pthread xerces-c
                       ${PKG_CONFIG_MODULE_SDL_LIBRARIES}
                       ${PKG_CONFIG_MODULE_SDL_TTF_LIBRARIES}
                       ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES})
target_link_libraries (${EXECUTABLE}
                       ACE
                       RPG_Dice RPG_Chance RPG_Common RPG_Magic RPG_Item
                       RPG_Character RPG_Monster RPG_Player RPG_Combat
                       RPG_Map RPG_Graphics RPG_Engine)

set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

########### install files ###############
install (TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)

########### next target ###############
set (EXECUTABLE character_generator)
set (${EXECUTABLE}_SRCS
     character_generator.cpp)

set_source_files_properties (${${EXECUTABLE}_SRCS} PROPERTIES COMPILE_DEFINITIONS BASEDIR="/usr/local/share/RPG/config")

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})

target_link_libraries (${EXECUTABLE}
                       xerces-c
                       ${PKG_CONFIG_MODULE_SDL_LIBRARIES}
                       ${PKG_CONFIG_MODULE_SDL_TTF_LIBRARIES}
                       ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES})
target_link_libraries (${EXECUTABLE}
                       ACE
                       RPG_Dice RPG_Chance RPG_Common RPG_Magic RPG_Item
                       RPG_Character RPG_Monster RPG_Player RPG_Combat
                       RPG_Map RPG_Graphics RPG_Engine)

set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

########### install files ###############
install (TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)

########### next target ###############
set (EXECUTABLE character_generator_gui)
set (${EXECUTABLE}_SRCS
     character_generator_gui.cpp
     character_generator_gui_callbacks.cpp
     character_generator_gui_callbacks.h
     character_generator_gui_common.h)

set_source_files_properties (${${EXECUTABLE}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"/usr/local/share/RPG\";CONFIGDIR=\".\"")

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})

include_directories (${PKG_CONFIG_MODULE_LIBGLADE_INCLUDE_DIRS}
                     ${PKG_CONFIG_MODULE_LIBGNOMEUI_INCLUDE_DIRS})

target_link_libraries (${EXECUTABLE}
                       xerces-c
                       ${PKG_CONFIG_MODULE_SDL_LIBRARIES}
                       ${PKG_CONFIG_MODULE_SDL_MIXER_LIBRARIES}
                       ${PKG_CONFIG_MODULE_SDL_TTF_LIBRARIES}
                       ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES}
                       ${PKG_CONFIG_MODULE_LIBGLADE_LIBRARIES}
                       ${PKG_CONFIG_MODULE_LIBGNOMEUI_LIBRARIES})
target_link_libraries (${EXECUTABLE}
                       ACE RPG_Dice RPG_Chance RPG_Common RPG_Magic RPG_Item
                       RPG_Character RPG_Monster RPG_Player RPG_Combat
                       RPG_Map RPG_Sound RPG_Graphics RPG_Engine RPG_Client)

set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

########### install files ###############
install (TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)
install (FILES ${EXECUTABLE}.glade DESTINATION share/RPG/config)
