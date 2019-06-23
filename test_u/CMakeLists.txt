cmake_minimum_required(VERSION 2.8)

#project(test_u CXX)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/Configuration.cmake)
include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/Macros.cmake)
include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/VersionConfig.cmake)

if (UNIX)
 set (NEWLINE_STYLE UNIX)
elseif (WIN32)
 set (NEWLINE_STYLE WIN32)
else ()
 set (NEWLINE_STYLE UNIX)
endif ()
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake.in
                ${CMAKE_CURRENT_BINARY_DIR}/rpg_config.h
                @ONLY
                NEWLINE_STYLE ${NEWLINE_STYLE})

add_definitions(-DHAVE_CONFIG_H)
# include rpg_config.h
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(chance)
add_subdirectory(character)
add_subdirectory(combat)
add_subdirectory(graphics)
add_subdirectory(client)
add_subdirectory(item)
add_subdirectory(magic)
add_subdirectory(map)
add_subdirectory(net)
add_subdirectory(sound)
