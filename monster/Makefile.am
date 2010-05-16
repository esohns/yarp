METASOURCES = AUTO

ACLOCAL_AMFLAGS = `-I m4'

include_HEADERS = rpg_monster_XML_parser.h rpg_monster_XML_types.h \
	rpg_monster_actiontrigger.h rpg_monster_advancement.h rpg_monster_advancementstep.h \
	rpg_monster_attack.h rpg_monster_attackaction.h rpg_monster_common.h \
	rpg_monster_common_tools.h rpg_monster_defenseaction.h rpg_monster_dictionary.h rpg_monster_incl.h \
	rpg_monster_naturalarmorclass.h rpg_monster_naturalweapon.h rpg_monster_organization.h \
	rpg_monster_organizations.h rpg_monster_organizationslaverstep.h rpg_monster_organizationstep.h \
	rpg_monster_propertiesxml.h rpg_monster_savingthrowmodifiers.h \
	rpg_monster_specialabilityprecondition.h rpg_monster_specialabilityproperties.h \
	rpg_monster_specialattackprecondition.h rpg_monster_specialattackproperties.h \
	rpg_monster_specialbaseproperties.h rpg_monster_specialdefenseprecondition.h \
	rpg_monster_specialdefenseproperties.h rpg_monster_specialpropertytypeunion.h rpg_monster_summonmonster.h \
	rpg_monster_weapontypeunion.h

lib_LTLIBRARIES = libRPG_Monster.la

libRPG_Monster_la_LDFLAGS = -L/usr/local/lib
libRPG_Monster_la_SOURCES = rpg_XMLSchema_XML_types.h \
	rpg_monster_XML_parser.cpp rpg_monster_XML_parser.h rpg_monster_XML_types.cpp \
	rpg_monster_XML_types.h rpg_monster_actiontrigger.h rpg_monster_advancement.h \
	rpg_monster_advancementstep.h rpg_monster_attack.h rpg_monster_attackaction.h rpg_monster_common.h \
	rpg_monster_common_tools.cpp rpg_monster_common_tools.h rpg_monster_defenseaction.h \
	rpg_monster_dictionary.cpp rpg_monster_dictionary.h rpg_monster_incl.h \
	rpg_monster_naturalarmorclass.h rpg_monster_naturalweapon.h rpg_monster_organization.h \
	rpg_monster_organizations.h rpg_monster_organizationslaverstep.h rpg_monster_organizationstep.h \
	rpg_monster_propertiesxml.h rpg_monster_savingthrowmodifiers.h \
	rpg_monster_specialabilityprecondition.h rpg_monster_specialabilityproperties.h \
	rpg_monster_specialattackprecondition.h rpg_monster_specialattackproperties.h \
	rpg_monster_specialbaseproperties.h rpg_monster_specialdefenseprecondition.h \
	rpg_monster_specialdefenseproperties.h rpg_monster_specialpropertytypeunion.h rpg_monster_summonmonster.h \
	rpg_monster_weapontypeunion.h

bindata_DATA = rpg_monster.xsd rpg_monster.xml
bindatadir = $(bindir)
noinst_HEADERS = rpg_XMLSchema_XML_types.h
