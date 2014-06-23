METASOURCES = AUTO

#ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = \
  -DRPG_MAGIC_BUILD_DLL \
  -I$(srcdir)/../common \
  -I$(srcdir)/../character \
  -I$(srcdir)/../chance/dice

SUBDIRS = scripts

noinst_HEADERS = \
  stdafx.h
include_HEADERS = \
  rpg_magic_abilityclass.h \
  rpg_magic_abilitytype.h \
  rpg_magic_casterclassunion.h \
  rpg_magic_check.h \
  rpg_magic_checktypeunion.h \
  rpg_magic_common.h \
  rpg_magic_common_tools.h \
  rpg_magic_countermeasure.h \
  rpg_magic_defines.h \
  rpg_magic_descriptor.h \
  rpg_magic_dictionary.h \
  rpg_magic_domain.h \
  rpg_magic_exports.h \
  rpg_magic_incl.h \
  rpg_magic_school.h \
  rpg_magic_spell_areaofeffect.h \
  rpg_magic_spell_castingtime.h \
  rpg_magic_spell_damagetypeunion.h
  rpg_magic_spell_duration.h \
  rpg_magic_spell_durationproperties.h \
  rpg_magic_spell_effect.h \
  rpg_magic_spell_effectproperties.h \
  rpg_magic_spell_level.h \
  rpg_magic_spell_precondition.h \
  rpg_magic_spell_preconditionproperties.h \
  rpg_magic_spell_propertiesxml.h \
  rpg_magic_spell_rangeeffect.h \
  rpg_magic_spell_rangeproperties.h \
  rpg_magic_spell_target.h \
  rpg_magic_spell_targetproperties.h \
  rpg_magic_spell_type.h \
  rpg_magic_spelllikeproperties.h \
  rpg_magic_spelltype.h \
  rpg_magic_subschool.h \
  rpg_magic_XML_parser.h \
  rpg_magic_XML_tree.h \
  rpg_magic_XML_types.h

lib_LTLIBRARIES = libRPG_Magic.la

libRPG_Magic_la_SOURCES = \
  rpg_magic_abilityclass.h \
  rpg_magic_abilitytype.h \
  rpg_magic_casterclassunion.h \
  rpg_magic_check.h \
  rpg_magic_checktypeunion.h \
  rpg_magic_common.h \
  rpg_magic_common_tools.cpp \
  rpg_magic_common_tools.h \
  rpg_magic_countermeasure.h \
  rpg_magic_defines.h \
  rpg_magic_descriptor.h \
  rpg_magic_dictionary.cpp \
  rpg_magic_dictionary.h \
  rpg_magic_domain.h \
  rpg_magic_exports.h \
  rpg_magic_incl.h \
  rpg_magic_school.h \
  rpg_magic_spell_areaofeffect.h \
  rpg_magic_spell_castingtime.h \
  rpg_magic_spell_damagetypeunion.h \
  rpg_magic_spell_duration.h \
  rpg_magic_spell_durationproperties.h \
  rpg_magic_spell_effect.h \
  rpg_magic_spell_effectproperties.h \
  rpg_magic_spell_level.h \
  rpg_magic_spell_precondition.h \
  rpg_magic_spell_preconditionproperties.h \
  rpg_magic_spell_propertiesxml.h \
  rpg_magic_spell_rangeeffect.h \
  rpg_magic_spell_rangeproperties.h \
  rpg_magic_spell_target.h \
  rpg_magic_spell_targetproperties.h \
  rpg_magic_spell_type.h \
  rpg_magic_spelllikeproperties.h \
  rpg_magic_spelltype.h \
  rpg_magic_subschool.h \
  rpg_magic_XML_parser.cpp \
  rpg_magic_XML_parser.h \
  rpg_magic_XML_tree.cpp \
  rpg_magic_XML_tree.h \
  rpg_magic_XML_types.cpp \
  rpg_magic_XML_types.h \
  stdafx.cpp \
  stdafx.h

pkgdatadir = $(datadir)/@META_PACKAGE@
configdir = $(pkgdatadir)/config
config_DATA = \
  rpg_magic.xml \
  rpg_magic.xsd

EXTRA_DIST = \
  $(config_DATA) \
  rpg_magic.map

