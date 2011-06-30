#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_subdirectory(data)

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
    rpg_graphics_export.h
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

add_dependencies(RPG_Graphics RPG_Common RPG_Dice)

set_target_properties(RPG_Graphics PROPERTIES VERSION 1.0.0 SOVERSION 1)
install(TARGETS RPG_Graphics LIBRARY DESTINATION lib)

########### install files ###############

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
        rpg_graphics_export.h
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
