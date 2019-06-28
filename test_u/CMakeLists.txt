include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/Threads.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/ACE.cmake)

option (BUILD_TEST_U "build test_u" OFF)

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

add_subdirectory (chance)
add_subdirectory (character)
add_subdirectory (client)
add_subdirectory (combat)
add_subdirectory (graphics)
add_subdirectory (item)
add_subdirectory (magic)
add_subdirectory (map)
add_subdirectory (net)
add_subdirectory (sound)

# phonies
add_custom_target (test_u ALL
                   COMMENT "building test_u...")
add_dependencies (test_u chance_dice chance_rangeToRoll)
add_dependencies (test_u character_parser character_generator character_generator_gui monster_parser)
add_dependencies (test_u client_gui)
add_dependencies (test_u combat_simulator)
add_dependencies (test_u graphics_parser SDL_gui)
add_dependencies (test_u item_parser)
add_dependencies (test_u magic_parser)
add_dependencies (test_u map_parser map_generator path_finder)
add_dependencies (test_u net_client net_server)
add_dependencies (test_u sound_parser)

if (NOT BUILD_TEST_U)
 set_target_properties (test_u PROPERTIES EXCLUDE_FROM_ALL 1)
 cmake_policy (SET CMP0012 NEW)
 if (WIN32)
  set_target_properties (test_u PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD TRUE)
 endif (WIN32)
endif ()

# ----------------------------------------

#include (CreateLaunchers)

# enable (unit/integration) tests
#include (CTest)
enable_testing ()
