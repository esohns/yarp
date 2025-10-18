set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts")
include (Threads)

include (ACE)

option (BUILD_TEST_U "build test_u" OFF)

# user interface
#option (GUI_SUPPORT "enable graphical user interface support" ON)
#if (GUI_SUPPORT)
# add_definitions (-DGUI_SUPPORT)
 set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src/ui/scripts")
 include (PreUIFramework)
 include (SetUIFramework)
 if (GTK_SUPPORT)
  include (SetGTKVersion)
 endif (GTK_SUPPORT)
#endif (GUI_SUPPORT)

string (REPLACE ${CMAKE_SOURCE_DIR} "" BUILD_PATH_SUFFIX ${CMAKE_BINARY_DIR})
string (SUBSTRING ${BUILD_PATH_SUFFIX} 1 -1 BUILD_PATH_SUFFIX)
find_package (Common
              REQUIRED
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../modules ${CMAKE_CURRENT_SOURCE_DIR}/../..
              PATH_SUFFIXES Common/${BUILD_PATH_SUFFIX}
              NO_DEFAULT_PATH)
if (NOT EXISTS ${Common_DIR})
 message (FATAL_ERROR "could not find package \"Common\", aborting")
endif ()

find_package (ACEStream
              REQUIRED
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../modules ${CMAKE_CURRENT_SOURCE_DIR}/../..
              PATH_SUFFIXES ACEStream/${BUILD_PATH_SUFFIX}
              NO_DEFAULT_PATH)
if (NOT EXISTS ${ACEStream_DIR})
 message (FATAL_ERROR "could not find package \"ACEStream\", aborting")
endif ()

find_package (ACENetwork
              REQUIRED
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../modules ${CMAKE_CURRENT_SOURCE_DIR}/../..
              PATH_SUFFIXES ACENetwork/${BUILD_PATH_SUFFIX}
              NO_DEFAULT_PATH)
if (NOT EXISTS ${ACENetwork_DIR})
 message (FATAL_ERROR "could not find package \"ACENetwork\", aborting")
endif ()

file (TO_CMAKE_PATH "$ENV{LIB_ROOT}/cmake-modules" CMAKE_MODULES_PATH)
file (TO_CMAKE_PATH "$ENV{LIB_ROOT}/CMakeHelpers" CMAKE_HELPERS_PATH)
list (APPEND CMAKE_MODULE_PATH
      "${CMAKE_MODULES_PATH}"
      "${CMAKE_HELPERS_PATH}")
#include (UseBackportedModules)
include (CreateLaunchers)

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
