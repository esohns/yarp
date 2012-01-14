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

#include "rpg_graphics_exports.h"
#include "rpg_graphics_common.h"
#include "rpg_graphics_cursor.h"
#include "rpg_graphics_SDL_window_base.h"

#include <SDL/SDL.h>

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Graphics_Export RPG_Graphics_HotSpot
 : public RPG_Graphics_SDLWindowBase
{
 public:
  RPG_Graphics_HotSpot(const RPG_Graphics_SDLWindowBase&, // parent
                       const RPG_Graphics_WindowSize_t&,  // size
                       // *NOTE*: offset doesn't include any border(s) !
                       const RPG_Graphics_Offset_t&,      // offset
                       const RPG_Graphics_Cursor&);       // (hover) cursor graphic
  virtual ~RPG_Graphics_HotSpot();

  const RPG_Graphics_Cursor getCursorType() const;

  // implement (part of) RPG_Graphics_IWindow
  virtual void draw(SDL_Surface* = NULL,       // target surface (default: screen)
                    const unsigned long& = 0,  // offset x (top-left = [0,0])
                    const unsigned long& = 0); // offset y (top-left = [0,0])
  virtual void handleEvent(const SDL_Event&,      // event
                           RPG_Graphics_IWindow*, // target window (NULL: this)
                           bool&);                // return value: redraw ?

  static void init(const RPG_Graphics_SDLWindowBase&, // parent
                   const RPG_Graphics_WindowSize_t&,  // size
                   // *NOTE*: offset doesn't include any border(s) !
                   const RPG_Graphics_Offset_t&,      // offset
                   const RPG_Graphics_Cursor&);       // (hover) cursor graphic

 private:
  typedef RPG_Graphics_SDLWindowBase inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_HotSpot());
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_HotSpot(const RPG_Graphics_HotSpot&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_HotSpot& operator=(const RPG_Graphics_HotSpot&));

  RPG_Graphics_Cursor myCursorType;
  bool                myCursorHasBeenSet;
};

#endif
