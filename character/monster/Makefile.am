METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
  -DRPG_MONSTER_BUILD_DLL \
  -I$(srcdir)/../../common \
  -I$(srcdir)/../../character \
  -I$(srcdir)/../../magic \
  -I$(srcdir)/../../item \
  -I$(srcdir)/../../combat \
  -I$(srcdir)/../player \
  -I$(srcdir)/../../chance/dice

SUBDIRS = scripts

noinst_HEADERS = \
 stdafx.h
include_HEADERS = \
  rpg_monster.h \
  rpg_monster_actiontrigger.h \
  rpg_monster_advancement.h \
  rpg_monster_advancementstep.h \
  rpg_monster_attack.h \
  rpg_monster_attackaction.h \
  rpg_monster_common.h \
  rpg_monster_common_tools.h \
  rpg_monster_defenseaction.h \
  rpg_monster_defines.h \
  rpg_monster_dictionary.h \
  rpg_monster_exports.h \
  rpg_monster_incl.h \
  rpg_monster_naturalarmorclass.h \
  rpg_monster_naturalweapon.h \
  rpg_monster_organization.h \
  rpg_monster_organizations.h \
  rpg_monster_organizationslaverstep.h \
  rpg_monster_organizationstep.h \
  rpg_monster_propertiesxml.h \
  rpg_monster_savingthrowmodifiers.h \
  rpg_monster_size.h \
  rpg_monster_spawn.h \
  rpg_monster_specialabilityprecondition.h \
  rpg_monster_specialabilityproperties.h \
  rpg_monster_specialattackprecondition.h \
  rpg_monster_specialattackproperties.h \
  rpg_monster_specialbaseproperties.h \
  rpg_monster_specialdefenseprecondition.h \
  rpg_monster_specialdefenseproperties.h \
  rpg_monster_specialpropertytypeunion.h \
  rpg_monster_summonmonster.h \
  rpg_monster_weapontypeunion.h \
  rpg_monster_XML_parser.h \
  rpg_monster_XML_tree.h \
  rpg_monster_XML_types.h

lib_LTLIBRARIES = libRPG_Monster.la

libRPG_Monster_la_SOURCES = \
  rpg_monster.cpp \
  rpg_monster.h \
  rpg_monster_actiontrigger.h \
  rpg_monster_advancement.h \
  rpg_monster_advancementstep.h \
  rpg_monster_attack.h \
  rpg_monster_attackaction.h \
  rpg_monster_common.h \
  rpg_monster_common_tools.cpp \
  rpg_monster_common_tools.h \
  rpg_monster_defenseaction.h \
  rpg_monster_defines.h \
  rpg_monster_dictionary.cpp \
  rpg_monster_dictionary.h \
  rpg_monster_exports.h \
  rpg_monster_incl.h \
  rpg_monster_naturalarmorclass.h \
  rpg_monster_naturalweapon.h \
  rpg_monster_organization.h \
  rpg_monster_organizations.h \
  rpg_monster_organizationslaverstep.h \
  rpg_monster_organizationstep.h \
  rpg_monster_propertiesxml.h \
  rpg_monster_savingthrowmodifiers.h \
  rpg_monster_size.h \
  rpg_monster_spawn.h \
  rpg_monster_specialabilityprecondition.h \
  rpg_monster_specialabilityproperties.h \
  rpg_monster_specialattackprecondition.h \
  rpg_monster_specialattackproperties.h \
  rpg_monster_specialbaseproperties.h \
  rpg_monster_specialdefenseprecondition.h \
  rpg_monster_specialdefenseproperties.h \
  rpg_monster_specialpropertytypeunion.h \
  rpg_monster_summonmonster.h \
  rpg_monster_weapontypeunion.h \
  rpg_monster_XML_parser.cpp \
  rpg_monster_XML_parser.h \
  rpg_monster_XML_tree.cpp \
  rpg_monster_XML_tree.h \
  rpg_monster_XML_types.cpp \
  rpg_monster_XML_types.h \
  stdafx.cpp \
  stdafx.h

pkgdatadir = $(datadir)/@META_PACKAGE@
configdir = $(pkgdatadir)/config
config_DATA = \
  rpg_monster.xsd \
  rpg_monster.xml

EXTRA_DIST = \
  $(config_DATA) \
  rpg_monster.map
