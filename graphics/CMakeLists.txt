include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/Threads.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/ACE.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src/image/scripts/png.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../scripts/xerces-c.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../scripts/sdl.cmake)

########### next target ###############
set (LIBRARY RPG_Graphics)
set (${LIBRARY}_SRCS
     stdafx.h
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
     rpg_graphics_iwindow_base.h
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
if (SDL2_USE)
 set (${LIBRARY}_SRCS
      ${${LIBRARY}_SRCS}
      rpg_graphics_texture.cpp
      rpg_graphics_texture.h)
endif (SDL2_USE)

#set_source_files_properties (${${LIBRARY}_SRCS} PROPERTIES COMPILE_DEFINITIONS RPG_GRAPHICS_BUILD_DLL)

add_library (${LIBRARY} STATIC ${${LIBRARY}_SRCS})

if (LIBPNG_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             "$<BUILD_INTERFACE:${PNG_INCLUDE_DIRS}>")
endif (LIBPNG_SUPPORT)
if (XERCESC_SUPPORT)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${XERCESC_INCLUDE_DIRS}>)
endif (XERCESC_SUPPORT)
if (SDL_SUPPORT)
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

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src>)

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../3rd_party/xsd>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../chance/dice>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../map>)

# add_custom_target(graphics_bindings
#                   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_graphics.xsd)
#add_custom_command (OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/rpg_graphics_XML_types.cpp
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_graphics_XML_types.h
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_graphics_XML_tree.cpp
#                           ${CMAKE_CURRENT_SOURCE_DIR}/rpg_graphics_XML_tree.h
#                    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/bindings.sh
##                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rpg_graphics.xsd
##                            ${CMAKE_CURRENT_SOURCE_DIR}/rpg_graphics.map
#                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts
#                    COMMENT "generating XML <--> C++ bindings...")

add_dependencies (${LIBRARY} RPG_Common RPG_Dice)

#set_target_properties (${LIBRARY} PROPERTIES VERSION 1.0.0 SOVERSION 1)

if (LIBPNG_SUPPORT)
 target_link_libraries (${LIBRARY}
                        ${PNG_LIBRARIES})
endif (LIBPNG_SUPPORT)
if (SDL_SUPPORT)
 target_link_libraries (${LIBRARY}
                        ${SDL_LIBRARIES})
endif (SDL_SUPPORT)

# add PCH
target_precompile_headers (${LIBRARY}
                           PRIVATE stdafx.h)

########### install files ###############
install (FILES
         etc/rpg_graphics.xml
         etc/rpg_graphics.xsd
         DESTINATION ${INSTALL_SHARE_DIR} COMPONENT config)
install (FILES
         rpg_graphics_SDL_tools.h
         rpg_graphics_SDL_window_base.h
         rpg_graphics_SDL_window_sub.h
         rpg_graphics_XML_parser.h
         rpg_graphics_XML_tree.h
         rpg_graphics_XML_types.h
         rpg_graphics_category.h
         rpg_graphics_common.h
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
         rpg_graphics_iwindow_base.h
         rpg_graphics_orientation.h
         rpg_graphics_sprite.h
         rpg_graphics_stairsstyle.h
         rpg_graphics_styleunion.h
         rpg_graphics_surface.h
         rpg_graphics_texture.h
         rpg_graphics_tile.h
         rpg_graphics_tilegraphic.h
         rpg_graphics_tileset.h
         rpg_graphics_tilesetgraphic.h
         rpg_graphics_tilesettype.h
         rpg_graphics_tiletype.h
         rpg_graphics_toplevel.h
         rpg_graphics_wallstyle.h
         DESTINATION include)
install (TARGETS ${LIBRARY} LIBRARY
         DESTINATION lib)

add_subdirectory (share)
