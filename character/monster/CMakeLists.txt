#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############
set(RPG_Character_Monster_LIB_SRCS
    rpg_XMLSchema_XML_types.h
    rpg_character_monster.cpp
    rpg_character_monster.h
    rpg_character_monster_XML_parser.cpp
    rpg_character_monster_XML_parser.h
    rpg_character_monster_XML_types.cpp
    rpg_character_monster_XML_types.h
    rpg_character_monster_actiontrigger.h
    rpg_character_monster_advancement.h
    rpg_character_monster_advancementstep.h
    rpg_character_monster_attack.h
    rpg_character_monster_attackaction.h
    rpg_character_monster_common.h
    rpg_character_monster_common_tools.cpp
    rpg_character_monster_common_tools.h
    rpg_character_monster_defenseaction.h
    rpg_character_monster_defines.h
    rpg_character_monster_dictionary.cpp
    rpg_character_monster_dictionary.h
    rpg_character_monster_incl.h
    rpg_character_monster_naturalarmorclass.h
    rpg_character_monster_naturalweapon.h
    rpg_character_monster_organization.h
    rpg_character_monster_organizations.h
    rpg_character_monster_organizationslaverstep.h
    rpg_character_monster_organizationstep.h
    rpg_character_monster_propertiesxml.h
    rpg_character_monster_savingthrowmodifiers.h
    rpg_character_monster_specialabilityprecondition.h
    rpg_character_monster_specialabilityproperties.h
    rpg_character_monster_specialattackprecondition.h
    rpg_character_monster_specialattackproperties.h
    rpg_character_monster_specialbaseproperties.h
    rpg_character_monster_specialdefenseprecondition.h
    rpg_character_monster_specialdefenseproperties.h
    rpg_character_monster_specialpropertytypeunion.h
    rpg_character_monster_summonmonster.h
    rpg_character_monster_weapontypeunion.h)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Character_Monster_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_library(RPG_Character_Monster SHARED ${RPG_Character_Monster_LIB_SRCS})

# add_custom_target(monster_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character_monster.xsd)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character_monster_XML_types.cpp ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character_monster_XML_types.h ${CMAKE_CURRENT_SOURCE_DIR}/rpg_XMLSchema_XML_types.h
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character_monster.xsd ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character_monster.map
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                   COMMENT "generating XML <--> C++ bindings...")

add_dependencies(RPG_Character_Monster RPG_Combat RPG_Character RPG_Item RPG_Magic RPG_Common RPG_Dice)

set_target_properties(RPG_Character_Monster PROPERTIES VERSION 1.0.0 SOVERSION 1)

########### install files ###############
install(TARGETS RPG_Character_Monster LIBRARY
        DESTINATION lib)
install(FILES rpg_character_monster.xsd rpg_character_monster.xml
        DESTINATION share/RPG/config)

install(FILES
        rpg_character_monster.h
        rpg_character_monster_XML_parser.h
        rpg_character_monster_XML_types.h
        rpg_character_monster_actiontrigger.h
        rpg_character_monster_advancement.h
        rpg_character_monster_advancementstep.h
        rpg_character_monster_attack.h
        rpg_character_monster_attackaction.h
        rpg_character_monster_common.h
        rpg_character_monster_common_tools.h
        rpg_character_monster_defenseaction.h
        rpg_character_monster_defines.h
        rpg_character_monster_dictionary.h
        rpg_character_monster_incl.h
        rpg_character_monster_naturalarmorclass.h
        rpg_character_monster_naturalweapon.h
        rpg_character_monster_organization.h
        rpg_character_monster_organizations.h
        rpg_character_monster_organizationslaverstep.h
        rpg_character_monster_organizationstep.h
        rpg_character_monster_propertiesxml.h
        rpg_character_monster_savingthrowmodifiers.h
        rpg_character_monster_specialabilityprecondition.h
        rpg_character_monster_specialabilityproperties.h
        rpg_character_monster_specialattackprecondition.h
        rpg_character_monster_specialattackproperties.h
        rpg_character_monster_specialbaseproperties.h
        rpg_character_monster_specialdefenseprecondition.h
        rpg_character_monster_specialdefenseproperties.h
        rpg_character_monster_specialpropertytypeunion.h
        rpg_character_monster_summonmonster.h
        rpg_character_monster_weapontypeunion.h
        DESTINATION include)
