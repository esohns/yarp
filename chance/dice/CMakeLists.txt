#include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/Threads.cmake)

#include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/ACE.cmake)

########### next target ###############
set (LIBRARY RPG_Dice)
set (${LIBRARY}_SRCS
     rpg_dice.cpp
     rpg_dice.h
     rpg_dice_common.h
     rpg_dice_common_tools.cpp
     rpg_dice_common_tools.h
     rpg_dice_defines.h
     rpg_dice_dietype.h
     rpg_dice_incl.h
     rpg_dice_roll.h
     rpg_dice_valuerange.h
     rpg_dice_XML_parser.cpp
     rpg_dice_XML_parser.h
     rpg_dice_XML_tree.cpp
     rpg_dice_XML_tree.h
     rpg_dice_XML_types.cpp
     rpg_dice_XML_types.h
     stdafx.h)
#     rpg_XMLSchema_XML_tree.h
#     rpg_XMLSchema_XML_types.h)

#set_source_files_properties (${${LIBRARY}_SRCS} PROPERTIES COMPILE_DEFINITIONS RPG_DICE_BUILD_DLL)

add_library (${LIBRARY} STATIC ${${LIBRARY}_SRCS})

if (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/xerces-c/build/msvc/src>)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/xerces-c/src>)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/xsd/include>)
endif (WIN32)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${ACE_INCLUDE_DIR}>)

if (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>)
endif (WIN32)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_party/xsd>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../common>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/timer>)

# add_custom_target(dice_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_dice.xsd)
#add_custom_command (OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_dice_XML_tree.cpp
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_dice_XML_tree.h
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_dice_XML_types.cpp
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_dice_XML_types.h
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_dice_dietype.h
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_dice_incl.h
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_dice_roll.h
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_dice_valuerange.h
#                    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
##                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_dice.xsd
##                            ${CMAKE_CURRENT_SOURCE_DIR}/rpg_dice.map
#                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
#                    COMMENT "generating XML <--> C++ bindings...")

set_target_properties (${LIBRARY} PROPERTIES VERSION 1.0.0 SOVERSION 1)

# add PCH
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

########### install files ###############
install (FILES etc/rpg_dice.xsd
         DESTINATION ${INSTALL_SHARE_DIR} COMPONENT config)
install (FILES
         rpg_dice.h
         rpg_dice_common.h
         rpg_dice_common_tools.h
         rpg_dice_defines.h
         rpg_dice_dietype.h
         rpg_dice_incl.h
         rpg_dice_roll.h
         rpg_dice_valuerange.h
         rpg_dice_XML_parser.h
         rpg_dice_XML_tree.h
         rpg_dice_XML_types.h
         DESTINATION include)
install (TARGETS ${LIBRARY} LIBRARY
         DESTINATION lib)
