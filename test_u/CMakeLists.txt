include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/Threads.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/ACE.cmake)

option (BUILD_TEST_U "build test_u" OFF)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/Configuration.cmake)
include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/Macros.cmake)
include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/VersionConfig.cmake)

# user interface
#option (GUI_SUPPORT "enable graphical user interface support" ON)
#if (GUI_SUPPORT)
 add_definitions (-DGUI_SUPPORT)
 include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src/ui/scripts/PreUIFramework.cmake)
 include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src/ui/scripts/SetUIFramework.cmake)

 if (CURSES_USE)
 elseif (GTK_USE)
  include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src/ui/scripts/SetGTKVersion.cmake)

  if (CMAKE_BUILD_TYPE STREQUAL Debug)
   add_definitions (-DG_ERRORCHECK_MUTEXES)
  endif (CMAKE_BUILD_TYPE STREQUAL Debug)
 elseif (WXWIDGETS_USE)
  message (STATUS "wxWidgets_DEFINITIONS: ${wxWidgets_DEFINITIONS}")
  message (STATUS "wxWidgets_DEFINITIONS_DEBUG: ${wxWidgets_DEFINITIONS_DEBUG}")
  message (STATUS "wxWidgets_CXX_FLAGS: ${wxWidgets_CXX_FLAGS}")
  message (STATUS "wxWidgets_USE_FILE: ${wxWidgets_USE_FILE}")
# # *NOTE*: re-enable ANSI mode, iff available
# add_definitions (-DwxUSE_UNICODE=0)
  foreach (DEFINITION ${wxWidgets_DEFINITIONS})
   add_definitions (-D${DEFINITION})
  endforeach ()
  if (WIN32)
#  add_definitions (-D__WXMSW__)
#  add_definitions (-DWXUSINGDLL)
   if (MSVC)
    add_definitions (-DwxMSVC_VERSION_AUTO)
   endif (MSVC)
  endif (WIN32)
  add_definitions (-DwxUSE_GUI=1)
  if (CMAKE_BUILD_TYPE STREQUAL Debug)
#   add_definitions (-D__WXDEBUG__)
  endif (CMAKE_BUILD_TYPE STREQUAL Debug)
 endif ()
#endif (GUI_SUPPORT)

get_filename_component (BUILD_PATH ${CMAKE_CURRENT_BINARY_DIR}/../.. REALPATH DIRECTORY)
get_filename_component (BUILD_PATH_SUFFIX ${BUILD_PATH} NAME)
find_package (Common ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
              REQUIRED
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../modules ${CMAKE_CURRENT_SOURCE_DIR}/../..
              PATH_SUFFIXES Common/${BUILD_PATH_SUFFIX}
              NO_DEFAULT_PATH)
if (NOT EXISTS ${Common_DIR})
 message (FATAL_ERROR "could not find package \"Common\", aborting")
endif ()
add_definitions (-DCOMMON_HAS_DLL)

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
