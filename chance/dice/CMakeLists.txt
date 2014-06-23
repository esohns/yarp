cmake_minimum_required(VERSION 2.8)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############
set(RPG_Dice_LIB_SRCS
    rpg_dice.cpp
    rpg_dice.h
    rpg_dice_common.h
    rpg_dice_common_tools.cpp
    rpg_dice_common_tools.h
    rpg_dice_dietype.h
    rpg_dice_exports.h
    rpg_dice_incl.h
    rpg_dice_roll.h
    rpg_dice_valuerange.h
    rpg_dice_XML_parser.cpp
    rpg_dice_XML_parser.h
    rpg_dice_XML_tree.cpp
    rpg_dice_XML_tree.h
    rpg_dice_XML_types.cpp
    rpg_dice_XML_types.h
    rpg_XMLSchema_XML_tree.h
    rpg_XMLSchema_XML_types.h)

set_source_files_properties(${RPG_Dice_LIB_SRCS} PROPERTIES COMPILE_DEFINITIONS RPG_DICE_BUILD_DLL)
if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Dice_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_library(RPG_Dice SHARED ${RPG_Dice_LIB_SRCS})

# add_custom_target(dice_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_dice.xsd)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_dice_XML_tree.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/rpg_dice_XML_tree.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/rpg_XMLSchema_XML_tree.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/rpg_dice_XML_types.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/rpg_dice_XML_types.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/rpg_XMLSchema_XML_types.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/rpg_dice_dietype.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/rpg_dice_incl.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/rpg_dice_roll.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/rpg_dice_valuerange.h
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_dice.xsd
                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_dice.map
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                   COMMENT "generating XML <--> C++ bindings...")

set_target_properties(RPG_Dice PROPERTIES VERSION 1.0.0 SOVERSION 1)

########### install files ###############
install(FILES rpg_dice.xsd
        DESTINATION share/RPG/config)
install(FILES
        rpg_dice.h
        rpg_dice_common.h
        rpg_dice_common_tools.h
        rpg_dice_dietype.h
	rpg_dice_exports.h
        rpg_dice_incl.h
        rpg_dice_roll.h
        rpg_dice_valuerange.h
        rpg_dice_XML_parser.h
        rpg_dice_XML_tree.h
        rpg_dice_XML_types.h
        DESTINATION include)
install(TARGETS RPG_Dice LIBRARY DESTINATION lib)
