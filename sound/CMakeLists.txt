#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_subdirectory(data)

########### next target ###############
set(RPG_Sound_LIB_SRCS
    rpg_XMLSchema_XML_types.h
    rpg_sound.h
    rpg_sound_XML_parser.cpp
    rpg_sound_XML_parser.h
    rpg_sound_XML_types.cpp
    rpg_sound_XML_types.h
    rpg_sound_category.h
    rpg_sound_common.h
    rpg_sound_common_tools.cpp
    rpg_sound_common_tools.h
    rpg_sound_defines.h
    rpg_sound_dictionary.cpp
    rpg_sound_dictionary.h
    rpg_sound_event.h
    rpg_sound_incl.h)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Sound_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_library(RPG_Sound SHARED ${RPG_Sound_LIB_SRCS})

# add_custom_target(sound_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_sound.xsd)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_sound_XML_types.cpp ${CMAKE_CURRENT_SOURCE_DIR}/rpg_sound_XML_types.h ${CMAKE_CURRENT_SOURCE_DIR}/rpg_XMLSchema_XML_types.h
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_sound.xsd ${CMAKE_CURRENT_SOURCE_DIR}/rpg_sound.map
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                   COMMENT "generating XML <--> C++ bindings...")

add_dependencies(RPG_Sound RPG_Common RPG_Dice)

set_target_properties(RPG_Sound PROPERTIES VERSION 1.0.0 SOVERSION 1)

########### install files ###############
install(TARGETS RPG_Sound LIBRARY
        DESTINATION lib)
install(FILES rpg_sound.xml rpg_sound.xsd
        DESTINATION share/RPG/config)
install(FILES
        rpg_sound.h
        rpg_sound_XML_parser.h
        rpg_sound_XML_types.h
        rpg_sound_category.h
        rpg_sound_common.h
        rpg_sound_common_tools.h
        rpg_sound_defines.h
        rpg_sound_dictionary.h
        rpg_sound_event.h
        rpg_sound_incl.h
        DESTINATION include)
