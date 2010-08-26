METASOURCES = AUTO

ACLOCAL_AMFLAGS = `-I m4'

include_HEADERS = rpg_item_XML_parser.h rpg_item_XML_tree.h \
	rpg_item_XML_types.h rpg_item_armor.h rpg_item_armor_base.h rpg_item_armorcategory.h \
	rpg_item_armorpropertiesbase.h rpg_item_armorpropertiesxml.h rpg_item_armortype.h rpg_item_base.h \
	rpg_item_baseproperties.h rpg_item_common.h rpg_item_common_tools.h rpg_item_creationcost.h \
	rpg_item_criticalhitproperties.h rpg_item_dictionary.h rpg_item_incl.h rpg_item_instance_base.h \
	rpg_item_instance_common.h rpg_item_instance_manager.h rpg_item_magicalprerequisites.h \
	rpg_item_money.h rpg_item_propertiesbase.h rpg_item_storeprice.h rpg_item_type.h \
	rpg_item_weapon.h rpg_item_weapon_base.h rpg_item_weaponcategory.h rpg_item_weaponclass.h \
	rpg_item_weaponpropertiesbase.h rpg_item_weaponpropertiesxml.h rpg_item_weapontype.h

lib_LTLIBRARIES = libRPG_Item.la
libRPG_Item_la_SOURCES = rpg_XMLSchema_XML_tree.h rpg_XMLSchema_XML_types.h \
	rpg_item_XML_parser.cpp rpg_item_XML_parser.h rpg_item_XML_tree.cpp rpg_item_XML_tree.h \
	rpg_item_XML_types.cpp rpg_item_XML_types.h rpg_item_armor.cpp rpg_item_armor.h \
	rpg_item_armor_base.cpp rpg_item_armor_base.h rpg_item_armorcategory.h \
	rpg_item_armordictionary.h rpg_item_armorpropertiesbase.h rpg_item_armorpropertiesxml.h \
	rpg_item_armortype.h rpg_item_base.cpp rpg_item_base.h rpg_item_baseproperties.h \
	rpg_item_common.h rpg_item_common_tools.cpp rpg_item_common_tools.h \
	rpg_item_creationcost.h rpg_item_criticalhitproperties.h rpg_item_dictionary.cpp \
	rpg_item_dictionary.h rpg_item_instance_base.cpp rpg_item_instance_base.h \
	rpg_item_instance_common.h rpg_item_instance_manager.cpp rpg_item_instance_manager.h \
	rpg_item_magicalprerequisites.h rpg_item_propertiesbase.h rpg_item_storeprice.h rpg_item_weapon.cpp \
	rpg_item_weapon.h rpg_item_weapon_base.cpp rpg_item_weapon_base.h \
	rpg_item_weaponcategory.h rpg_item_weaponclass.h rpg_item_weapondictionary.h \
	rpg_item_weaponpropertiesbase.h rpg_item_weaponpropertiesxml.h rpg_item_weapontype.h
libRPG_Item_la_LDFLAGS = -L/usr/local/lib


bindata_DATA = rpg_item.xsd rpg_item.xml
bindatadir = $(bindir)
noinst_HEADERS = rpg_XMLSchema_XML_tree.h rpg_XMLSchema_XML_types.h
