cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../chance/dice
                    ${CMAKE_CURRENT_SOURCE_DIR}/../common
                    ${CMAKE_CURRENT_SOURCE_DIR}/../character
                    ${CMAKE_CURRENT_SOURCE_DIR}/../magic
                    ${CMAKE_CURRENT_SOURCE_DIR}/../item)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############
set(RPG_Combat_LIB_SRCS
    rpg_XMLSchema_XML_types.h
    rpg_combat_XML_parser.cpp
    rpg_combat_XML_parser.h
    rpg_combat_XML_types.cpp
    rpg_combat_XML_types.h
    rpg_combat_attackform.h
    rpg_combat_attacksituation.h
    rpg_combat_check.h
    rpg_combat_checktypeunion.h
    rpg_combat_common.h
    rpg_combat_exports.h
    rpg_combat_common_tools.cpp
    rpg_combat_common_tools.h
    rpg_combat_damage.h
    rpg_combat_damagecountermeasure.h
    rpg_combat_damageduration.h
    rpg_combat_damagereductiontype.h
    rpg_combat_defensesituation.h
    rpg_combat_incl.h
    rpg_combat_otherdamage.h
    rpg_combat_otherdamagetype.h
    rpg_combat_rangedattackproperties.h
    rpg_combat_rangedeffectunion.h)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Combat_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()
set_source_files_properties(${RPG_Combat_LIB_SRCS} PROPERTIES COMPILE_DEFINITIONS RPG_COMBAT_BUILD_DLL)

add_library(RPG_Combat SHARED ${RPG_Combat_LIB_SRCS})

# add_custom_target(combat_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_combat.xsd)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_combat_XML_types.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/rpg_combat_XML_types.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/rpg_XMLSchema_XML_types.h
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_combat.xsd
                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_combat.map
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                   COMMENT "generating XML <--> C++ bindings...")

add_dependencies(RPG_Combat RPG_Character RPG_Item RPG_Magic RPG_Common RPG_Dice)

set_target_properties(RPG_Combat PROPERTIES VERSION 1.0.0 SOVERSION 1)

########### install files ###############
install(FILES rpg_combat.xsd
        DESTINATION share/RPG/config)
install(FILES
        rpg_combat_XML_parser.h
        rpg_combat_XML_types.h
        rpg_combat_attackform.h
        rpg_combat_attacksituation.h
        rpg_combat_check.h
        rpg_combat_checktypeunion.h
        rpg_combat_common.h
        rpg_combat_common_tools.h
        rpg_combat_exports.h
        rpg_combat_damage.h
        rpg_combat_damagecountermeasure.h
        rpg_combat_damageduration.h
        rpg_combat_damageelement.h
        rpg_combat_damagereductiontype.h
        rpg_combat_damagetypeunion.h
        rpg_combat_defensesituation.h
        rpg_combat_incl.h
        rpg_combat_otherdamage.h
        rpg_combat_otherdamagetype.h
        rpg_combat_rangedattackproperties.h
        rpg_combat_rangedeffectunion.h
        rpg_combat_specialattack.h
        rpg_combat_specialdamagetype.h
        DESTINATION include)
install(TARGETS RPG_Combat LIBRARY DESTINATION lib)
