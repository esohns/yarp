METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
#  -I$(top_srcdir)/../../modules/ATCD/ACE
AM_CPPFLAGS = \
  -I$(top_srcdir)/../../ATCD/ACE \
  -I$(top_srcdir)/../modules/libCommon/src \
  -I$(top_srcdir)/../common

SUBDIRS = scripts

noinst_HEADERS = \
  stdafx.h
include_HEADERS = \
  rpg_dice_common_tools.h \
  rpg_dice_common.h \
  rpg_dice_defines.h \
  rpg_dice_dietype.h \
  rpg_dice_exports.h \
  rpg_dice_incl.h \
  rpg_dice_roll.h \
  rpg_dice.h \
  rpg_dice_valuerange.h \
  rpg_dice_XML_parser.h \
  rpg_dice_XML_tree.h \
  rpg_dice_XML_types.h

lib_LTLIBRARIES = libRPG_Dice.la

libRPG_Dice_la_SOURCES = \
  rpg_dice_common_tools.cpp \
  rpg_dice_common_tools.h \
  rpg_dice_common.h \
  rpg_dice_defines.h \
  rpg_dice_dietype.h \
  rpg_dice_exports.h \
  rpg_dice_incl.h \
  rpg_dice_roll.h \
  rpg_dice_valuerange.h \
  rpg_dice_XML_parser.cpp \
  rpg_dice_XML_parser.h \
  rpg_dice_XML_tree.cpp \
  rpg_dice_XML_tree.h \
  rpg_dice_XML_types.cpp \
  rpg_dice_XML_types.h \
  rpg_dice.cpp \
  rpg_dice.h

pkgdatadir = $(datadir)/@META_PACKAGE@
configdir = $(pkgdatadir)/config
config_DATA = \
  rpg_dice.xsd

EXTRA_DIST = $(config_DATA) \
  rpg_dice.map
