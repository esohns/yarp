METASOURCES = AUTO

#ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -DACE_HAS_DLL

bin_PROGRAMS = XML2CppCode

XML2CppCode_SOURCES = \
  handle_xmlchoice.cpp \
  handle_xmlchoice.h \
  handle_xmlenumeration.cpp \
  handle_xmlenumeration.h \
  handle_xmlsequence.cpp \
  handle_xmlsequence.h \
  handle_xmlunion.cpp \
  handle_xmlunion.h \
  iXML_definition_handler.h \
  main.cpp \
  xml2cppcode_common_tools.cpp \
  xml2cppcode_common_tools.h \
  xml2cppcode.h \
  XML_handler.cpp \
  XML_handler.h \
  XML_handler_base.cpp \
  XML_handler_base.h \
  XML_parser.cpp \
  XML_parser.h \
  stdafx.cpp \
  stdafx.h
XML2CppCode_CPPFLAGS = $(AM_CPPFLAGS)
#XML2CppCode_LDFLAGS = -L/usr/local/lib
XML2CppCode_LDADD = -lACE -lpthread -lxerces-c

EXTRA_DIST = \
  configure.ac \
  m4/ax_prefix_config_h.m4

