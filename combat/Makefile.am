#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
  -I$(srcdir)/../common \
  -I$(srcdir)/../chance/dice \
  -I$(srcdir)/../magic \
  -I$(srcdir)/../character \
  -I$(srcdir)/../item

SUBDIRS = scripts

noinst_HEADERS = \
  stdafx.h
include_HEADERS = \
  rpg_combat_attackform.h \
  rpg_combat_attacksituation.h \
  rpg_combat_check.h \
  rpg_combat_checktypeunion.h \
  rpg_combat_common.h \
  rpg_combat_common_tools.h \
  rpg_combat_damage.h \
  rpg_combat_damagecountermeasure.h \
  rpg_combat_damageduration.h \
  rpg_combat_damageelement.h \
  rpg_combat_damagereductiontype.h \
  rpg_combat_damagetypeunion.h \
  rpg_combat_defensesituation.h \
  rpg_combat_exports.h \
  rpg_combat_incl.h \
  rpg_combat_otherdamage.h \
  rpg_combat_otherdamagetype.h \
  rpg_combat_rangedattackproperties.h \
  rpg_combat_rangedeffectunion.h \
  rpg_combat_specialattack.h \
  rpg_combat_specialdamagetype.h \
  rpg_combat_XML_parser.h \
  rpg_combat_XML_tree.h \
  rpg_combat_XML_types.h

lib_LTLIBRARIES = libRPG_Combat.la

libRPG_Combat_la_SOURCES = \
  rpg_combat_attackform.h \
  rpg_combat_attacksituation.h \
  rpg_combat_check.h \
  rpg_combat_checktypeunion.h \
  rpg_combat_common.h \
  rpg_combat_common_tools.cpp \
  rpg_combat_common_tools.h \
  rpg_combat_damage.h \
  rpg_combat_damagecountermeasure.h \
  rpg_combat_damageduration.h \
  rpg_combat_damagereductiontype.h \
  rpg_combat_defensesituation.h \
  rpg_combat_exports.h \
  rpg_combat_incl.h \
  rpg_combat_otherdamage.h \
  rpg_combat_otherdamagetype.h \
  rpg_combat_rangedattackproperties.h \
  rpg_combat_rangedeffectunion.h \
  rpg_combat_XML_parser.cpp \
  rpg_combat_XML_parser.h \
  rpg_combat_XML_tree.cpp \
  rpg_combat_XML_tree.h \
  rpg_combat_XML_types.cpp \
  rpg_combat_XML_types.h \
  stdafx.cpp \
  stdafx.h

pkgdatadir = $(datadir)/@META_PACKAGE@
configdir = $(pkgdatadir)/config
config_DATA = \
  rpg_combat.xsd

EXTRA_DIST = \
  $(config_DATA) \
  rpg_combat.map

