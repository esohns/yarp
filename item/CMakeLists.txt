
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############

set(RPG_Item_LIB_SRCS
    rpg_XMLSchema_XML_tree.h
    rpg_XMLSchema_XML_types.h
    rpg_item_XML_parser.cpp
    rpg_item_XML_parser.h
    rpg_item_XML_tree.cpp
    rpg_item_XML_tree.h
    rpg_item_XML_types.cpp
    rpg_item_XML_types.h
    rpg_item_armor.cpp
    rpg_item_armor.h
    rpg_item_armorcategory.h
    rpg_item_armorpropertiesbase.h
    rpg_item_armorpropertiesxml.h
    rpg_item_armortype.h
    rpg_item_base.cpp
    rpg_item_base.h
    rpg_item_baseproperties.h
    rpg_item_common.h
    rpg_item_common_tools.cpp
    rpg_item_common_tools.h
    rpg_item_creationcost.h
    rpg_item_criticalhitproperties.h
    rpg_item_defines.h
    rpg_item_dictionary.cpp
    rpg_item_dictionary.h
    rpg_item_instance_base.cpp
    rpg_item_instance_base.h
    rpg_item_instance_common.h
    rpg_item_instance_manager.cpp
    rpg_item_instance_manager.h
    rpg_item_magicalprerequisites.h
    rpg_item_propertiesbase.h
    rpg_item_storeprice.h
    rpg_item_weapon.cpp
    rpg_item_weapon.h
    rpg_item_weaponcategory.h
    rpg_item_weaponclass.h
    rpg_item_weaponpropertiesbase.h
    rpg_item_weaponpropertiesxml.h
    rpg_item_weapontype.h)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Item_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_library(RPG_Item SHARED ${RPG_Item_LIB_SRCS})

add_dependencies(RPG_Item RPG_Magic RPG_Common RPG_Dice)

set_target_properties(RPG_Item PROPERTIES VERSION 1.0.0 SOVERSION 1)

install(TARGETS RPG_Item LIBRARY DESTINATION lib)

########### install files ###############

install(FILES
	rpg_item.xsd
	rpg_item.xml
DESTINATION share/RPG/config)

install(FILES
	rpg_item_XML_parser.h
	rpg_item_XML_tree.h
  	rpg_item_XML_types.h
	rpg_item_armor.h
	rpg_item_armorcategory.h
  	rpg_item_armorpropertiesbase.h
	rpg_item_armorpropertiesxml.h
	rpg_item_armortype.h
	rpg_item_base.h
  	rpg_item_baseproperties.h
	rpg_item_common.h
	rpg_item_common_tools.h
	rpg_item_creationcost.h
  	rpg_item_criticalhitproperties.h
	rpg_item_defines.h
	rpg_item_dictionary.h
	rpg_item_incl.h
  	rpg_item_instance_base.h
	rpg_item_instance_common.h
	rpg_item_instance_manager.h
  	rpg_item_magicalprerequisites.h
	rpg_item_money.h
	rpg_item_propertiesbase.h
	rpg_item_storeprice.h
  	rpg_item_type.h
	rpg_item_weapon.h
	rpg_item_weaponcategory.h
	rpg_item_weaponclass.h
  	rpg_item_weaponpropertiesbase.h
	rpg_item_weaponpropertiesxml.h
	rpg_item_weapontype.h
DESTINATION include)

