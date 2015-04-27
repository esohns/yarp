/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
 *   erik.sohns@web.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef RPG_GRAPHICS_COMMON_TOOLS_H
#define RPG_GRAPHICS_COMMON_TOOLS_H

#include <string>

#include "ace/Global_Macros.h"
#include "ace/Synch.h"

#include "rpg_map_common.h"

#include "rpg_graphics_common.h"
#include "rpg_graphics_exports.h"
#include "rpg_graphics_incl.h"

// forward declarations
class RPG_Common_ILock;

/**
  @author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Graphics_Export RPG_Graphics_Common_Tools
{
 public:
  static void preInitialize ();
  // *WARNING*: needs to be called AFTER SDL_SetVideoMode !
  // *NOTE*: not using SDL at all ? --> provide 'false' as last parameter...
  static bool initialize (const std::string&, // graphics directory
                          unsigned int,       // cache size
                          bool = true);       // initialize SDL ?
  static void finalize ();
  static std::string getGraphicsDirectory();
  static std::string typeToString(const RPG_Graphics_GraphicTypeUnion&);
  static std::string styleToString(const RPG_Graphics_StyleUnion&);
  static std::string tileToString(const RPG_Graphics_Tile&);
  static std::string tileSetToString(const RPG_Graphics_TileSet&);
  static std::string elementTypeToString(const RPG_Graphics_ElementTypeUnion&);
  static std::string elementsToString(const RPG_Graphics_Elements_t&);
  static std::string graphicToString(const RPG_Graphics_t&);

  static void graphicToFile(const RPG_Graphics_t&, // graphic
                            std::string&);         // return value: FQ filename
  static RPG_Graphics_TextSize_t textSize(const RPG_Graphics_Font&, // font
                                          const std::string&);      // string

  // *NOTE*: tileset needs to be SDL_FreeSurface()ed by the user !
  static void loadFloorEdgeTileSet(const RPG_Graphics_EdgeStyle&,     // style
                                   RPG_Graphics_FloorEdgeTileSet_t&); // return value: tileset
  static void loadFloorTileSet(const RPG_Graphics_FloorStyle&, // style
                               RPG_Graphics_FloorTileSet_t&);  // return value: tileset
  // *NOTE*: tileset needs to be SDL_FreeSurface()ed by the user !
  static void loadWallTileSet(const RPG_Graphics_WallStyle&, // style
                              const bool&,                   // half-height walls ?
                              RPG_Graphics_WallTileSet_t&);  // return value: tileset
    // *NOTE*: tileset needs to be SDL_FreeSurface()ed by the user !
  static void loadDoorTileSet(const RPG_Graphics_DoorStyle&, // style
                              RPG_Graphics_DoorTileSet_t&);  // return value: tileset
  // *NOTE*: uncached (!) surfaces need to be SDL_FreeSurface()ed by the user !
  static SDL_Surface* loadGraphic(const RPG_Graphics_GraphicTypeUnion&, // type
                                  const bool&,                          // convert to display format ?
                                  const bool&);                         // cache graphic ?

  static SDL_Surface* renderText(const RPG_Graphics_Font&, // font
                                 const std::string&,       // string
                                 const SDL_Color&);        // color

  // *NOTE*: source/target image must already be loaded into the framebuffer !
  static void fade(const bool&,       // fade in ? (else out)
                   const float&,      // interval (seconds)
                   const Uint32&,     // fade to/from color
                   RPG_Common_ILock*, // lock interface handle
                   SDL_Surface*);     // target surface (e.g. screen)

  static RPG_Graphics_Style random(const RPG_Graphics_Style&); // graphics style

  // coordinate transformations
  static RPG_Graphics_Position_t screen2Map(const RPG_Graphics_Position_t&,  // position (screen coordinates !)
                                            const RPG_Map_Size_t&,           // map size
                                            const RPG_Graphics_Size_t&,      // window size
                                            const RPG_Graphics_Position_t&); // viewport (map coordinates !)
  // *NOTE*: translates the center of a map square to screen coordinates
  static RPG_Graphics_Offset_t map2Screen(const RPG_Graphics_Position_t&,  // position (map coordinates !)
                                          const RPG_Graphics_Size_t&,      // window size
                                          const RPG_Graphics_Position_t&); // viewport (map coordinates !)

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Graphics_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_Common_Tools(const RPG_Graphics_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_Common_Tools& operator=(const RPG_Graphics_Common_Tools&));

  // helper methods
  static void initStringConversionTables();
  static bool initFonts();

  // convert style (wall-, floor-, ...) to appropriate graphic (meta)type
  static RPG_Graphics_GraphicTypeUnion styleToType(const RPG_Graphics_StyleUnion&, // style (generic)
                                                   const bool& = false);           // half-height (wallstyle only) ?

  static void fade(const float&,      // interval (seconds)
                   SDL_Surface*,      // target image
                   RPG_Common_ILock*, // lock interface handle
                   SDL_Surface*);     // target surface (e.g. screen)

  static std::string                  myGraphicsDirectory;

  static ACE_Thread_Mutex             myCacheLock;

  static unsigned int                 myOldestCacheEntry;
  static unsigned int                 myCacheSize;
  static RPG_Graphics_GraphicsCache_t myGraphicsCache;

  static RPG_Graphics_FontCache_t     myFontCache;

  static bool                         myPreInitialized;
  static bool                         myInitialized;
};

#endif
