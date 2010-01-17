METASOURCES = AUTO

include_HEADERS = rpg_magic_XML_parser.h rpg_magic_XML_types.h \
	rpg_magic_abilityclass.h rpg_magic_abilitytype.h rpg_magic_common.h rpg_magic_common_tools.h \
	rpg_magic_domain.h rpg_magic_incl.h rpg_magic_school.h rpg_magic_spell.h \
	rpg_magic_spellproperties.h

lib_LTLIBRARIES = libRPG_Magic.la

libRPG_Magic_la_LDFLAGS = -L/usr/local/lib
libRPG_Magic_la_SOURCES = rpg_XMLSchema_XML_types.h rpg_magic_XML_parser.cpp \
	rpg_magic_XML_parser.h rpg_magic_XML_types.cpp rpg_magic_XML_types.h rpg_magic_abilityclass.h \
	rpg_magic_abilitytype.h rpg_magic_common.h rpg_magic_common_tools.cpp rpg_magic_common_tools.h \
	rpg_magic_domain.h rpg_magic_incl.h rpg_magic_school.h rpg_magic_spell.h \
	rpg_magic_spellproperties.h

bindata_DATA = rpg_magic.xsd
bindatadir = $(bindir)
