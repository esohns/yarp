#METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4
#  -I$(top_srcdir)/../modules/ATCD/ACE
AM_CPPFLAGS = \
  -I$(top_srcdir)/../../ATCD/ACE \
  -I$(top_srcdir)/../modules/libCommon/src \
  -I$(top_srcdir)/../chance/dice \
  -I$(top_srcdir)/../common \
  -I$(top_srcdir)/../magic \
  -I$(top_srcdir)/../character \
  -DRPG_ITEM_BUILD_DLL

SUBDIRS = scripts

noinst_HEADERS = \
  stdafx.h
include_HEADERS = \
  rpg_item_armor.h \
  rpg_item_armorcategory.h \
  rpg_item_armorpropertiesbase.h \
  rpg_item_armorpropertiesxml.h \
  rpg_item_armortype.h \
  rpg_item_base.h \
  rpg_item_baseproperties.h \
  rpg_item_basexml.h \
  rpg_item_common.h \
  rpg_item_common_tools.h \
  rpg_item_common_XML_tools.h \
  rpg_item_commodity.h \
  rpg_item_commoditybeverage.h \
  rpg_item_commoditylight.h \
  rpg_item_commoditypropertiesbase.h \
  rpg_item_commoditytype.h \
  rpg_item_commodityunion.h \
  rpg_item_creationcost.h \
  rpg_item_criticalhitproperties.h \
  rpg_item_defines.h \
  rpg_item_dictionary.h \
  rpg_item_exports.h \
  rpg_item_incl.h \
  rpg_item_instance_base.h \
  rpg_item_instance_common.h \
  rpg_item_instance_manager.h \
  rpg_item_inventoryxml.h \
  rpg_item_magicalprerequisites.h \
  rpg_item_money.h \
  rpg_item_propertiesbase.h \
  rpg_item_storeprice.h \
  rpg_item_type.h \
  rpg_item_weapon.h \
  rpg_item_weaponcategory.h \
  rpg_item_weaponclass.h \
  rpg_item_weaponpropertiesbase.h \
  rpg_item_weaponpropertiesxml.h \
  rpg_item_weapontype.h \
  rpg_item_XML_parser.h \
  rpg_item_XML_tree.h \
  rpg_item_XML_types.h

lib_LTLIBRARIES = libRPG_Item.la

libRPG_Item_la_SOURCES = \
  rpg_item_armor.cpp \
  rpg_item_armor.h \
  rpg_item_armorcategory.h \
  rpg_item_armorpropertiesbase.h \
  rpg_item_armorpropertiesxml.h \
  rpg_item_armortype.h \
  rpg_item_base.cpp \
  rpg_item_base.h \
  rpg_item_basexml.h \
  rpg_item_baseproperties.h \
  rpg_item_common.h \
  rpg_item_common_tools.cpp \
  rpg_item_common_tools.h \
  rpg_item_common_XML_tools.h \
  rpg_item_common_XML_tools.cpp \
  rpg_item_commodity.h \
  rpg_item_commodity.cpp \
  rpg_item_commoditybeverage.h \
  rpg_item_commoditylight.h \
  rpg_item_commoditypropertiesbase.h \
  rpg_item_commoditytype.h \
  rpg_item_commodityunion.h \
  rpg_item_creationcost.h \
  rpg_item_criticalhitproperties.h \
  rpg_item_defines.h \
  rpg_item_dictionary.cpp \
  rpg_item_dictionary.h \
  rpg_item_exports.h \
  rpg_item_instance_base.cpp \
  rpg_item_instance_base.h \
  rpg_item_instance_common.h \
  rpg_item_instance_manager.cpp \
  rpg_item_instance_manager.h \
  rpg_item_inventoryxml.h \
  rpg_item_magicalprerequisites.h \
  rpg_item_propertiesbase.h \
  rpg_item_storeprice.h \
  rpg_item_weapon.cpp \
  rpg_item_weapon.h \
  rpg_item_weaponcategory.h \
  rpg_item_weaponclass.h \
  rpg_item_weaponpropertiesbase.h \
  rpg_item_weaponpropertiesxml.h \
  rpg_item_weapontype.h \
  rpg_item_XML_parser.cpp \
  rpg_item_XML_parser.h \
  rpg_item_XML_tree.cpp \
  rpg_item_XML_tree.h \
  rpg_item_XML_types.cpp \
  rpg_item_XML_types.h \
  stdafx.cpp \
  stdafx.h

pkgdatadir = $(datadir)/@META_PACKAGE@
configdir = $(pkgdatadir)/config
config_DATA = \
  rpg_item.xsd \
  rpg_item.xml

EXTRA_DIST = \
  $(config_DATA) \
  rpg_item.map
