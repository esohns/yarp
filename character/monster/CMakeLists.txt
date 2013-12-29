cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../common
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../magic
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../item
                    ${CMAKE_CURRENT_SOURCE_DIR}/../
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../combat
                    ${CMAKE_CURRENT_SOURCE_DIR}/../player)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############
set(RPG_Monster_LIB_SRCS
    rpg_XMLSchema_XML_types.h
    rpg_monster.cpp
    rpg_monster.h
    rpg_monster_XML_parser.cpp
    rpg_monster_XML_parser.h
    rpg_monster_XML_types.cpp
    rpg_monster_XML_types.h
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
    rpg_monster_exports.h
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
    rpg_monster_weapontypeunion.h)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Monster_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()
set_source_files_properties(${RPG_Monster_LIB_SRCS} PROPERTIES COMPILE_DEFINITIONS RPG_MONSTER_BUILD_DLL)

add_library(RPG_Monster SHARED ${RPG_Monster_LIB_SRCS})

# add_custom_target(monster_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_monster.xsd)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_monster_XML_types.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/rpg_monster_XML_types.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/rpg_XMLSchema_XML_types.h
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_monster.xsd
                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_monster.map
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                   COMMENT "generating XML <--> C++ bindings...")

add_dependencies(RPG_Monster RPG_Combat RPG_Character RPG_Item RPG_Magic RPG_Common RPG_Dice)

set_target_properties(RPG_Monster PROPERTIES VERSION 1.0.0 SOVERSION 1)

########### install files ###############
install(FILES rpg_monster.xsd rpg_monster.xml
        DESTINATION share/RPG/config)
install(FILES
        rpg_monster.h
        rpg_monster_XML_parser.h
        rpg_monster_XML_types.h
        rpg_monster_actiontrigger.h
        rpg_monster_advancement.h
        rpg_monster_advancementstep.h
        rpg_monster_attack.h
        rpg_monster_attackaction.h
        rpg_monster_common.h
        rpg_monster_common_tools.h
        rpg_monster_defenseaction.h
        rpg_monster_defines.h
        rpg_monster_exports.h
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
        DESTINATION include)
install(TARGETS RPG_Monster LIBRARY DESTINATION lib)
