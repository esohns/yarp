include (${CMAKE_CURRENT_SOURCE_DIR}/scripts/VersionConfig.cmake)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/modules/Common/scripts;${CMAKE_CURRENT_SOURCE_DIR}/../Common/scripts;${CMAKE_CURRENT_SOURCE_DIR}/scripts")
include (Configuration)
include (Macros)
include (OS)
include (compiler_init)

cmake_minimum_required (VERSION 2.8)

include (PrePreToolChain)

# *NOTE*: sets CMAKE_TOOLCHAIN_FILE, so include BEFORE project()
include (vcpkg)

project (yarp CXX)

include (PreToolChain)
include (SetToolChain)
include (ToolChain)
# *NOTE*: or use the cmake.exe '-T' option instead
include (SetTargetPlatform)
include (TargetPlatform)

# make relative paths absolute (needed later on)
foreach (path BIN CMAKE SHARE INCLUDE LIB)
 set (var INSTALL_${path}_DIR)
# if (NOT IS_ABSOLUTE "${${var}}")
 string (TOLOWER "${path}" RELATIVE_INSTALL_PATH)

 if (UNIX)
  set (${var} "${CMAKE_INSTALL_PREFIX}/${RELATIVE_INSTALL_PATH}")
 elseif (WIN32)
  set (${var} ${RELATIVE_INSTALL_PATH})
 endif ()
# message (STATUS "set \"${var}\" to \"${${var}}\"")
endforeach ()

#extract package version from VCS
# find_package(Subversion)
# if(SUBVERSION_FOUND)
#   Subversion_WC_INFO(${CMAKE_CURRENT_SOURCE_DIR} ER)
#   if(${Subversion_svn_info_result} EQUAL 0)
#     set(SUBVERSION_REVISION ${ER_WC_REVISION})
#   endif()
# endif()
find_package (Git)
if (GIT_FOUND)
 set (GIT_REVISION "539")
endif ()

# if(${SUBVERSION_REVISION})
#   message(STATUS "found subversion revision ${SUBVERSION_REVISION}...")
#   set(PACKAGE_REVISION ${SUBVERSION_REVISION})
# elseif(${GIT_REVISION})
if (${GIT_REVISION})
 message (STATUS "found git revision ${GIT_REVISION}...")
 set (PACKAGE_REVISION ${GIT_REVISION})
endif (${GIT_REVISION})

set (CPACK_PACKAGE_DESCRIPTION "(yet another) role-playing game system framework")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "A RPG system framework")
set (CPACK_PACKAGE_NAME "Yarp")
#set (CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.6), libgcc1 (>= 1:4.1)")

#set (CPACK_PACKAGE_CONTACT "Erik Sohns <erik.sohns@posteo.de>")
#set (CPACK_PACKAGE_VENDOR "Erik Sohns")
#set (CPACK_PACKAGE_VERSION_MAJOR "0")
#set (CPACK_PACKAGE_VERSION_MINOR "1")
#set (CPACK_PACKAGE_VERSION_PATCH ${PACKAGE_REVISION})
# set(CPACK_PACKAGE_VERSION_PATCH "0")
#set (VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

#message (STATUS "configuring ${CPACK_PACKAGE_NAME} version ${VERSION}...")

#set (CPACK_GENERATOR "DEB;RPM;")
#set (CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}-${CMAKE_SYSTEM_PROCESSOR}")

include (CPack)

