include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/Threads.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/ACE.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../scripts/xerces-c.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sdl.cmake)

########### next target ###############
set (LIBRARY RPG_Sound)
set (${LIBRARY}_SRCS
     stdafx.h
     rpg_sound.h
     rpg_sound_category.h
     rpg_sound_common.h
     rpg_sound_common_tools.cpp
     rpg_sound_common_tools.h
     rpg_sound_defines.h
     rpg_sound_event_manager.cpp
     rpg_sound_event_manager.h
     rpg_sound_dictionary.cpp
     rpg_sound_dictionary.h
     rpg_sound_event.h
     rpg_sound_incl.h
     rpg_sound_XML_parser.cpp
     rpg_sound_XML_parser.h
     rpg_sound_XML_types.cpp
     rpg_sound_XML_types.h)

#set_source_files_properties (${${LIBRARY}_SRCS} PROPERTIES COMPILE_DEFINITIONS BASEDIR=\"C:\\\")
#set_source_files_properties (${${LIBRARY}_SRCS} PROPERTIES COMPILE_DEFINITIONS RPG_SOUND_BUILD_DLL)

add_library (${LIBRARY} STATIC ${${LIBRARY}_SRCS})

if (XERCESC_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${XERCESC_INCLUDE_DIRS}>)
endif (XERCESC_SUPPORT)
if (SDL_SUPPORT)
 message (STATUS "SDL_INCLUDE_DIRS: ${SDL_INCLUDE_DIRS}")
 target_include_directories (${LIBRARY} PUBLIC
                             ${SDL_INCLUDE_DIRS})
endif (SDL_SUPPORT)
if (WIN32)
 add_compile_definitions (XSD_CXX11)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/xsd/include>)
endif (WIN32)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${ACE_INCLUDE_DIR}>)

if (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/include>)
endif (WIN32)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src/error>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src/task>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src/timer>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../3rd_party/xsd>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../chance/dice>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>)

# add_custom_target(sound_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_sound.xsd)
#add_custom_command (OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_sound_XML_types.cpp
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_sound_XML_types.h
#                    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
##                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_sound.xsd
##                            ${CMAKE_CURRENT_SOURCE_DIR}/rpg_sound.map
#                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
#                    COMMENT "generating XML <--> C++ bindings...")

add_dependencies (${LIBRARY} RPG_Dice RPG_Common)

set_target_properties (${LIBRARY} PROPERTIES VERSION 1.0.0 SOVERSION 1)

# add PCH
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

########### install files ###############
install (FILES
         etc/rpg_sound.xml
         etc/rpg_sound.xsd
         DESTINATION ${INSTALL_SHARE_DIR} COMPONENT config)
install (FILES
         rpg_sound.h
         rpg_sound_category.h
         rpg_sound_common.h
         rpg_sound_common_tools.h
         rpg_sound_defines.h
         rpg_sound_event_manager.h
        # rpg_sound_exports.h
         rpg_sound_dictionary.h
         rpg_sound_event.h
         rpg_sound_incl.h
         rpg_sound_XML_parser.h
         rpg_sound_XML_types.h
         DESTINATION include)
install (TARGETS ${LIBRARY} LIBRARY
         DESTINATION lib)

add_subdirectory (share)
