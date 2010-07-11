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

#include "rpg_graphics_type.h"

#include <SDL/SDL.h>

#include <ace/Global_Macros.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Graphics_Surface
{
 public:
  // *NOTE*: if ownership is rejected, the surface will be cached...
  RPG_Graphics_Surface(const RPG_Graphics_Type&, // graphic type
                       const bool&);             // assume ownership ?
  RPG_Graphics_Surface(SDL_Surface*, // SDL surface
                       const bool&); // assume ownership ?
  virtual ~RPG_Graphics_Surface();

  // *NOTE*: result need to be SDL_FreeSurface()d !
  // *WARNING*: display format is not available until AFTER SDL_SetVideoMode() !
  static SDL_Surface* load(const std::string&,  // file
                           const bool& = true); // convert to display format ?

  static void savePNG(const SDL_Surface&, // image
                      const std::string&, // file
                      const bool&);       // with alpha ?

  static SDL_Surface* get(const unsigned long&, // offset x (top-left == 0,0)
                          const unsigned long&, // offset y (top-left == 0,0)
                          const unsigned long&, // width
                          const unsigned long&, // height
                          const SDL_Surface&);  // source surface
  static void put(const unsigned long&, // offset x (top left == 0,0)
                  const unsigned long&, // offset y (top left == 0,0)
                  const SDL_Surface&,   // source surface
                  SDL_Surface*);        // target surface (e.g. screen)
  static void putText(const RPG_Graphics_Type&, // font
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

    // *NOTE*: results need to be SDL_FreeSurface()ed by the user !
  static SDL_Surface* shade(const SDL_Surface&,               // source surface
                            const Uint8& = SDL_ALPHA_OPAQUE); // alpha (0: transparent --> 255: opaque)

 protected:
  // safety measures
  RPG_Graphics_Surface();
  // *NOTE*: if ownership is rejected, the surface will be cached...
  void init(const RPG_Graphics_Type&, // graphic type
            const bool&);             // assume ownership ?
  void init(SDL_Surface*, // SDL surface
            const bool&); // assume ownership ?

  SDL_Surface* mySurface;

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_Surface(const RPG_Graphics_Surface&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_Surface& operator=(const RPG_Graphics_Surface&));

  // helper methods
  static SDL_Surface* loadPNG(const unsigned char*); // source buffer
//                             const unsigned char&); // alpha (0: transparent --> 255: opaque)

  bool         myOwnSurface;
};

#endif
