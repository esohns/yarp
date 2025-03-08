include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/Threads.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/ACE.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../scripts/xerces-c.cmake)

########### next target ###############
set (LIBRARY RPG_Combat)
set (${LIBRARY}_SRCS
     rpg_combat_attackform.h
     rpg_combat_attacksituation.h
     rpg_combat_check.h
     rpg_combat_checktypeunion.h
     rpg_combat_common.h
     rpg_combat_common_tools.cpp
     rpg_combat_common_tools.h
     rpg_combat_damage.h
     rpg_combat_damagecountermeasure.h
     rpg_combat_damageduration.h
     rpg_combat_damagereductiontype.h
     rpg_combat_defensesituation.h
     rpg_combat_defines.h
     rpg_combat_incl.h
     rpg_combat_otherdamage.h
     rpg_combat_otherdamagetype.h
     rpg_combat_rangedattackproperties.h
     rpg_combat_rangedeffectunion.h
     rpg_combat_XML_parser.cpp
     rpg_combat_XML_parser.h
     rpg_combat_XML_tree.cpp
     rpg_combat_XML_tree.h
     rpg_combat_XML_types.cpp
     rpg_combat_XML_types.h
     stdafx.h)

#set_source_files_properties(${RPG_Combat_LIB_SRCS} PROPERTIES COMPILE_DEFINITIONS RPG_COMBAT_BUILD_DLL)

add_library (${LIBRARY} STATIC ${${LIBRARY}_SRCS})

if (XERCESC_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${XERCESC_INCLUDE_DIRS}>)
endif (XERCESC_SUPPORT)
if (WIN32)
 add_compile_definitions (XSD_CXX11)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/xsd/include>)
endif (WIN32)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${ACE_INCLUDE_DIR}>)

if (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
endif (WIN32)
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
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../item>)

# add_custom_target(combat_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_combat.xsd)
#add_custom_command (OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_combat_XML_types.cpp
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_combat_XML_types.h
#                    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
#                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_combat.xsd
#                            ${CMAKE_CURRENT_SOURCE_DIR}/rpg_combat.map
#                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
#                    COMMENT "generating XML <--> C++ bindings...")

add_dependencies (${LIBRARY} RPG_Dice RPG_Common RPG_Character RPG_Magic RPG_Item)

#set_target_properties (${LIBRARY} PROPERTIES VERSION 1.0.0 SOVERSION 1)

# add PCH
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

########### install files ###############
install (FILES
         etc/rpg_combat.xsd
         DESTINATION ${INSTALL_SHARE_DIR} COMPONENT config)
install (FILES
         rpg_combat_attackform.h
         rpg_combat_attacksituation.h
         rpg_combat_check.h
         rpg_combat_checktypeunion.h
         rpg_combat_common.h
         rpg_combat_common_tools.h
         rpg_combat_damage.h
         rpg_combat_damagecountermeasure.h
         rpg_combat_damageduration.h
         rpg_combat_damageelement.h
         rpg_combat_damagereductiontype.h
         rpg_combat_damagetypeunion.h
         rpg_combat_defensesituation.h
         rpg_combat_defines.h
         rpg_combat_incl.h
         rpg_combat_otherdamage.h
         rpg_combat_otherdamagetype.h
         rpg_combat_rangedattackproperties.h
         rpg_combat_rangedeffectunion.h
         rpg_combat_specialattack.h
         rpg_combat_specialdamagetype.h
         rpg_combat_XML_parser.h
         rpg_combat_XML_tree.h
         rpg_combat_XML_types.h
         DESTINATION include)
install (TARGETS ${LIBRARY} LIBRARY
         DESTINATION lib)
