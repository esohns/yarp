project(XML2CppCode)

cmake_minimum_required(VERSION 2.8)

#find_package(ACE REQUIRED)
#find_package(pthread REQUIRED)
#find_package(xerces-c REQUIRED)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/XML2CppCode-config.h)

########### next target ###############
set(XML2CppCode_SRCS
    XML_handler.cpp
    XML_handler.h
    XML_handler_base.cpp
    XML_handler_base.h
    XML_parser.cpp
    XML_parser.h
    handle_xmlenumeration.cpp
    handle_xmlsequence.cpp
    handle_xmlsequence.h
    handle_xmlunion.cpp
    handle_xmlunion.h
    iXML_definition_handler.h
    main.cpp
    xml2cppcode_common_tools.cpp
    xml2cppcode_common_tools.h)

add_definitions(-DHAVE_CONFIG_H)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(XML2CppCode ${XML2CppCode_SRCS})

target_link_libraries(XML2CppCode ACE pthread xerces-c)

install(TARGETS XML2CppCode RUNTIME DESTINATION bin)

########### install files ###############

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#bin_PROGRAMS = XML2CppCode
#
#XML2CppCode_SOURCES = XML_handler.cpp XML_handler.h XML_handler_base.cpp \
#	XML_handler_base.h XML_parser.cpp XML_parser.h handle_xmlenumeration.cpp \
#	handle_xmlsequence.cpp handle_xmlsequence.h handle_xmlunion.cpp handle_xmlunion.h \
#	iXML_definition_handler.h main.cpp xml2cppcode_common_tools.cpp xml2cppcode_common_tools.h
#XML2CppCode_LDADD = -lACE -lpthread -lxerces-c
