METASOURCES = AUTO

ACLOCAL_AMFLAGS = `-I m4'

INCLUDES = -I$(top_srcdir)/.

noinst_HEADERS = rpg_XMLSchema.h

include_HEADERS = rpg_common_XML_parser.h rpg_common_XML_parser_base.h rpg_common_XML_types.h \
	rpg_common_attribute.h rpg_common_incl.h rpg_common_savingthrow.h \
	rpg_common_savingthrowunion.h rpg_common_tools.h

lib_LTLIBRARIES = libRPG_Common.la
libRPG_Common_la_SOURCES = rpg_XMLSchema.h rpg_common_XML_parser.cpp \
	rpg_common_XML_parser.h rpg_common_XML_parser_base.cpp rpg_common_XML_parser_base.h \
	rpg_common_XML_types.cpp rpg_common_XML_types.h rpg_common_attribute.h rpg_common_incl.h \
	rpg_common_savingthrow.h rpg_common_savingthrowunion.h rpg_common_tools.cpp rpg_common_tools.h
libRPG_Common_la_LDFLAGS = -L/usr/local/lib
