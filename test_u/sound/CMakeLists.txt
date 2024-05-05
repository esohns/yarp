include (${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/xerces-c.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../scripts/sdl.cmake)

if (UNIX)
 include (FindPkgConfig)
 pkg_check_modules (PKG_CONFIG_MODULE_XERCES REQUIRED xerces-c)
endif (UNIX)

if (XERCESC_SUPPORT)
 include_directories (${XERCESC_INCLUDE_DIRS})
endif (XERCESC_SUPPORT)
if (SDL_SUPPORT)
 include_directories (${SDL_INCLUDE_DIRS})
endif (SDL_SUPPORT)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/log)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/timer)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../common)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../sound)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../engine)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############
set (EXECUTABLE sound_parser)
set (${EXECUTABLE}_SRCS
     sound_parser.cpp
     stdafx.h)

#set_source_files_properties (${${EXECUTABLE}_SRCS} PROPERTIES COMPILE_DEFINITIONS "BASEDIR=\"C:\\\\\";DATADIR=\"/usr/local/share/RPG\"")

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})

if (SDL_SUPPORT)
 target_link_libraries (${EXECUTABLE}
                        ${SDL_LIBRARIES})
endif (SDL_SUPPORT)
if (UNIX)
 target_link_libraries(${EXECUTABLE}
                       ${PKG_CONFIG_MODULE_XERCES_LIBRARIES}
                       xerces-c
                       ${ACE_LIBRARY}
                       Common CommonLog CommonTimer
                       RPG_Dice RPG_Common RPG_Sound)
elseif (WIN32)
# set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

 target_link_libraries (${EXECUTABLE}
                       legacy_stdio_definitions.lib
                       ${XERCESC_LIBRARIES}
                       ${ACE_LIBRARY}
                       Common CommonError CommonLog CommonTimer
                       RPG_Dice RPG_Common RPG_Sound)
endif ()

# add PCH
target_precompile_headers (${EXECUTABLE}
                           PRIVATE stdafx.h)

set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

########### install files ###############
install(TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)

########### launcher ###############
file (TO_CMAKE_PATH "$ENV{LIB_ROOT}" LIB_ROOT)
file (TO_CMAKE_PATH "${SDL_LIB_DIR}" SDL_LIB_PATH)
if (UNIX)
 create_target_launcher (${EXECUTABLE}
                         ARGS "-d -l -t"
                         FORWARD_ARGS
#                         RUNTIME_LIBRARY_DIRS ${LIB_ROOT}/ACE_TAO/ACE/lib
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                         ENVIRONMENT foo=bar)
elseif (WIN32)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
 create_target_launcher (${EXECUTABLE}
                         COMMAND ${COMMAND}
                         ARGS "-d -l -r -t"
                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${LIB_ROOT}/zlib/build/Debug
#                                              ${LIB_ROOT}/libpng/build/Debug
                                              ${XERCESC_LIB_DIR}
                                              ${LIB_ROOT}/ACE_TAO/ACE/lib
                                              ${SDL_LIB_PATH}
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                         ENVIRONMENT foo=bar)
endif ()
