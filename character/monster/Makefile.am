METASOURCES = AUTO

# set the include path found by configure
INCLUDES = -I$(top_srcdir)/monster

include_HEADERS = rpg_monster_XML_parser.h rpg_monster_XML_parser_base.h \
	rpg_monster_XML_types.h rpg_monster_advancement.h rpg_monster_advancementstep.h \
	rpg_monster_attack.h rpg_monster_attackaction.h rpg_monster_common.h \
	rpg_monster_common_tools.h rpg_monster_dictionary.h rpg_monster_incl.h rpg_monster_metatype.h \
	rpg_monster_naturalarmorclass.h rpg_monster_naturalweapon.h rpg_monster_organization.h \
	rpg_monster_organizations.h rpg_monster_organizationslaverstep.h \
	rpg_monster_organizationstep.h rpg_monster_savingthrowmodifiers.h rpg_monster_subtype.h \
	rpg_monster_type.h

lib_LTLIBRARIES = libRPG_Monster.la

libRPG_Monster_la_LDFLAGS = -L/usr/local/lib
libRPG_Monster_la_SOURCES = rpg_XMLSchema.h rpg_monster_XML_parser.cpp \
	rpg_monster_XML_parser.h rpg_monster_XML_parser_base.cpp rpg_monster_XML_parser_base.h \
	rpg_monster_XML_types.cpp rpg_monster_XML_types.h rpg_monster_advancement.h \
	rpg_monster_advancementstep.h rpg_monster_attack.h rpg_monster_attackaction.h \
	rpg_monster_common_tools.cpp rpg_monster_common_tools.h rpg_monster_dictionary.cpp \
	rpg_monster_dictionary.h rpg_monster_incl.h rpg_monster_metatype.h \
	rpg_monster_naturalarmorclass.h rpg_monster_naturalweapon.h rpg_monster_organization.h \
	rpg_monster_organizations.h rpg_monster_organizationslaverstep.h rpg_monster_organizationstep.h \
	rpg_monster_savingthrowmodifiers.h rpg_monster_subtype.h rpg_monster_type.h

bindata_DATA = rpg_monster.xsd rpg_monster.xml
bindatadir = $(bindir)
noinst_HEADERS = rpg_XMLSchema.h
monster_parser_LDFLAGS = -L/usr/local/lib
bin_PROGRAMS = monster_parser
monster_parser_SOURCES = monster_parser.cpp
monster_parser_LDADD = $(top_builddir)/monster/libRPG_Monster.la \
	$(top_builddir)/libRPG_Character.la -lRPG_Combat -lRPG_Item -lRPG_Chance -lRPG_Dice -lxerces-c -lpthread \
	-lACE
