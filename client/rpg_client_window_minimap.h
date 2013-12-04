/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RPG_CLIENT_WINDOW_MINIMAP_H
#define RPG_CLIENT_WINDOW_MINIMAP_H

#include "rpg_graphics_SDL_window_base.h"

#include <SDL.h>

#include <ace/Global_Macros.h>

// forward declaration(s)
class RPG_Client_Engine;
class RPG_Engine;

class RPG_Client_Window_MiniMap
 : public RPG_Graphics_SDLWindowBase
{
 public:
  RPG_Client_Window_MiniMap(const RPG_Graphics_SDLWindowBase&, // parent
                            // *NOTE*: offset doesn't include any border(s) !
                            const RPG_Graphics_Offset_t&);     // offset
  virtual ~RPG_Client_Window_MiniMap();

  void init(RPG_Client_Engine*, // engine
            RPG_Engine*);       // (level) state

  // implement (part of) RPG_Graphics_IWindow
  virtual void draw(SDL_Surface* = NULL,      // target surface (default: screen)
                    const unsigned int& = 0,  // offset x (top-left = [0,0])
                    const unsigned int& = 0); // offset y (top-left = [0,0])
  virtual void handleEvent(const SDL_Event&,      // event
                           RPG_Graphics_IWindow*, // target window (NULL: this)
                           bool&);                // return value: redraw ?

 private:
  typedef RPG_Graphics_SDLWindowBase inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Client_Window_MiniMap());
  ACE_UNIMPLEMENTED_FUNC(RPG_Client_Window_MiniMap(const RPG_Client_Window_MiniMap&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Client_Window_MiniMap& operator=(const RPG_Client_Window_MiniMap&));

  RPG_Client_Engine* myClient;
  RPG_Engine*        myEngine;

  SDL_Surface*       myBG;
  SDL_Surface*       mySurface;
};

#endif // RPG_CLIENT_WINDOW_MINIMAP_H
