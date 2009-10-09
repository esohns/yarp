METASOURCES = AUTO

include_HEADERS = rpg_item_common.h rpg_item_common_tools.h \
	rpg_item_dictionary.h rpg_item_dictionary_parser.h rpg_item_inventory.h rpg_item_weapon.h \
	rpg_item_weapon_base.h

lib_LTLIBRARIES = libRPG_Item.la
libRPG_Item_la_SOURCES = rpg_item_common.h rpg_item_common_tools.cpp \
	rpg_item_common_tools.h rpg_item_dictionary.cpp rpg_item_dictionary.h \
	rpg_item_dictionary_parser.cpp rpg_item_dictionary_parser.h rpg_item_inventory.cpp \
	rpg_item_inventory.h rpg_item_weapon.cpp rpg_item_weapon.h rpg_item_weapon_base.cpp \
	rpg_item_weapon_base.h
libRPG_Item_la_LDFLAGS = -L/usr/local/lib
libRPG_Item_la_LIBADD = -lACEXML_Parser -lACEXML -lACE -lpthread
noinst_PROGRAMS = parser
parser_SOURCES = main.cpp
parser_LDADD = $(top_builddir)/libRPG_Item.la -lACEXML_Parser -lACEXML -lACE \
	-lpthread
parser_LDFLAGS = -L/usr/local/lib
