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
#ifndef RPG_GRAPHICS_HOTSPOT_H
#define RPG_GRAPHICS_HOTSPOT_H

#include "rpg_graphics_SDLwindow.h"
#include "rpg_graphics_common.h"
#include "rpg_graphics_type.h"

#include <SDL/SDL.h>

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Graphics_HotSpot
 : public RPG_Graphics_SDLWindow
{
 public:
  RPG_Graphics_HotSpot(const RPG_Graphics_SDLWindow&,    // parent
                       const RPG_Graphics_WindowSize_t&, // size
                       // *NOTE*: offset doesn't include any border(s) !
                       const RPG_Graphics_Offset_t&,     // offset
                       const RPG_Graphics_Type&);        // (hover) cursor graphic
  virtual ~RPG_Graphics_HotSpot();

  // implement (part of) RPG_Graphics_IWindow
  virtual void handleEvent(const SDL_Event&, // event
                           bool&);           // return value: redraw ?
  virtual void draw(SDL_Surface*,                  // target surface (screen !)
                    const RPG_Graphics_Offset_t&); // offset

  static void init(const RPG_Graphics_SDLWindow&,    // parent
                   const RPG_Graphics_WindowSize_t&, // size
                   // *NOTE*: offset doesn't include any border(s) !
                   const RPG_Graphics_Offset_t&,     // offset
                   const RPG_Graphics_Type&);        // (hover) cursor graphic

 private:
  typedef RPG_Graphics_SDLWindow inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_HotSpot());
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_HotSpot(const RPG_Graphics_HotSpot&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_HotSpot& operator=(const RPG_Graphics_HotSpot&));

  // helper methods
  const bool loadGraphics(const RPG_Graphics_Type&); // (hover) cursor graphic

  SDL_Surface* myCursor;
  bool         myInitialized;
};

#endif
