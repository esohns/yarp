METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -DRPG_COMMON_BUILD_DLL -DMETA_PACKAGE_NAME=\"@META_PACKAGE_NAME@\" \
  -I$(srcdir)/../chance/dice

noinst_HEADERS = stdafx.h
include_HEADERS = rpg_XMLSchema_XML_tree.h \
	rpg_XMLSchema_XML_types.h \
	rpg_common.h \
	rpg_common_XML_parser.h \
	rpg_common_XML_tree.h \
	rpg_common_XML_types.h \
	rpg_common_actiontype.h \
	rpg_common_amount.h \
	rpg_common_areaofeffect.h \
	rpg_common_attribute.h \
	rpg_common_basechecktypeunion.h \
	rpg_common_camp.h \
	rpg_common_checktype.h \
	rpg_common_climate.h \
	rpg_common_condition.h \
	rpg_common_countermeasure.h \
	rpg_common_creaturemetatype.h \
	rpg_common_creaturesubtype.h \
	rpg_common_creaturetype.h \
	rpg_common_defines.h \
	rpg_common_duration.h \
	rpg_common_effecttype.h \
	rpg_common_environment.h \
	rpg_common_environment_XML_types.h \
	rpg_common_environment_incl.h \
	rpg_common_file_tools.h \
	rpg_common_icontrol.h \
	rpg_common_idumpstate.h \
	rpg_common_incl.h \
	rpg_common_irefcount.h \
	rpg_common_istatistic.h \
	rpg_common_itimer.h \
	rpg_common_macros.h \
	rpg_common_physicaldamagetype.h \
	rpg_common_plane.h \
	rpg_common_referencecounter_base.h \
	rpg_common_savereductiontype.h \
	rpg_common_savingthrow.h \
	rpg_common_savingthrowcheck.h \
	rpg_common_size.h \
	rpg_common_skill.h \
	rpg_common_subclass.h \
	rpg_common_terrain.h \
	rpg_common_timer_manager.h \
	rpg_common_timerhandler.h \
	rpg_common_tools.h \
	rpg_common_usage.h \
	rpg_common_xsderrorhandler.h \
	rpg_common_exports.h \
	rpg_common_XML_tools.h \
	rpg_common_environment_XML_tree.h \
	rpg_common_fixedperiod.h \
	rpg_common_effectduration.h \
	rpg_common_track.h \
	rpg_common_timeofday.h \
	rpg_common_ambientlighting.h

lib_LTLIBRARIES = libRPG_Common.la

libRPG_Common_la_SOURCES = rpg_XMLSchema_XML_tree.h \
	rpg_XMLSchema_XML_types.h \
	rpg_common.h \
	rpg_common_XML_parser.cpp \
	rpg_common_XML_parser.h \
	rpg_common_XML_tree.cpp \
	rpg_common_XML_tree.h \
	rpg_common_XML_types.cpp \
	rpg_common_XML_types.h \
	rpg_common_actiontype.h \
	rpg_common_amount.h \
	rpg_common_areaofeffect.h \
	rpg_common_attribute.h \
	rpg_common_basechecktypeunion.h \
	rpg_common_camp.h \
	rpg_common_checktype.h \
	rpg_common_climate.h \
	rpg_common_condition.h \
	rpg_common_countermeasure.h \
	rpg_common_creaturemetatype.h \
	rpg_common_creaturesubtype.h \
	rpg_common_creaturetype.h \
	rpg_common_defines.h \
	rpg_common_duration.h \
	rpg_common_effecttype.h \
	rpg_common_environment.h \
	rpg_common_environment_XML_types.cpp \
	rpg_common_environment_XML_types.h \
	rpg_common_environment_incl.h \
	rpg_common_file_tools.cpp \
	rpg_common_file_tools.h \
	rpg_common_icontrol.h \
	rpg_common_idumpstate.h \
	rpg_common_incl.h \
	rpg_common_irefcount.h \
	rpg_common_istatistic.h \
	rpg_common_itimer.h \
	rpg_common_macros.h \
	rpg_common_physicaldamagetype.h \
	rpg_common_plane.h \
	rpg_common_referencecounter_base.cpp \
	rpg_common_referencecounter_base.h \
	rpg_common_savereductiontype.h \
	rpg_common_savingthrow.h \
	rpg_common_savingthrowcheck.h \
	rpg_common_size.h \
	rpg_common_skill.h \
	rpg_common_subclass.h \
	rpg_common_terrain.h \
	rpg_common_timer_manager.cpp \
	rpg_common_timer_manager.h \
	rpg_common_timerhandler.cpp \
	rpg_common_timerhandler.h \
	rpg_common_tools.cpp \
	rpg_common_tools.h \
	rpg_common_usage.h \
	rpg_common_xsderrorhandler.cpp \
	rpg_common_xsderrorhandler.h \
	rpg_common_exports.h \
	rpg_common_XML_tools.h \
	rpg_common_XML_tools.cpp \
	rpg_common_environment_XML_tree.cpp \
	rpg_common_environment_XML_tree.h \
	rpg_common_fixedperiod.h \
	rpg_common_effectduration.h \
	rpg_common_track.h \
	rpg_common_timeofday.h \
  rpg_common_ambientlighting.h

pkgdatadir = $(datadir)/@META_PACKAGE_NAME@
configdir = $(pkgdatadir)/config
config_DATA = rpg_common.xsd rpg_common_environment.xsd

EXTRA_DIST = $(config_DATA)
