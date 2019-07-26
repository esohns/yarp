include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/Threads.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/ACE.cmake)

########### next target ###############
set (LIBRARY RPG_Character)
set (${LIBRARY}_SRCS
#     rpg_XMLSchema_XML_tree.h
#     rpg_XMLSchema_XML_types.h
     rpg_character_ability.h
     rpg_character_alignment.h
     rpg_character_alignmentcivic.h
     rpg_character_alignmentethic.h
     rpg_character_attributes.h
     rpg_character_check.h
     rpg_character_checktypeunion.h
     rpg_character_class_common.h
     rpg_character_class_common_tools.cpp
     rpg_character_class_common_tools.h
     rpg_character_classxml.h
     rpg_character_common.h
     rpg_character_common_tools.cpp
     rpg_character_common_tools.h
     rpg_character_encumbrance.h
     rpg_character_equipmentslot.h
 #    rpg_character_exports.h
     rpg_character_feat.h
     rpg_character_feats.h
     rpg_character_gender.h
     rpg_character_incl.h
     rpg_character_metaclass.h
     rpg_character_offhand.h
     rpg_character_race.h
     rpg_character_race_common.h
     rpg_character_race_common_tools.cpp
     rpg_character_race_common_tools.h
     rpg_character_skills.h
     rpg_character_skills_common.h
     rpg_character_skills_common_tools.cpp
     rpg_character_skills_common_tools.h
     rpg_character_skillvalue.h
     rpg_character_XML_parser.cpp
     rpg_character_XML_parser.h
     rpg_character_XML_tools.cpp
     rpg_character_XML_tools.h
     rpg_character_XML_tree.cpp
     rpg_character_XML_tree.h
     rpg_character_XML_types.cpp
     rpg_character_XML_types.h)

#set_source_files_properties(${RPG_Character_LIB_SRCS} PROPERTIES COMPILE_DEFINITIONS RPG_CHARACTER_BUILD_DLL)

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
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../3rd_party/xsd>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../chance>)
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
#target_include_directories (${LIBRARY} PUBLIC
#                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../combat>)

# add_custom_target(character_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character.xsd)
#add_custom_command (OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character_XML_tree.cpp
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character_XML_tree.h
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character_XML_types.cpp
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character_XML_types.h
#                    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
##                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character.xsd
##                            ${CMAKE_CURRENT_SOURCE_DIR}/rpg_character.map
#                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
#                    COMMENT "generating XML <--> C++ bindings...")

add_dependencies (${LIBRARY} RPG_Dice RPG_Common RPG_Magic RPG_Item)

set_target_properties (${LIBRARY} PROPERTIES VERSION 1.0.0 SOVERSION 1)

# add PCH
set_target_properties (${LIBRARY} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${LIBRARY})

########### install files ###############
install (FILES rpg_character.xsd
         DESTINATION share/RPG/config)
install (FILES
#         rpg_XMLSchema_XML_tree.h
         rpg_character_ability.h
         rpg_character_alignment.h
         rpg_character_alignmentcivic.h
         rpg_character_alignmentethic.h
         rpg_character_attributes.h
         rpg_character_check.h
         rpg_character_checktypeunion.h
         rpg_character_class_common.h
         rpg_character_class_common_tools.h
         rpg_character_classxml.h
         rpg_character_common.h
         rpg_character_common_tools.h
         rpg_character_encumbrance.h
         rpg_character_equipmentslot.h
#         rpg_character_exports.h
         rpg_character_feat.h
         rpg_character_feats.h
         rpg_character_gender.h
         rpg_character_incl.h
         rpg_character_metaclass.h
         rpg_character_offhand.h
         rpg_character_race.h
         rpg_character_race_common.h
         rpg_character_race_common_tools.h
         rpg_character_skills.h
         rpg_character_skills_common.h
         rpg_character_skills_common_tools.h
         rpg_character_skillvalue.h
         rpg_character_XML_parser.h
         rpg_character_XML_tools.h
         rpg_character_XML_tree.h
         rpg_character_XML_types.h
         DESTINATION include)
install (TARGETS ${LIBRARY} LIBRARY
         DESTINATION lib)

add_subdirectory (monster)
add_subdirectory (player)
