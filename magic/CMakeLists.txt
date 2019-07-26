include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/Threads.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/ACE.cmake)

########### next target ###############
set (LIBRARY RPG_Magic)
set (${LIBRARY}_SRCS
#     rpg_XMLSchema_XML_tree.h
#     rpg_XMLSchema_XML_types.h
     rpg_magic_XML_parser.cpp
     rpg_magic_XML_parser.h
     rpg_magic_XML_tree.cpp
     rpg_magic_XML_tree.h
     rpg_magic_XML_types.cpp
     rpg_magic_XML_types.h
     rpg_magic_abilityclass.h
     rpg_magic_abilitytype.h
     rpg_magic_casterclassunion.h
     rpg_magic_check.h
     rpg_magic_checktypeunion.h
     rpg_magic_common.h
     rpg_magic_common_tools.cpp
     rpg_magic_common_tools.h
     rpg_magic_countermeasure.h
     rpg_magic_defines.h
 #    rpg_magic_exports.h
     rpg_magic_descriptor.h
     rpg_magic_dictionary.cpp
     rpg_magic_dictionary.h
     rpg_magic_domain.h
     rpg_magic_incl.h
     rpg_magic_school.h
     rpg_magic_spell_castingtime.h
     rpg_magic_spell_damagetypeunion.h
     rpg_magic_spell_duration.h
     rpg_magic_spell_durationproperties.h
     rpg_magic_spell_effect.h
     rpg_magic_spell_effectproperties.h
     rpg_magic_spell_level.h
     rpg_magic_spell_precondition.h
     rpg_magic_spell_preconditionproperties.h
     rpg_magic_spell_propertiesxml.h
     rpg_magic_spell_rangeeffect.h
     rpg_magic_spell_rangeproperties.h
     rpg_magic_spell_target.h
     rpg_magic_spell_areaofeffect.h
     rpg_magic_spell_targetproperties.h
     rpg_magic_spell_type.h
     rpg_magic_spelllikeproperties.h
     rpg_magic_spelltype.h
     rpg_magic_subschool.h)

#set_source_files_properties (${${LIBRARY}_LIB_SRCS} PROPERTIES COMPILE_DEFINITIONS RPG_MAGIC_BUILD_DLL)

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
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../chance/dice>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../character>)

# add_custom_target(magic_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_magic.xsd)
#add_custom_command (OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_magic_XML_types.cpp
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_magic_XML_types.h
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_magic_XML_tree.cpp
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_magic_XML_tree.h
#                    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
##                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_magic.xsd
##                            ${CMAKE_CURRENT_SOURCE_DIR}/rpg_magic.map
#                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
#                    COMMENT "generating XML <--> C++ bindings...")

add_dependencies (${LIBRARY} RPG_Dice RPG_Common)
#add_dependencies (${LIBRARY} RPG_Dice RPG_Common RPG_Character)

set_target_properties (${LIBRARY} PROPERTIES VERSION 1.0.0 SOVERSION 1)

# add PCH
set_target_properties (${LIBRARY} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${LIBRARY})

########### install files ###############
install (FILES
         rpg_magic.xml
         rpg_magic.xsd
         DESTINATION share/RPG/config)
install (FILES
         rpg_magic_XML_parser.h
         rpg_magic_XML_tree.h
         rpg_magic_XML_types.h
         rpg_magic_abilityclass.h
         rpg_magic_abilitytype.h
         rpg_magic_casterclassunion.h
         rpg_magic_check.h
         rpg_magic_checktypeunion.h
         rpg_magic_common.h
         rpg_magic_common_tools.h
         rpg_magic_countermeasure.h
         rpg_magic_defines.h
      #   rpg_magic_exports.h
         rpg_magic_descriptor.h
         rpg_magic_dictionary.h
         rpg_magic_domain.h
         rpg_magic_incl.h
         rpg_magic_school.h
         rpg_magic_spell_castingtime.h
         rpg_magic_spell_damagetypeunion.h
         rpg_magic_spell_duration.h
         rpg_magic_spell_durationproperties.h
         rpg_magic_spell_effect.h
         rpg_magic_spell_effectproperties.h
         rpg_magic_spell_level.h
         rpg_magic_spell_precondition.h
         rpg_magic_spell_preconditionproperties.h
         rpg_magic_spell_propertiesxml.h
         rpg_magic_spell_rangeeffect.h
         rpg_magic_spell_rangeproperties.h
         rpg_magic_spell_target.h
         rpg_magic_spell_areaofeffect.h
         rpg_magic_spell_targetproperties.h
         rpg_magic_spell_type.h
         rpg_magic_spelllikeproperties.h
         rpg_magic_spelltype.h
         rpg_magic_subschool.h
         DESTINATION include)
install (TARGETS ${LIBRARY} LIBRARY
         DESTINATION lib)
