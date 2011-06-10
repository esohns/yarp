cmake_minimum_required(VERSION 2.8)

project(Yarp CXX)

#extract package version from VCS
# find_package(Subversion)
# if(SUBVERSION_FOUND)
#   Subversion_WC_INFO(${CMAKE_CURRENT_SOURCE_DIR} ER)
#   if(${Subversion_svn_info_result} EQUAL 0)
#     set(SUBVERSION_REVISION ${ER_WC_REVISION})
#   endif()
# endif()
find_package(Git)
if(GIT_FOUND)
  set(GIT_REVISION "539")
endif()

# if(${SUBVERSION_REVISION})
#   message(STATUS "found subversion revision ${SUBVERSION_REVISION}...")
#   set(PACKAGE_REVISION ${SUBVERSION_REVISION})
# elseif(${GIT_REVISION})
if(${GIT_REVISION})
  message(STATUS "found git revision ${GIT_REVISION}...")
  set(PACKAGE_REVISION ${GIT_REVISION})
endif()

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

find_package(Doxygen)
if (DOXYGEN_FOUND STREQUAL "NO")
  message(FATAL_ERROR "Doxygen not found. Please get a copy http://www.doxygen.org")
endif (DOXYGEN_FOUND STREQUAL "NO")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)

add_custom_target(doxygen ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES api-doc)

get_target_property(DOC_TARGET doc TYPE)
if(NOT DOC_TARGET)
  add_custom_target(doc)
endif()
add_dependencies(doc doxygen)

add_subdirectory(../chance chance)
add_subdirectory(../common common)
add_subdirectory(../magic magic)
add_subdirectory(../item item)
add_subdirectory(../character character)
add_subdirectory(../monster monster)
add_subdirectory(../combat combat)
add_subdirectory(../map map)
add_subdirectory(../graphics graphics)
add_subdirectory(../sound sound)
add_subdirectory(../engine engine)
add_subdirectory(../stream stream)
add_subdirectory(../net net)
add_subdirectory(../client client)
add_subdirectory(../test_u test_u)

set(DOC_FILES COPYING INSTALL AUTHORS NEWS README TODO)
set(DOC_PATH "share/doc/${CPACK_PACKAGE_NAME}")
install(FILES ${DOC_FILES}
        DESTINATION ${DOC_PATH})

option(INSTALL_DOCUMENTATION "install generated documentation" OFF)
if(INSTALL_DOCUMENTATION)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/api-doc/html
          DESTINATION ${DOC_PATH})
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/api-doc/man/man3
          DESTINATION share/man/man3)
else(INSTALL_DOCUMENTATION)
  message(STATUS "documentation will NOT be installed...")
endif(INSTALL_DOCUMENTATION)

########### install files ###############

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#SUBDIRS = chance common magic item character monster combat map graphics sound engine stream net client test_u
#
#EXTRA_DIST = Yarp.spec
