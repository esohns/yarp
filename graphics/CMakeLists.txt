#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

cmake_minimum_required(VERSION 2.8)

add_subdirectory(data)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../chance/dice")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../common")

########### next target ###############
set(RPG_Graphics_LIB_SRCS
    rpg_XMLSchema_XML_tree.h
    rpg_XMLSchema_XML_types.h
    rpg_graphics_SDL_tools.cpp
    rpg_graphics_SDL_tools.h
    rpg_graphics_SDL_window_base.cpp
    rpg_graphics_SDL_window_base.h
    rpg_graphics_SDL_window_sub.cpp
    rpg_graphics_SDL_window_sub.h
    rpg_graphics_XML_parser.cpp
    rpg_graphics_XML_parser.h
    rpg_graphics_XML_tree.cpp
    rpg_graphics_XML_tree.h
    rpg_graphics_XML_types.cpp
    rpg_graphics_XML_types.h
    rpg_graphics_category.h
    rpg_graphics_common.h
    rpg_graphics_exports.h
    rpg_graphics_common_tools.cpp
    rpg_graphics_common_tools.h
    rpg_graphics_cursor.h
    rpg_graphics_cursor_manager.cpp
    rpg_graphics_cursor_manager.h
    rpg_graphics_defines.h
    rpg_graphics_dictionary.cpp
    rpg_graphics_dictionary.h
    rpg_graphics_doorstyle.h
    rpg_graphics_edgestyle.h
    rpg_graphics_element.h
    rpg_graphics_elementtypeunion.h
    rpg_graphics_floorstyle.h
    rpg_graphics_font.h
    rpg_graphics_graphic.h
    rpg_graphics_graphictypeunion.h
    rpg_graphics_hotspot.cpp
    rpg_graphics_hotspot.h
    rpg_graphics_hotspottype.h
    rpg_graphics_image.h
    rpg_graphics_incl.h
    rpg_graphics_windowtype.h
    rpg_graphics_interfaceelementtype.h
    rpg_graphics_iwindow.h
    rpg_graphics_orientation.h
    rpg_graphics_sprite.h
    rpg_graphics_stairsstyle.h
    rpg_graphics_styleunion.h
    rpg_graphics_surface.cpp
    rpg_graphics_surface.h
    rpg_graphics_tile.h
    rpg_graphics_tilegraphic.h
    rpg_graphics_tileset.h
    rpg_graphics_tilesetgraphic.h
    rpg_graphics_tilesettype.h
    rpg_graphics_tiletype.h
    rpg_graphics_toplevel.cpp
    rpg_graphics_toplevel.h
    rpg_graphics_wallstyle.h)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Graphics_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()
set_source_files_properties(${RPG_Graphics_LIB_SRCS} PROPERTIES COMPILE_DEFINITIONS RPG_GRAPHICS_BUILD_DLL)

add_library(RPG_Graphics SHARED ${RPG_Graphics_LIB_SRCS})

# add_custom_target(graphics_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_graphics.xsd)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_graphics_XML_types.cpp ${CMAKE_CURRENT_SOURCE_DIR}/rpg_graphics_XML_types.h ${CMAKE_CURRENT_SOURCE_DIR}/rpg_XMLSchema_XML_types.h ${CMAKE_CURRENT_SOURCE_DIR}/rpg_graphics_XML_tree.cpp ${CMAKE_CURRENT_SOURCE_DIR}/rpg_graphics_XML_tree.h ${CMAKE_CURRENT_SOURCE_DIR}/rpg_XMLSchema_XML_tree.h
                   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_graphics.xsd ${CMAKE_CURRENT_SOURCE_DIR}/rpg_graphics.map
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                   COMMENT "generating XML <--> C++ bindings...")

add_dependencies(RPG_Graphics RPG_Common RPG_Dice)

set_target_properties(RPG_Graphics PROPERTIES VERSION 1.0.0 SOVERSION 1)

########### install files ###############
install(TARGETS RPG_Graphics LIBRARY
        DESTINATION lib)
install(FILES
        rpg_graphics.xml
        rpg_graphics.xsd
        DESTINATION share/RPG/config)
install(FILES
        rpg_graphics_SDL_tools.h
        rpg_graphics_SDL_window_base.h
        rpg_graphics_SDL_window_sub.h
        rpg_graphics_XML_parser.h
        rpg_graphics_XML_tree.h
        rpg_graphics_XML_types.h
        rpg_graphics_category.h
        rpg_graphics_common.h
        rpg_graphics_exports.h
        rpg_graphics_common_tools.h
        rpg_graphics_cursor.h
        rpg_graphics_cursor_manager.h
        rpg_graphics_defines.h
        rpg_graphics_dictionary.h
        rpg_graphics_doorstyle.h
        rpg_graphics_edgestyle.h
        rpg_graphics_element.h
        rpg_graphics_elementtypeunion.h
        rpg_graphics_floorstyle.h
        rpg_graphics_font.h
        rpg_graphics_graphic.h
        rpg_graphics_graphictypeunion.h
        rpg_graphics_hotspot.h
        rpg_graphics_hotspottype.h
        rpg_graphics_image.h
        rpg_graphics_incl.h
        rpg_graphics_windowtype.h
        rpg_graphics_interfaceelementtype.h
        rpg_graphics_iwindow.h
        rpg_graphics_orientation.h
        rpg_graphics_sprite.h
        rpg_graphics_stairsstyle.h
        rpg_graphics_styleunion.h
        rpg_graphics_surface.h
        rpg_graphics_tile.h
        rpg_graphics_tilegraphic.h
        rpg_graphics_tileset.h
        rpg_graphics_tilesetgraphic.h
        rpg_graphics_tilesettype.h
        rpg_graphics_tiletype.h
        rpg_graphics_toplevel.h
        rpg_graphics_wallstyle.h
        DESTINATION include)