option (DOXYGEN_SUPPORT "generate documentation" OFF)
if (DOXYGEN_SUPPORT)
 find_package (Doxygen)
 if (DOXYGEN_FOUND STREQUAL "NO")
  message(FATAL_ERROR "Doxygen not found. Please get a copy http://www.doxygen.org")
 endif (DOXYGEN_FOUND STREQUAL "NO")
 configure_file(${CMAKE_CURRENT_SOURCE_DIR}/scripts/Doxyfile.cmake ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

 add_custom_target (doxygen
                    ${DOXYGEN_EXECUTABLE}
                    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

 set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES api-doc)

 get_target_property(DOC_TARGET doc TYPE)
 if(NOT DOC_TARGET)
  add_custom_target (doc)
 endif (NOT DOC_TARGET)
 add_dependencies (doc doxygen)
endif (DOXYGEN_SUPPORT)

#Use solution folders.
set_property (GLOBAL PROPERTY USE_FOLDERS ON)

add_compile_options ($<$<CONFIG:Debug>:-D_DEBUG>)
add_compile_options ($<$<CONFIG:Release>:-DACE_NDEBUG>)
if (CMAKE_BUILD_TYPE STREQUAL Debug)
 if (CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options (-g)
  add_compile_options (-O0)
  add_compile_options (-fno-inline)
 endif (CMAKE_COMPILER_IS_GNUCXX)
elseif (CMAKE_BUILD_TYPE STREQUAL Release)
 if (CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options (-O3)
 endif (CMAKE_COMPILER_IS_GNUCXX)
endif ()
if (MSVC)
 remove_definitions (/DWINVER)
 remove_definitions (/D_WIN32_WINNT)
endif (MSVC)

if (UNIX)
 if (CMAKE_COMPILER_IS_GNUCC)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
  set (CMAKE_EXE_LINKER_FLAGS "-Wl,-z,multidefs")
  set (CMAKE_STATIC_LINKER_FLAGS "-Wl,-z,multidefs")
 endif (CMAKE_COMPILER_IS_GNUCC)
endif (UNIX)

option (TRACING "enable tracing" OFF)
if (TRACING)
 add_definitions (-DCOMMON_NTRACE=0)
#  set_property (SOURCE PROPERTY COMPILE_DEFINITIONS COMMON_NTRACE=0)
else ()
 add_definitions (-DCOMMON_NTRACE=1)
#  set_property (SOURCE PROPERTY COMPILE_DEFINITIONS COMMON_NTRACE=1)
endif ()

include (valgrind)

if (UNIX)
 if (CMAKE_COMPILER_IS_GNUCXX)
#  add_compile_options (-ansi)
#  add_compile_options (-std=c++11)
  add_compile_options (-Wall -Wno-deprecated-declarations)
  set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS} -g")
  add_definitions (-D_GNU_SOURCE)
#  set_property (SOURCE PROPERTY COMPILE_DEFINITIONS _GNU_SOURCE)
 endif (CMAKE_COMPILER_IS_GNUCXX)
elseif (WIN32)
 if (MSVC)
  add_compile_options (/MP)
  add_definitions (-D_CRT_SECURE_NO_WARNINGS)
  add_definitions (-D_WINSOCK_DEPRECATED_NO_WARNINGS)

  set (MSVC_INCREMENTAL_YES_FLAG "/INCREMENTAL:NO")
  foreach (FLAG_TYPE EXE MODULE SHARED STATIC)
   string (REGEX REPLACE "/INCREMENTAL* " "/INCREMENTAL:NO " CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG "${CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG}")
   string (REPLACE "/EDITANDCONTINUE"     ""                 CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG "${CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG}")
   set (CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG "${CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG}"
        CACHE STRING "Overriding default debug ${FLAG_TYPE} linker flags." FORCE)
   mark_as_advanced (CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG)
   string (REGEX REPLACE "/INCREMENTAL* " "/INCREMENTAL:NO " CMAKE_${FLAG_TYPE}_LINKER_FLAGS_RELEASE "${CMAKE_${FLAG_TYPE}_LINKER_FLAGS_RELEASE}")
   mark_as_advanced (CMAKE_${FLAG_TYPE}_LINKER_FLAGS_RELEASE)
  endforeach ()
 endif (MSVC)
endif ()

set (LLVM_COMPILER_ID_STRING "Clang")
string (TOLOWER "${LLVM_COMPILER_ID_STRING}" LLVM_COMPILER_ID_STRING_LOWER)
string (TOLOWER "${CMAKE_CXX_COMPILER_ID}" CXX_COMPILER_ID_STRING_LOWER)
if (CXX_COMPILER_ID_STRING_LOWER STREQUAL LLVM_COMPILER_ID_STRING_LOWER)
 add_compile_options (-fPIC)
# add_definitions (-D_GLIBCXX_USE_CXX11_ABI=0)
endif ()

if (UNIX)
 set (NEWLINE_STYLE UNIX)
elseif (WIN32)
 set (NEWLINE_STYLE WIN32)
else ()
 set (NEWLINE_STYLE UNIX)
endif ()
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/scripts/config.h.cmake.in
                ${CMAKE_CURRENT_BINARY_DIR}/rpg_config.h
                @ONLY
                NEWLINE_STYLE ${NEWLINE_STYLE})
add_definitions (-DHAVE_CONFIG_H)
# include rpg_config.h
include_directories (${CMAKE_CURRENT_BINARY_DIR})

string (REPLACE ${CMAKE_SOURCE_DIR} "" BUILD_PATH_SUFFIX ${CMAKE_BINARY_DIR})
string (SUBSTRING ${BUILD_PATH_SUFFIX} 1 -1 BUILD_PATH_SUFFIX)
find_package (Common
              REQUIRED
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/modules ${CMAKE_CURRENT_SOURCE_DIR}/..
              PATH_SUFFIXES Common/${BUILD_PATH_SUFFIX}
              NO_DEFAULT_PATH)
if (NOT EXISTS ${Common_DIR})
 message (FATAL_ERROR "could not find package \"Common\", aborting")
else ()
 message (STATUS "found package \"Common\": \"${Common_DIR}\"")
endif ()

include (libraries_init)

add_subdirectory (chance)
add_subdirectory (common)
add_subdirectory (magic)
add_subdirectory (item)
add_subdirectory (character)
add_subdirectory (combat)
add_subdirectory (map)
add_subdirectory (graphics)
add_subdirectory (sound)
add_subdirectory (engine)
add_subdirectory (net)
add_subdirectory (client)
add_subdirectory (test_u)
#add_subdirectory (tools)

set (DOC_FILES COPYING INSTALL AUTHORS NEWS README TODO)
set (DOC_PATH "share/doc/${CPACK_PACKAGE_NAME}")
install (FILES
         ${DOC_FILES}
         DESTINATION ${DOC_PATH})

option (INSTALL_DOCUMENTATION "install generated documentation" OFF)
if (INSTALL_DOCUMENTATION)
  install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/api-doc/html
           DESTINATION ${DOC_PATH})
  install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/api-doc/man/man3
           DESTINATION share/man/man3)
else (INSTALL_DOCUMENTATION)
  message (STATUS "documentation will NOT be installed...")
endif (INSTALL_DOCUMENTATION)
