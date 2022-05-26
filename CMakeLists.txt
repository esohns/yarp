cmake_minimum_required (VERSION 2.8)

project (Yarp CXX)

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
endif ()

set(CPACK_PACKAGE_DESCRIPTION "(yet another) role-playing game system framework")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A RPG system framework")
set(CPACK_PACKAGE_NAME "Yarp")
#set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.6), libgcc1 (>= 1:4.1)")

set(CPACK_PACKAGE_CONTACT "Erik Sohns <erik.sohns@web.de>")
#set(CPACK_PACKAGE_VENDOR "Erik Sohns")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH ${PACKAGE_REVISION})
# set(CPACK_PACKAGE_VERSION_PATCH "0")
set(VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

message(STATUS "configuring ${CPACK_PACKAGE_NAME} version ${VERSION}...")

set(CPACK_GENERATOR "DEB;RPM;")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}-${CMAKE_SYSTEM_PROCESSOR}")

include(CPack)

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

#list (APPEND CMAKE_MODULE_PATH "$ENV{LIB_ROOT}/cotire/CMake")
#list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/scripts")
#cmake_policy (SET CMP0011 NEW)
#include (cotire)
#set_directory_properties (PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)

option (DEBUG_DEBUGGER "debug using IDE" OFF)
if (DEBUG_DEBUGGER)
 add_definitions (-DDEBUG_DEBUGGER)
endif (DEBUG_DEBUGGER)

if (UNIX)
 if (CMAKE_COMPILER_IS_GNUCC)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
  set (CMAKE_EXE_LINKER_FLAGS "-Wl,-z,multidefs")
  set (CMAKE_STATIC_LINKER_FLAGS "-Wl,-z,multidefs")
 endif (CMAKE_COMPILER_IS_GNUCC)
endif (UNIX)

include (${CMAKE_CURRENT_SOURCE_DIR}/../Common/scripts/Configuration.cmake)
include (${CMAKE_CURRENT_SOURCE_DIR}/../Common/scripts/Macros.cmake)
include (${CMAKE_CURRENT_SOURCE_DIR}/scripts/VersionConfig.cmake)

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

#add_subdirectory(../tools tools)
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

set (DOC_FILES COPYING INSTALL AUTHORS NEWS README TODO)
set (DOC_PATH "share/doc/${CPACK_PACKAGE_NAME}")
install (FILES ${DOC_FILES}
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
