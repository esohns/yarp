METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
#  -I$(top_srcdir)/../modules/ATCD/ACE
AM_CPPFLAGS = \
  -I$(top_srcdir)/../../ATCD/ACE \
  -I$(top_srcdir)/../modules/libCommon/src \
  -I$(top_srcdir)/../chance/dice \
  -I$(top_srcdir)/../chance \
  -I$(top_srcdir)/../common \
  -I$(top_srcdir)/../magic \
  -I$(top_srcdir)/../item

SUBDIRS = . player monster scripts

noinst_HEADERS = \
  stdafx.h
include_HEADERS = \
  rpg_character_ability.h \
  rpg_character_alignment.h \
  rpg_character_alignmentcivic.h \
  rpg_character_alignmentethic.h \
  rpg_character_attributes.h \
  rpg_character_check.h \
  rpg_character_checktypeunion.h \
  rpg_character_class_common.h \
  rpg_character_class_common_tools.h \
  rpg_character_classxml.h \
  rpg_character_common.h \
  rpg_character_common_tools.h \
  rpg_character_defines.h \
  rpg_character_encumbrance.h \
  rpg_character_equipmentslot.h \
  rpg_character_exports.h \
  rpg_character_feat.h \
  rpg_character_feats.h \
  rpg_character_gender.h \
  rpg_character_incl.h \
  rpg_character_metaclass.h \
  rpg_character_offhand.h \
  rpg_character_race.h \
  rpg_character_race_common.h \
  rpg_character_race_common_tools.h \
  rpg_character_skills.h \
  rpg_character_skills_common.h \
  rpg_character_skills_common_tools.h \
  rpg_character_skillvalue.h \
  rpg_character_XML_parser.h \
  rpg_character_XML_tools.h \
  rpg_character_XML_tree.h \
  rpg_character_XML_types.h

lib_LTLIBRARIES = libRPG_Character.la

libRPG_Character_la_SOURCES = \
  rpg_character_ability.h \
  rpg_character_alignment.h \
  rpg_character_alignmentcivic.h \
  rpg_character_alignmentethic.h \
  rpg_character_attributes.h \
  rpg_character_check.h \
  rpg_character_checktypeunion.h \
  rpg_character_class_common.h \
  rpg_character_class_common_tools.cpp \
  rpg_character_class_common_tools.h \
  rpg_character_classxml.h \
  rpg_character_common.h \
  rpg_character_common_tools.cpp \
  rpg_character_common_tools.h \
  rpg_character_defines.h \
  rpg_character_encumbrance.h \
  rpg_character_equipmentslot.h \
  rpg_character_exports.h \
  rpg_character_feat.h \
  rpg_character_feats.h \
  rpg_character_gender.h \
  rpg_character_incl.h \
  rpg_character_metaclass.h \
  rpg_character_offhand.h \
  rpg_character_race.h \
  rpg_character_race_common.h \
  rpg_character_race_common_tools.cpp \
  rpg_character_race_common_tools.h \
  rpg_character_skills.h \
  rpg_character_skills_common.h \
  rpg_character_skills_common_tools.cpp \
  rpg_character_skills_common_tools.h \
  rpg_character_skillvalue.h \
  rpg_character_XML_parser.cpp \
  rpg_character_XML_parser.h \
  rpg_character_XML_tools.h \
  rpg_character_XML_tools.cpp \
  rpg_character_XML_tree.cpp \
  rpg_character_XML_tree.h \
  rpg_character_XML_types.cpp \
  rpg_character_XML_types.h \
  stdafx.cpp \
  stdafx.h

pkgdatadir = $(datadir)/@META_PACKAGE@
configdir = $(pkgdatadir)/config
config_DATA = rpg_character.xsd

EXTRA_DIST = \
  $(config_DATA) \
  rpg_character.map
