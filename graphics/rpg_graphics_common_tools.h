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

#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "rpg_map_common.h"

#include "rpg_graphics_common.h"
#include "rpg_graphics_incl.h"

// forward declarations
class Common_ILock;

class RPG_Graphics_Common_Tools
{
 public:
  // *WARNING*: needs to be called BEFORE SDL_Init() !
  static void preInitialize ();
  // *WARNING*: needs to be called AFTER SDL_SetVideoMode !
  // *NOTE*: not using SDL at all ? --> provide 'false' as last parameter...
  static bool initialize (const std::string&, // graphics directory
                          unsigned int,       // cache size
                          bool = true);       // initialize SDL ?
  static void finalize ();
  static std::string getGraphicsDirectory ();
  static std::string toString (const struct RPG_Graphics_GraphicTypeUnion&);
  static std::string toString (const struct RPG_Graphics_StyleUnion&);
  static std::string toString (const struct RPG_Graphics_Tile&);
  static std::string toString (const struct RPG_Graphics_TileSet&);
  static std::string toString (const struct RPG_Graphics_ElementTypeUnion&);
  static std::string toString (const RPG_Graphics_Elements_t&);
  static std::string toString (const RPG_Graphics_t&);

  static void graphicToFile (const RPG_Graphics_t&, // graphic
                             std::string&);         // return value: FQ filename
  static RPG_Graphics_TextSize_t textSize (enum RPG_Graphics_Font, // font
                                           const std::string&);    // string

  // *NOTE*: tileset needs to be SDL_FreeSurface()ed by the user !
  static void loadFloorEdgeTileSet (enum RPG_Graphics_EdgeStyle,            // style
                                    struct RPG_Graphics_FloorEdgeTileSet&); // return value: tileset
  static void loadFloorTileSet (enum RPG_Graphics_FloorStyle,       // style
                                struct RPG_Graphics_FloorTileSet&); // return value: tileset
  // *NOTE*: tileset needs to be SDL_FreeSurface()ed by the user !
  static void loadWallTileSet (enum RPG_Graphics_WallStyle,       // style
                               bool,                              // half-height walls ?
                               struct RPG_Graphics_WallTileSet&); // return value: tileset
    // *NOTE*: tileset needs to be SDL_FreeSurface()ed by the user !
  static void loadDoorTileSet (enum RPG_Graphics_DoorStyle,       // style
                               struct RPG_Graphics_DoorTileSet&); // return value: tileset
  // *NOTE*: uncached (!) surfaces need to be SDL_FreeSurface()ed by the user !
  static SDL_Surface* loadGraphic (const struct RPG_Graphics_GraphicTypeUnion&, // type
                                   bool,                                        // convert to display format ?
                                   bool);                                       // cache graphic ?
#if defined (SDL2_USE) || defined (SDL3_USE)
  static SDL_Texture* loadGraphic (SDL_Renderer*,                              // renderer handle
                                   const struct RPG_Graphics_GraphicTypeUnion&/*, // type
                                   bool*/);                                    // cache graphic ?
#endif // SDL2_USE || SDL3_USE

  static SDL_Surface* renderText (enum RPG_Graphics_Font,   // font
                                  const std::string&,       // string
                                  const struct SDL_Color&); // color

  // *NOTE*: source/target image must already be loaded into the framebuffer !
#if defined (SDL_USE)
  static void fade (bool,          // fade-in ? : -out
                    float,         // interval (seconds)
                    Uint32,        // fade to/from color
                    Common_ILock*, // lock interface handle
                    SDL_Surface*); // target surface (e.g. screen)
#elif defined (SDL2_USE) || defined (SDL3_USE)
  static void fade (bool,          // fade-in ? : -out
                    float,         // interval (seconds)
                    Uint32,        // fade to/from color
                    Common_ILock*, // lock interface handle
                    SDL_Window*);  // target window (e.g. screen)
#endif // SDL_USE || SDL2_USE || SDL3_USE

  static struct RPG_Graphics_Style random (const struct RPG_Graphics_Style&); // graphics style

  // coordinate transformations
  static RPG_Graphics_Position_t screenToMap (const RPG_Graphics_Position_t&,  // position (screen coordinates !)
                                              const RPG_Map_Size_t&,           // map size
                                              const RPG_Graphics_Size_t&,      // window size
                                              const RPG_Graphics_Position_t&); // viewport (map coordinates !)
  // *NOTE*: translates the center of a map square to screen coordinates
  static RPG_Graphics_Offset_t mapToScreen (const RPG_Graphics_Position_t&,  // position (map coordinates !)
                                            const RPG_Graphics_Size_t&,      // window size
                                            const RPG_Graphics_Position_t&); // viewport (map coordinates !)

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Graphics_Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~RPG_Graphics_Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Graphics_Common_Tools (const RPG_Graphics_Common_Tools&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Graphics_Common_Tools& operator= (const RPG_Graphics_Common_Tools&))

  // helper methods
  static void initializeStringConversionTables();
  static bool initializeFonts();

  // convert style (wall-, floor-, ...) to appropriate graphic (meta)type
  static struct RPG_Graphics_GraphicTypeUnion styleToType (const struct RPG_Graphics_StyleUnion&, // style (generic)
                                                           bool = false);                         // half-height (wallstyle only) ?

#if defined (SDL_USE)
  static void fade (float,         // interval (seconds)
                    SDL_Surface*,  // target image
                    Common_ILock*, // lock interface handle
                    SDL_Surface*); // target surface (e.g. screen)
#elif defined (SDL2_USE) || defined (SDL3_USE)
  static void fade (float,         // interval (seconds)
                    SDL_Surface*,  // target image
                    Common_ILock*, // lock interface handle
                    SDL_Window*);  // target window (e.g. screen)
#endif // SDL_USE || SDL2_USE || SDL3_USE

  static std::string                  myGraphicsDirectory;

  static ACE_SYNCH_MUTEX              myCacheLock;

  static unsigned int                 myOldestCacheEntry;
  static unsigned int                 myCacheSize;
  static RPG_Graphics_GraphicsCache_t myGraphicsCache;

  static RPG_Graphics_FontCache_t     myFontCache;

  static bool                         myPreInitialized;
  static bool                         myInitialized;
};

#endif
