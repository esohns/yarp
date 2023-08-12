include (${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/sdl.cmake)

if (UNIX)
 include(FindPkgConfig)
 pkg_check_modules(PKG_CONFIG_MODULE_SDL_MIXER REQUIRED SDL_mixer)
elseif (WIN32)
# add_definitions (-D__IDataObject_INTERFACE_DEFINED__)
 add_definitions (-D_USE_MATH_DEFINES)
endif ()

if (SDL_SUPPORT)
 include_directories (${SDL_INCLUDE_DIRS})
endif (SDL_SUPPORT)
if (UNIX)
 include_directories (${PKG_CONFIG_MODULE_SDL_MIXER_INCLUDE_DIRS})
elseif (WIN32)
 include_directories ($ENV{LIB_ROOT}/SDL_mixer/include)
endif ()

if (WIN32)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/include)
endif (WIN32)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/image)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/log)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/timer)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui/gtk)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/3rd_party/tree)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../common
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../item
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../character
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../magic
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../combat
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../character/player
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../character/monster
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../map
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../engine
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../client
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../graphics
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../sound)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############
set (EXECUTABLE graphics_parser)
set (${EXECUTABLE}_SRCS
     graphics_parser.cpp
     stdafx.h)

if (UNIX)
 set_source_files_properties(${${EXECUTABLE}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"C:\\\\\";DATADIR=\"/usr/local/share/RPG/config\"")
elseif (WIN32)
 set_source_files_properties(${${EXECUTABLE}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"C:\\\\\"")
endif ()

add_executable(${EXECUTABLE} ${${EXECUTABLE}_SRCS})

if (SDL_SUPPORT)
 target_link_libraries (${EXECUTABLE}
                        ${SDL_LIBRARIES})
endif (SDL_SUPPORT)
if (UNIX)
 target_link_libraries(${EXECUTABLE}
                       xerces-c
                       ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES}
                       ${ACE_LIBRARY}
                       Common CommonLog CommonTimer
                       RPG_Dice RPG_Common RPG_Graphics)
elseif (WIN32)
# set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

 target_link_libraries (${EXECUTABLE}
                        $ENV{LIB_ROOT}/xerces-c/build/msvc/src/Debug/xerces-c_3D.lib
                        $ENV{LIB_ROOT}/ACE_TAO/ACE/lib/ACEd.lib
                        Common CommonLog CommonTimer
                        RPG_Dice RPG_Common RPG_Graphics)
endif ()

set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

# add PCH
target_precompile_headers (${EXECUTABLE}
                           PRIVATE stdafx.h)

install(TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)

########### next target ###############
set (EXECUTABLE_2 SDL_gui)
set (${EXECUTABLE_2}_SRCS
     SDL_gui.cpp
     SDL_gui_common.h
     SDL_gui_defines.h
     SDL_gui_levelwindow_3d.cpp
     SDL_gui_levelwindow_3d.h
     SDL_gui_levelwindow_isometric.cpp
     SDL_gui_levelwindow_isometric.h
     SDL_gui_mainwindow.cpp
     SDL_gui_mainwindow.h
     SDL_gui_minimapwindow.cpp
     SDL_gui_minimapwindow.h
     stdafx.h)

if (UNIX)
 set_source_files_properties(${${EXECUTABLE_2}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"C:\\\\\";DATADIR=\"/usr/local/share/RPG/config\"")
elseif (WIN32)
 set_source_files_properties(${${EXECUTABLE_2}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"C:\\\\\"")
endif ()

add_executable (${EXECUTABLE_2}
                ${${EXECUTABLE_2}_SRCS})

add_dependencies(${EXECUTABLE_2}
                 RPG_Dice RPG_Chance RPG_Common
                 RPG_Magic RPG_Item RPG_Character RPG_Monster RPG_Player RPG_Combat
                 RPG_Map RPG_Engine RPG_Sound RPG_Graphics RPG_Client)

if (SDL_SUPPORT)
 target_link_libraries (${EXECUTABLE_2}
                        ${SDL_LIBRARIES})
endif (SDL_SUPPORT)
if (UNIX)
 target_link_libraries (${EXECUTABLE_2}
                        ${PKG_CONFIG_MODULE_SDL_MIXER_LIBRARIES}
                        ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES}
#                        ${PKG_CONFIG_MODULE_LIBGLADE_LIBRARIES}
                        GLU
                        xerces-c
                        ${ACE_LIBRARY}
                        Common CommonLog CommonTimer CommonUI CommonUI_GTK
                        ACEStream ACEStream_Network
                        ACENetwork
                        RPG_Dice RPG_Chance RPG_Common
                        RPG_Character RPG_Magic RPG_Item
                        RPG_Monster RPG_Player RPG_Combat
                        RPG_Map RPG_Engine RPG_Net RPG_Net_Protocol
                        RPG_Sound RPG_Graphics RPG_Client
                        RPG_Character RPG_Magic RPG_Item
                        RPG_Dice RPG_Common RPG_Monster RPG_Map
                        RPG_Combat RPG_Chance RPG_Net RPG_Sound
                        RPG_Character RPG_Player
                        ACEStream_Network)
elseif (WIN32)
# set_target_properties (${EXECUTABLE_2} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

 target_link_libraries (${EXECUTABLE_2}
                        Opengl32.lib Glu32.lib Secur32.lib Userenv.lib
                        $ENV{LIB_ROOT}/SDL_mixer/lib/${CMAKE_VS_PLATFORM_TOOLSET_HOST_ARCHITECTURE}/SDL_mixer.lib
                        $ENV{LIB_ROOT}/xerces-c/build/msvc/src/Debug/xerces-c_3D.lib
                        $ENV{LIB_ROOT}/ACE_TAO/ACE/lib/ACEd.lib
                        Common CommonError CommonLog CommonTimer CommonUI CommonUI_GTK
                        ACEStream ACEStream_Network
                        ACENetwork
                        RPG_Dice RPG_Chance RPG_Common
                        RPG_Character RPG_Magic RPG_Item
                        RPG_Monster RPG_Player RPG_Combat
                        RPG_Map RPG_Engine RPG_Net RPG_Net_Protocol
                        RPG_Sound RPG_Graphics RPG_Client)
endif ()

set_property (TARGET ${EXECUTABLE_2} PROPERTY FOLDER "test_u")

# add PCH
target_precompile_headers (${EXECUTABLE_2}
                           REUSE_FROM ${EXECUTABLE})

########### install files ###############
install(TARGETS ${EXECUTABLE_2} RUNTIME DESTINATION bin)

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
                        ARGS "-d -t"
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
 file (TO_CMAKE_PATH "${GTK_LIB_DIRS}" GTK_LIB_PATH)
endif ()
create_target_launcher (${EXECUTABLE_2}
                        COMMAND ${COMMAND}
                        ARGS "-l -t"
                        FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${LIB_ROOT}/zlib/build/Debug
                                             ${LIB_ROOT}/libpng/build/Debug
                                             ${LIB_ROOT}/openssl
                                             ${SDL_LIB_PATH}
                                             ${LIB_ROOT}/xerces-c/build/msvc/src/Debug
                                             ${LIB_ROOT}/ACE_TAO/ACE/lib
                                             ${GTK_LIB_PATH}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                        ENVIRONMENT foo=bar)
