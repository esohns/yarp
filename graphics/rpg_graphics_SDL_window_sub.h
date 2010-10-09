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
#ifndef RPG_GRAPHICS_SDL_WINDOW_SUB_H
#define RPG_GRAPHICS_SDL_WINDOW_SUB_H

#include "rpg_graphics_common.h"
#include "rpg_graphics_SDL_window_base.h"

#include <SDL/SDL.h>

#include <ace/Global_Macros.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Graphics_SDLWindowSub
 : public RPG_Graphics_SDLWindowBase
{
 public:
  RPG_Graphics_SDLWindowSub(const RPG_Graphics_WindowType&,    // type
                            const RPG_Graphics_SDLWindowBase&, // parent
                            // *NOTE*: offset doesn't include any border(s) !
                            const RPG_Graphics_Offset_t&,      // offset
                            const std::string&,                // title
                            SDL_Surface* = NULL);              // background
  virtual ~RPG_Graphics_SDLWindowSub();

  // close (& destroy)
  void close();

 protected:
  // helper method(s)
  // *NOTE*: make sure setScreen has been invoked
  void saveBG(const RPG_Graphics_WindowSize_t&);

  bool         myHasBeenMarkedForClosure;

 private:
  typedef RPG_Graphics_SDLWindowBase inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_SDLWindowSub());
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_SDLWindowSub(const RPG_Graphics_SDLWindowSub&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_SDLWindowSub& operator=(const RPG_Graphics_SDLWindowSub&));

  // helper method(s)
  // *NOTE*: make sure setScreen has been invoked
  void restoreBG();

  SDL_Surface* myBG;
};

#endif
