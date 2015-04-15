#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
#  -I$(top_srcdir)/../modules/ATCD/ACE
#  -I$(srcdir)/../include
AM_CPPFLAGS = \
  -I$(top_srcdir)/../../ATCD/ACE \
  -I$(top_srcdir)/../modules/libCommon/src \
  -I$(top_srcdir)/../chance/dice \
  -I$(builddir)/.. \
  -DRPG_COMMON_BUILD_DLL \
  -DMETA_PACKAGE=\"@META_PACKAGE@\"

SUBDIRS = scripts

noinst_HEADERS = stdafx.h
include_HEADERS = \
  rpg_common_actiontype.h \
  rpg_common_ambientlighting.h \
  rpg_common_amount.h \
  rpg_common_areaofeffect.h \
  rpg_common_attribute.h \
  rpg_common_basechecktypeunion.h \
  rpg_common_camp.h \
  rpg_common_checktype.h \
  rpg_common_climate.h \
  rpg_common_file_tools.h \
  rpg_common_condition.h \
  rpg_common_countermeasure.h \
  rpg_common_creaturemetatype.h \
  rpg_common_creaturesubtype.h \
  rpg_common_creaturetype.h \
  rpg_common_defines.h \
  rpg_common_duration.h \
  rpg_common_effectduration.h \
  rpg_common_effecttype.h \
  rpg_common_environment_incl.h \
  rpg_common_environment_XML_tree.h \
  rpg_common_environment_XML_types.h \
  rpg_common_environment.h \
  rpg_common_exports.h \
  rpg_common_fixedperiod.h \
  rpg_common_ilock.h \
  rpg_common_incl.h \
  rpg_common_macros.h \
  rpg_common_physicaldamagetype.h \
  rpg_common_plane.h \
  rpg_common_savereductiontype.h \
  rpg_common_savingthrow.h \
  rpg_common_savingthrowcheck.h \
  rpg_common_size.h \
  rpg_common_skill.h \
  rpg_common_subclass.h \
  rpg_common_terrain.h \
  rpg_common_timeofday.h \
  rpg_common_tools.h \
  rpg_common_track.h \
  rpg_common_usage.h \
  rpg_common_XML_parser.h \
  rpg_common_XML_tools.h \
  rpg_common_XML_tree.h \
  rpg_common_XML_types.h \
  rpg_common_xsderrorhandler.h \
  rpg_common.h \
  rpg_XMLSchema_XML_types.h \
  rpg_XMLSchema_XML_tree.h

lib_LTLIBRARIES = libRPG_Common.la
libRPG_Common_la_LDFLAGS = -version-info 0:1:0
libRPG_Common_la_SOURCES = \
  rpg_common_actiontype.h \
  rpg_common_ambientlighting.h \
  rpg_common_amount.h \
  rpg_common_areaofeffect.h \
  rpg_common_attribute.h \
  rpg_common_basechecktypeunion.h \
  rpg_common_camp.h \
  rpg_common_checktype.h \
  rpg_common_climate.h \
  rpg_common_file_tools.cpp \
  rpg_common_file_tools.h \
  rpg_common_condition.h \
  rpg_common_countermeasure.h \
  rpg_common_creaturemetatype.h \
  rpg_common_creaturesubtype.h \
  rpg_common_creaturetype.h \
  rpg_common_defines.h \
  rpg_common_duration.h \
  rpg_common_effectduration.h \
  rpg_common_effecttype.h \
  rpg_common_environment_incl.h \
  rpg_common_environment_XML_tree.cpp \
  rpg_common_environment_XML_tree.h \
  rpg_common_environment_XML_types.cpp \
  rpg_common_environment_XML_types.h \
  rpg_common_environment.h \
  rpg_common_exports.h \
  rpg_common_fixedperiod.h \
  rpg_common_ilock.h \
  rpg_common_incl.h \
  rpg_common_macros.h \
  rpg_common_physicaldamagetype.h \
  rpg_common_plane.h \
  rpg_common_savereductiontype.h \
  rpg_common_savingthrow.h \
  rpg_common_savingthrowcheck.h \
  rpg_common_size.h \
  rpg_common_skill.h \
  rpg_common_subclass.h \
  rpg_common_terrain.h \
  rpg_common_timeofday.h \
  rpg_common_tools.cpp \
  rpg_common_tools.h \
  rpg_common_track.h \
  rpg_common_usage.h \
  rpg_common_XML_parser.cpp \
  rpg_common_XML_parser.h \
  rpg_common_XML_tools.cpp \
  rpg_common_XML_tools.h \
  rpg_common_XML_tree.cpp \
  rpg_common_XML_tree.h \
  rpg_common_XML_types.cpp \
  rpg_common_XML_types.h \
  rpg_common_xsderrorhandler.cpp \
  rpg_common_xsderrorhandler.h \
  rpg_common.h \
  rpg_XMLSchema_XML_types.h \
  rpg_XMLSchema_XML_tree.h \
  stdafx.h
#  stdafx.h.gch

# (GNU-)make-rule for pch
#%.h.gch: %.h
#  $(CXX) -c $<
#stdafx.h.gch: stdafx.h
#   $(CXX) -c $<
#stdafx.h.gch: Makefile
#  $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(libRPG_Common_la_CXXFLAGS) $(extra_CFLAGS_libRPG_Common_la) -c stdafx.h -o stdafx.h.gch

pkgdatadir = $(datadir)/@META_PACKAGE@
configdir = $(pkgdatadir)/config
config_DATA = \
  rpg_common.xsd \
  rpg_common_environment.xsd

EXTRA_DIST = \
  $(config_DATA) \
  rpg_common.map \
  rpg_common_environment.map
