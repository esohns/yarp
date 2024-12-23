/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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

#ifndef RPG_GRAPHICS_SURFACE_H
#define RPG_GRAPHICS_SURFACE_H

#include <string>

#include "ace/Global_Macros.h"

#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"

#include "rpg_graphics_common.h"
#include "rpg_graphics_cursor.h"
#include "rpg_graphics_font.h"
#include "rpg_graphics_image.h"
#include "rpg_graphics_sprite.h"
#include "rpg_graphics_tilegraphic.h"
#include "rpg_graphics_tilesetgraphic.h"
#include "rpg_graphics_graphictypeunion.h"

class RPG_Graphics_Surface
{
 public:
  static Uint32 SDL_surface_flags;

  RPG_Graphics_Surface ();
  RPG_Graphics_Surface (const RPG_Graphics_Surface&);
  // *NOTE*: if ownership is rejected, the surface will be cached...
  RPG_Graphics_Surface (const RPG_Graphics_GraphicTypeUnion&, // type
                        bool);                                // assume ownership ?
  RPG_Graphics_Surface (SDL_Surface*, // SDL surface
                        bool);        // assume ownership ?
  virtual ~RPG_Graphics_Surface ();

  // *NOTE*: if ownership is rejected, the surface will be cached...
  void init (const RPG_Graphics_GraphicTypeUnion&, // type
             bool);                                // assume ownership ?
  void init (SDL_Surface*, // SDL surface
             bool);        // assume ownership ?
  inline RPG_Graphics_GraphicTypeUnion type () const { return myType; }
  // *WARNING*: NEVER SDL_FreeSurface() the return argument !
  inline SDL_Surface* surface () const { return mySurface; }

  // clip/unclip the SDL window ("screen");
#if defined (SDL_USE)
  static void clip ();
  static void unclip ();
#elif defined (SDL2_USE) || defined (SDL3_USE)
  static void clip (SDL_Window*); // window handle
  static void unclip (SDL_Window*); // window handle
#endif // SDL_USE || SDL2_USE || SDL3_USE

  // *NOTE*: results need to be SDL_FreeSurface()d !
  // *WARNING*: display format is not available until AFTER SDL_SetVideoMode() !
  static SDL_Surface* load (const std::string&, // file
                            bool);              // convert to display format ?

  static void savePNG (const SDL_Surface&, // image
                       const std::string&, // file
                       bool);              // with alpha ?

  // *NOTE*: results need to be SDL_FreeSurface()d !
  static SDL_Surface* create (unsigned int,  // width
                              unsigned int); // height
  static void copy (const SDL_Surface&, // source surface
                    SDL_Surface&);      // target surface
  // *NOTE*: results need to be SDL_FreeSurface()d !
  static SDL_Surface* copy (const SDL_Surface&); // source surface

  // *NOTE*: results need to be SDL_FreeSurface()d !
  static SDL_Surface* get (const RPG_Graphics_Offset_t&, // offset (top left == 0,0)
                           unsigned int,                 // width
                           unsigned int,                 // height
                           const SDL_Surface&);          // source surface
  static void get (const RPG_Graphics_Offset_t&, // offset (top left == 0,0)
                   const SDL_Surface&,           // source surface
                   SDL_Surface&);                // target surface
  static void put (const RPG_Graphics_Offset_t&, // offset (top left == 0,0)
                   const SDL_Surface&,           // source surface
                   SDL_Surface*,                 // target surface (e.g. screen)
                   struct SDL_Rect&);            // return value: "dirty" region
  static bool putText (enum RPG_Graphics_Font,       // font
                       const std::string&,           // string
                       const struct SDL_Color&,      // color
                       bool,                         // shade ?
                       const struct SDL_Color&,      // shade color
                       const RPG_Graphics_Offset_t&, // offset (top left == 0,0)
                       SDL_Surface*,                 // target surface (e.g. screen)
                       struct SDL_Rect&);            // return value: "dirty" region
  static void putRectangle (const struct SDL_Rect&, // rectangle
                            Uint32,                 // color
                            SDL_Surface*);          // target surface (e.g. screen)

  // *IMPORTANT NOTE*: transparent pixels will remain that way
  static void alpha (Uint8,  // alpha (0: transparent --> 255: opaque)
                     SDL_Surface&); // target surface
  // *NOTE*: results need to be SDL_FreeSurface()d !
  static SDL_Surface* alpha (const SDL_Surface&,        // source surface
                             Uint8 = SDL_ALPHA_OPAQUE); // alpha (0: transparent --> 255: opaque)
  static void clear (SDL_Surface*,                   // target surface
                     const struct SDL_Rect* = NULL); // clip area (if any)
  static void fill (Uint32,                         // color
                    SDL_Surface*,                   // target surface
                    const struct SDL_Rect* = NULL); // clip area (if any)

  // refresh (partial) screen surface
#if defined (SDL_USE)
  static void update (const struct SDL_Rect&, // "dirty" rectangle
                      SDL_Surface*);          // target surface (e.g. screen)
#elif defined (SDL2_USE) || defined (SDL3_USE)
  static void update (const struct SDL_Rect&, // "dirty" rectangle
                      SDL_Window*);           // target window (e.g. screen)
#endif // SDL_USE || SDL2_USE || SDL3_USE

 private:
  // helper methods
  static SDL_Surface* loadPNG (const std::string&, // filename
                               FILE*);             // file handle

  SDL_Surface*                         mySurface;
  struct RPG_Graphics_GraphicTypeUnion myType;
  bool                                 myOwnSurface;

  static struct SDL_Rect               myClipRectangle;
};

#endif
