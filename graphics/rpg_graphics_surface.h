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

#include "rpg_graphics_cursor.h"
#include "rpg_graphics_font.h"
#include "rpg_graphics_image.h"
#include "rpg_graphics_sprite.h"
#include "rpg_graphics_tilegraphic.h"
#include "rpg_graphics_tilesetgraphic.h"
#include "rpg_graphics_graphictypeunion.h"

#include <SDL/SDL.h>

#include <ace/Global_Macros.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Graphics_Surface
{
 public:
  RPG_Graphics_Surface();
  RPG_Graphics_Surface(const RPG_Graphics_Surface&);
  // *NOTE*: if ownership is rejected, the surface will be cached...
  RPG_Graphics_Surface(const RPG_Graphics_GraphicTypeUnion&, // type
                       const bool&);                         // assume ownership ?
  RPG_Graphics_Surface(SDL_Surface*, // SDL surface
                       const bool&); // assume ownership ?
  virtual ~RPG_Graphics_Surface();

  // *NOTE*: if ownership is rejected, the surface will be cached...
  void init(const RPG_Graphics_GraphicTypeUnion&, // type
            const bool&);                         // assume ownership ?
  void init(SDL_Surface*, // SDL surface
            const bool&); // assume ownership ?
  const RPG_Graphics_GraphicTypeUnion type() const;
  // *WARNING*: NEVER SDL_FreeSurface() the return argument !
  SDL_Surface* surface() const;

  // *NOTE*: results need to be SDL_FreeSurface()d !
  // *WARNING*: display format is not available until AFTER SDL_SetVideoMode() !
  static SDL_Surface* load(const std::string&,  // file
                           const bool& = true); // convert to display format ?

  static void savePNG(const SDL_Surface&, // image
                      const std::string&, // file
                      const bool&);       // with alpha ?

  // *NOTE*: results need to be SDL_FreeSurface()d !
  static SDL_Surface* create(const unsigned long&,  // width
                             const unsigned long&); // height
  // *NOTE*: results need to be SDL_FreeSurface()d !
  static SDL_Surface* copy(const SDL_Surface&); // source surface

  // *NOTE*: results need to be SDL_FreeSurface()d !
  static SDL_Surface* get(const unsigned long&, // offset x (top-left == 0,0)
                          const unsigned long&, // offset y (top-left == 0,0)
                          const unsigned long&, // width
                          const unsigned long&, // height
                          const SDL_Surface&);  // source surface
  static void get(const unsigned long&, // offset x (top-left == 0,0)
                  const unsigned long&, // offset y (top-left == 0,0)
                  const bool&,          // blit to target surface ?
                  const SDL_Surface&,   // source surface
                  SDL_Surface&);        // target surface
  static void put(const unsigned long&, // offset x (top left == 0,0)
                  const unsigned long&, // offset y (top left == 0,0)
                  const SDL_Surface&,   // source surface
                  SDL_Surface*);        // target surface (e.g. screen)
  static void putText(const RPG_Graphics_Font&, // font
                      const std::string&,       // string
                      const SDL_Color&,         // color
                      const bool&,              // shade ?
                      const SDL_Color&,         // shade color
                      const unsigned long&,     // offset x (top left == 0,0)
                      const unsigned long&,     // offset y (top left == 0,0)
                      SDL_Surface*);            // target surface (e.g. screen)
  static void putRect(const SDL_Rect&, // rectangle
                      const Uint32&,   // color
                      SDL_Surface*);   // target surface (e.g. screen)

    // *NOTE*: results need to be SDL_FreeSurface()d !
  static SDL_Surface* shade(const SDL_Surface&,               // source surface
                            const Uint8& = SDL_ALPHA_OPAQUE); // alpha (0: transparent --> 255: opaque)
  static void clear(SDL_Surface*); // target surface

  // refresh (partial) screen surface
  static void update(const SDL_Rect&, // "dirty" rectangle
                     SDL_Surface*);   // target surface (e.g. screen)

 private:
  // helper methods
  static SDL_Surface* loadPNG(const std::string&,    // filename
                              const unsigned char*); // source buffer
//                             const unsigned char&); // alpha (0: transparent --> 255: opaque)

  SDL_Surface*                  mySurface;
  RPG_Graphics_GraphicTypeUnion myType;
  bool                          myOwnSurface;
};

#endif
