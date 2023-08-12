include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/Threads.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/ACE.cmake)

########### next target ###############
set (LIBRARY RPG_Item)
set (${LIBRARY}_SRCS
#    rpg_XMLSchema_XML_tree.h
#    rpg_XMLSchema_XML_types.h
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
     rpg_item_commodity.cpp
     rpg_item_commodity.h
     rpg_item_commoditybeverage.h
     rpg_item_commoditylight.h
     rpg_item_commoditypropertiesbase.h
     rpg_item_commoditytype.h
     rpg_item_commodityunion.h
     rpg_item_common.h
     rpg_item_common_tools.cpp
     rpg_item_common_tools.h
     rpg_item_common_XML_tools.cpp
     rpg_item_common_XML_tools.h
     rpg_item_creationcost.h
     rpg_item_criticalhitproperties.h
     rpg_item_defines.h
   #  rpg_item_exports.h
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
     rpg_item_weapontype.h
     stdafx.h)

#set_source_files_properties(${RPG_Item_LIB_SRCS} PROPERTIES COMPILE_DEFINITIONS RPG_ITEM_BUILD_DLL)

add_library (${LIBRARY} STATIC ${${LIBRARY}_SRCS})

if (UNIX)
elseif (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/xerces-c/build/msvc/src>)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/xerces-c/src>)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/xsd/include>)
endif ()

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${ACE_INCLUDE_DIR}>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../3rd_party/xsd>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../chance/dice>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../character>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../magic>)

# add_custom_target(item_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_item.xsd)
#add_custom_command (OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_item_XML_types.cpp
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_item_XML_types.h
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_item_XML_tree.cpp
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_item_XML_tree.h
#                    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
##                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_item.xsd
##                            ${CMAKE_CURRENT_SOURCE_DIR}/rpg_item.map
#                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
#                    COMMENT "generating XML <--> C++ bindings...")

#add_dependencies (${LIBRARY} RPG_Dice RPG_Common RPG_Character RPG_Magic)
add_dependencies (${LIBRARY} RPG_Dice RPG_Common RPG_Magic)

set_target_properties (${LIBRARY} PROPERTIES VERSION 1.0.0 SOVERSION 1)

# add PCH
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

########### install files ###############
install (FILES
         etc/rpg_item.xsd
         etc/rpg_item.xml
         DESTINATION ${INSTALL_SHARE_DIR} COMPONENT config)
install (FILES
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
         rpg_item_commodity.h
         rpg_item_commoditybeverage.h
         rpg_item_commoditylight.h
         rpg_item_commoditypropertiesbase.h
         rpg_item_commoditytype.h
         rpg_item_commodityunion.h
         rpg_item_common.h
         rpg_item_common_tools.h
         rpg_item_common_XML_tools.h
         rpg_item_creationcost.h
         rpg_item_criticalhitproperties.h
         rpg_item_defines.h
        # rpg_item_exports.h
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
install (TARGETS ${LIBRARY} LIBRARY
         DESTINATION lib)
