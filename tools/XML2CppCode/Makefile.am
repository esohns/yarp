METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = XML2CppCode

XML2CppCode_SOURCES = XML_handler.cpp XML_handler.h XML_handler_base.cpp \
	XML_handler_base.h XML_parser.cpp XML_parser.h handle_xmlenumeration.cpp \
	handle_xmlsequence.cpp handle_xmlsequence.h handle_xmlunion.cpp handle_xmlunion.h \
	iXML_definition_handler.h main.cpp xml2cppcode_common_tools.cpp xml2cppcode_common_tools.h
XML2CppCode_LDADD = -lACE -lpthread -lxerces-c
