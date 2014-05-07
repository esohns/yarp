METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4 -I .
AM_CPPFLAGS = \
  -DRPG_GRAPHICS_BUILD_DLL \
  -I$(srcdir)/../common \
  -I$(srcdir)/../map

SUBDIRS = scripts data

noinst_HEADERS = \
  stdafx.h

include_HEADERS = \
  rpg_graphics_SDL_tools.h \
  rpg_graphics_SDL_window_base.h \
  rpg_graphics_SDL_window_sub.h \
  rpg_graphics_XML_parser.h \
  rpg_graphics_XML_tree.h \
  rpg_graphics_XML_types.h \
  rpg_graphics_category.h \
  rpg_graphics_colorname.h \
  rpg_graphics_colorrgba.h \
  rpg_graphics_common.h \
  rpg_graphics_common_tools.h \
  rpg_graphics_cursor.h \
  rpg_graphics_cursor_manager.h \
  rpg_graphics_defines.h \
  rpg_graphics_dictionary.h \
  rpg_graphics_doorstyle.h \
  rpg_graphics_edgestyle.h \
  rpg_graphics_element.h \
  rpg_graphics_elementtypeunion.h \
  rpg_graphics_floorstyle.h \
  rpg_graphics_font.h \
  rpg_graphics_graphic.h \
  rpg_graphics_graphictypeunion.h \
  rpg_graphics_hotspot.h \
  rpg_graphics_hotspottype.h \
  rpg_graphics_image.h \
  rpg_graphics_incl.h \
  rpg_graphics_interfaceelementtype.h \
  rpg_graphics_iwindow.h \
  rpg_graphics_mapstyle.h \
  rpg_graphics_orientation.h \
  rpg_graphics_sprite.h \
  rpg_graphics_stairsstyle.h \
  rpg_graphics_styleunion.h \
  rpg_graphics_surface.h \
  rpg_graphics_tile.h \
  rpg_graphics_tilegraphic.h \
  rpg_graphics_tileset.h \
  rpg_graphics_tilesetgraphic.h \
  rpg_graphics_tilesettype.h \
  rpg_graphics_tiletype.h \
  rpg_graphics_toplevel.h \
  rpg_graphics_wallstyle.h \
  rpg_graphics_exports.h \
  rpg_graphics_windowtype.h

lib_LTLIBRARIES = libRPG_Graphics.la

libRPG_Graphics_la_SOURCES = \
  rpg_graphics_SDL_tools.cpp \
  rpg_graphics_SDL_tools.h \
  rpg_graphics_SDL_window_base.cpp \
  rpg_graphics_SDL_window_base.h \
  rpg_graphics_SDL_window_sub.cpp \
  rpg_graphics_SDL_window_sub.h \
  rpg_graphics_XML_parser.cpp \
  rpg_graphics_XML_parser.h \
  rpg_graphics_XML_tree.cpp \
  rpg_graphics_XML_tree.h \
  rpg_graphics_XML_types.cpp \
  rpg_graphics_XML_types.h \
  rpg_graphics_category.h \
  rpg_graphics_colorname.h \
  rpg_graphics_colorrgba.h \
  rpg_graphics_common.h \
  rpg_graphics_common_tools.cpp \
  rpg_graphics_common_tools.h \
  rpg_graphics_cursor.h \
  rpg_graphics_cursor_manager.cpp \
  rpg_graphics_cursor_manager.h \
  rpg_graphics_defines.h \
  rpg_graphics_dictionary.cpp \
  rpg_graphics_dictionary.h \
  rpg_graphics_doorstyle.h \
  rpg_graphics_edgestyle.h \
  rpg_graphics_element.h \
  rpg_graphics_elementtypeunion.h \
  rpg_graphics_floorstyle.h \
  rpg_graphics_font.h \
  rpg_graphics_graphic.h \
  rpg_graphics_graphictypeunion.h \
  rpg_graphics_hotspot.cpp \
  rpg_graphics_hotspot.h \
  rpg_graphics_hotspottype.h \
  rpg_graphics_image.h \
  rpg_graphics_incl.h \
  rpg_graphics_interfaceelementtype.h \
  rpg_graphics_iwindow.h \
  rpg_graphics_mapstyle.h \
  rpg_graphics_orientation.h \
  rpg_graphics_sprite.h \
  rpg_graphics_stairsstyle.h \
  rpg_graphics_styleunion.h \
  rpg_graphics_surface.cpp \
  rpg_graphics_surface.h \
  rpg_graphics_tile.h \
  rpg_graphics_tilegraphic.h \
  rpg_graphics_tileset.h \
  rpg_graphics_tilesetgraphic.h \
  rpg_graphics_tilesettype.h \
  rpg_graphics_tiletype.h \
  rpg_graphics_toplevel.cpp \
  rpg_graphics_toplevel.h \
  rpg_graphics_wallstyle.h \
  rpg_graphics_exports.h \
  rpg_graphics_windowtype.h

libRPG_Graphics_la_CPPFLAGS = $(AM_CPPFLAGS) $(pkg_libs_CFLAGS)
libRPG_Graphics_la_LDFLAGS  = $(pkg_libs_LDFLAGS)
libRPG_Graphics_la_LIBADD   = $(pkg_libs_LIBS)

pkgdatadir = $(datadir)/@META_PACKAGE@
configdir = $(pkgdatadir)/config
config_DATA = \
  rpg_graphics.xml \
  rpg_graphics.xsd

EXTRA_DIST = \
  $(config_DATA) \
  rpg_graphics.map
