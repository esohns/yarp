########### next target ###############
set (LIBRARY RPG_Monster)
set (${LIBRARY}_SRCS
 #    rpg_XMLSchema_XML_types.h
     rpg_monster.cpp
     rpg_monster.h
     rpg_monster_actiontrigger.h
     rpg_monster_advancement.h
     rpg_monster_advancementstep.h
     rpg_monster_attack.h
     rpg_monster_attackaction.h
     rpg_monster_common.h
     rpg_monster_common_tools.cpp
     rpg_monster_common_tools.h
     rpg_monster_defenseaction.h
     rpg_monster_defines.h
  #   rpg_monster_exports.h
     rpg_monster_dictionary.cpp
     rpg_monster_dictionary.h
     rpg_monster_incl.h
     rpg_monster_naturalarmorclass.h
     rpg_monster_naturalweapon.h
     rpg_monster_organization.h
     rpg_monster_organizations.h
     rpg_monster_organizationslaverstep.h
     rpg_monster_organizationstep.h
     rpg_monster_propertiesxml.h
     rpg_monster_savingthrowmodifiers.h
     rpg_monster_size.h
     rpg_monster_specialabilityprecondition.h
     rpg_monster_specialabilityproperties.h
     rpg_monster_specialattackprecondition.h
     rpg_monster_specialattackproperties.h
     rpg_monster_specialbaseproperties.h
     rpg_monster_specialdefenseprecondition.h
     rpg_monster_specialdefenseproperties.h
     rpg_monster_specialpropertytypeunion.h
     rpg_monster_summonmonster.h
     rpg_monster_weapontypeunion.h
     rpg_monster_XML_parser.cpp
     rpg_monster_XML_parser.h
     rpg_monster_XML_tree.cpp
     rpg_monster_XML_tree.h
     rpg_monster_XML_types.cpp
     rpg_monster_XML_types.h)

#set_source_files_properties(${RPG_Monster_LIB_SRCS} PROPERTIES COMPILE_DEFINITIONS RPG_MONSTER_BUILD_DLL)

add_library (${LIBRARY} STATIC ${${LIBRARY}_SRCS})

if (UNIX)
elseif (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/xerces-c/build/src>)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/xerces-c/src>)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/xsd/include>)
endif ()

if (UNIX)
 target_include_directories (${LIBRARY} PUBLIC
#                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE>)
#                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ACE_TAO/ACE>)
                             $<BUILD_INTERFACE:/usr/local/src/ACE_wrappers>)
elseif (WIN32)
 if (DEFINED ENV{ACE_ROOT})
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:$ENV{ACE_ROOT}>)
 else ()
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ACE_TAO/ACE>)
 endif (DEFINED ENV{ACE_ROOT})
endif ()

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../common>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../magic>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../item>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../combat>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../player>)

# add_custom_target(monster_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_monster.xsd)
#add_custom_command (OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_monster_XML_types.cpp
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_monster_XML_types.h
#                    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
##                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_monster.xsd
##                            ${CMAKE_CURRENT_SOURCE_DIR}/rpg_monster.map
#                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
#                    COMMENT "generating XML <--> C++ bindings...")

add_dependencies (${LIBRARY} RPG_Dice RPG_Common RPG_Character RPG_Magic RPG_Item RPG_Combat)

set_target_properties (${LIBRARY} PROPERTIES VERSION 1.0.0 SOVERSION 1)

# add PCH
set_target_properties (${LIBRARY} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${LIBRARY})

########### install files ###############
install (FILES
         rpg_monster.xsd
         rpg_monster.xml
         DESTINATION share/RPG/config)
install (FILES
         rpg_monster.h
         rpg_monster_actiontrigger.h
         rpg_monster_advancement.h
         rpg_monster_advancementstep.h
         rpg_monster_attack.h
         rpg_monster_attackaction.h
         rpg_monster_common.h
         rpg_monster_common_tools.h
         rpg_monster_defenseaction.h
         rpg_monster_defines.h
       #  rpg_monster_exports.h
         rpg_monster_dictionary.h
         rpg_monster_incl.h
         rpg_monster_naturalarmorclass.h
         rpg_monster_naturalweapon.h
         rpg_monster_organization.h
         rpg_monster_organizations.h
         rpg_monster_organizationslaverstep.h
         rpg_monster_organizationstep.h
         rpg_monster_propertiesxml.h
         rpg_monster_savingthrowmodifiers.h
         rpg_monster_size.h
         rpg_monster_specialabilityprecondition.h
         rpg_monster_specialabilityproperties.h
         rpg_monster_specialattackprecondition.h
         rpg_monster_specialattackproperties.h
         rpg_monster_specialbaseproperties.h
         rpg_monster_specialdefenseprecondition.h
         rpg_monster_specialdefenseproperties.h
         rpg_monster_specialpropertytypeunion.h
         rpg_monster_summonmonster.h
         rpg_monster_weapontypeunion.h
         rpg_monster_XML_parser.h
         rpg_monster_XML_tree.h
         rpg_monster_XML_types.h
         DESTINATION include)
install (TARGETS ${LIBRARY} LIBRARY
         DESTINATION lib)
