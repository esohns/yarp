project(XML2CppCode)

cmake_minimum_required(VERSION 2.8)

#find_package(ACE REQUIRED)
#find_package(pthread REQUIRED)
#find_package(xerces-c REQUIRED)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/XML2CppCode-config.h)

########### next target ###############
set(XML2CppCode_SRCS
    XML_handler.cpp
    XML_handler.h
    XML_handler_base.cpp
    XML_handler_base.h
    XML_parser.cpp
    XML_parser.h
    handle_xmlchoice.cpp
    handle_xmlchoice.h
    handle_xmlenumeration.cpp
    handle_xmlenumeration.h
    handle_xmlsequence.cpp
    handle_xmlsequence.h
    handle_xmlstruct.cpp
    handle_xmlstruct.h
    handle_xmlunion.cpp
    handle_xmlunion.h
    iXML_definition_handler.h
    main.cpp
    xml2cppcode_common_tools.cpp
    xml2cppcode_common_tools.h)

add_definitions(-DHAVE_CONFIG_H)

if (UNIX)
elseif (WIN32)
 include_directories ($ENV{LIB_ROOT}/xerces-c/build/src)
 include_directories ($ENV{LIB_ROOT}/xerces-c/src)
endif ()

if (UNIX)
 include_directories (
#                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE>)
#                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ACE_TAO/ACE>)
                      /usr/local/src/ACE_wrappers)
elseif (WIN32)
 if (DEFINED ENV{ACE_ROOT})
  include_directories ($ENV{ACE_ROOT})
 else ()
  include_directories ($ENV{LIB_ROOT}/ACE_TAO/ACE)
 endif (DEFINED ENV{ACE_ROOT})
endif ()

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(XML2CppCode ${XML2CppCode_SRCS})

target_link_libraries(XML2CppCode
                      $ENV{LIB_ROOT}/ACE_TAO/ACE/lib/ACEd.lib
                      $ENV{LIB_ROOT}/xerces-c/build/src/Debug/xerces-c_3D.lib)

########### install files ###############
install(TARGETS XML2CppCode RUNTIME DESTINATION bin)

