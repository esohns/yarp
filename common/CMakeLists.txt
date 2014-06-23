cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../chance/dice)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############
set(RPG_Common_LIB_SRCS
    rpg_XMLSchema_XML_tree.h
    rpg_XMLSchema_XML_types.h
    rpg_common.h
    rpg_common_XML_parser.cpp
    rpg_common_XML_parser.h
    rpg_common_XML_tree.cpp
    rpg_common_XML_tree.h
    rpg_common_XML_types.cpp
    rpg_common_XML_types.h
    rpg_common_actiontype.h
    rpg_common_amount.h
    rpg_common_areaofeffect.h
    rpg_common_attribute.h
    rpg_common_basechecktypeunion.h
    rpg_common_camp.h
    rpg_common_checktype.h
    rpg_common_climate.h
    rpg_common_condition.h
    rpg_common_countermeasure.h
    rpg_common_creaturemetatype.h
    rpg_common_creaturesubtype.h
    rpg_common_creaturetype.h
    rpg_common_defines.h
    rpg_common_exports.h
    rpg_common_duration.h
    rpg_common_effectduration.h
    rpg_common_effecttype.h
    rpg_common_environment.h
    rpg_common_environment_XML_tree.cpp
    rpg_common_environment_XML_tree.h
    rpg_common_environment_XML_types.cpp
    rpg_common_environment_XML_types.h
    rpg_common_environment_incl.h
    rpg_common_file_tools.cpp
    rpg_common_file_tools.h
    rpg_common_icontrol.h
    rpg_common_idumpstate.h
    rpg_common_incl.h
    rpg_common_irefcount.h
    rpg_common_istatistic.h
    rpg_common_itimer.h
    rpg_common_macros.h
    rpg_common_physicaldamagetype.h
    rpg_common_plane.h
    rpg_common_referencecounter.cpp
    rpg_common_referencecounter.h
    rpg_common_savereductiontype.h
    rpg_common_savingthrow.h
    rpg_common_savingthrowcheck.h
    rpg_common_size.h
    rpg_common_skill.h
    rpg_common_subclass.h
    rpg_common_terrain.h
    rpg_common_timeofday.h
    rpg_common_timer_manager.cpp
    rpg_common_timer_manager.h
    rpg_common_timerhandler.cpp
    rpg_common_timerhandler.h
    rpg_common_tools.cpp
    rpg_common_tools.h
    rpg_common_usage.h
    rpg_common_xsderrorhandler.cpp
    rpg_common_xsderrorhandler.h)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Common_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()
set_source_files_properties(${RPG_Common_LIB_SRCS} PROPERTIES COMPILE_DEFINITIONS RPG_COMMON_BUILD_DLL)

add_library(RPG_Common SHARED ${RPG_Common_LIB_SRCS})

# add_custom_target(common_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_common.xsd)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_common_XML_types.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/rpg_common_XML_types.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/rpg_XMLSchema_XML_types.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/rpg_common_XML_tree.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/rpg_common_XML_tree.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/rpg_XMLSchema_XML_tree.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/rpg_common_environment_XML_types.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/rpg_common_environment_XML_types.h
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_common.xsd
                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_common.map
                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_common_environment.xsd
                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_common_environment.map
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                   COMMENT "generating XML <--> C++ bindings...")

add_dependencies(RPG_Common RPG_Dice)

set_target_properties(RPG_Common PROPERTIES VERSION 1.0.0 SOVERSION 1)

########### install files ###############
install(FILES rpg_common.xsd rpg_common_environment.xsd
        DESTINATION share/RPG/config)
install(FILES
        rpg_XMLSchema_XML_tree.h
        rpg_XMLSchema_XML_types.h
        rpg_common.h
        rpg_common_XML_parser.h
        rpg_common_XML_tree.h
        rpg_common_XML_types.h
        rpg_common_actiontype.h
        rpg_common_amount.h
        rpg_common_areaofeffect.h
        rpg_common_attribute.h
        rpg_common_basechecktypeunion.h
        rpg_common_camp.h
        rpg_common_checktype.h
        rpg_common_climate.h
        rpg_common_condition.h
        rpg_common_countermeasure.h
        rpg_common_creaturemetatype.h
        rpg_common_creaturesubtype.h
        rpg_common_creaturetype.h
        rpg_common_defines.h
        rpg_common_exports.h
        rpg_common_duration.h
        rpg_common_effecttype.h
        rpg_common_effectduration.h
        rpg_common_environment.h
        rpg_common_environment_XML_tree.h
        rpg_common_environment_XML_types.h
        rpg_common_environment_incl.h
        rpg_common_file_tools.h
        rpg_common_icontrol.h
        rpg_common_idumpstate.h
        rpg_common_incl.h
        rpg_common_irefcount.h
        rpg_common_istatistic.h
        rpg_common_itimer.h
        rpg_common_macros.h
        rpg_common_physicaldamagetype.h
        rpg_common_plane.h
        rpg_common_referencecounter.h
        rpg_common_savereductiontype.h
        rpg_common_savingthrow.h
        rpg_common_savingthrowcheck.h
        rpg_common_size.h
        rpg_common_skill.h
        rpg_common_subclass.h
        rpg_common_terrain.h
        rpg_common_timeofday.h
        rpg_common_timer_manager.h
        rpg_common_timerhandler.h
        rpg_common_tools.h
        rpg_common_usage.h
        rpg_common_xsderrorhandler.h
        DESTINATION include)
install(TARGETS RPG_Common LIBRARY DESTINATION lib)
