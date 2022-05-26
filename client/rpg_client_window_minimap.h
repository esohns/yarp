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

#include "ace/Global_Macros.h"

#define _SDL_main_h
#include "SDL.h"

#include "rpg_graphics_SDL_window_base.h"

// forward declaration(s)
class RPG_Client_Engine;
class RPG_Engine;

class RPG_Client_Window_MiniMap
 : public RPG_Graphics_SDLWindowBase
{
 public:
  RPG_Client_Window_MiniMap (const RPG_Graphics_SDLWindowBase&, // parent
                             // *NOTE*: offset doesn't include any border(s) !
                             const RPG_Graphics_Offset_t&,      // offset
                             bool = false);                     // debug ?
  virtual ~RPG_Client_Window_MiniMap ();

  void initialize (RPG_Client_Engine*, // engine
                   RPG_Engine*);       // (level) state

  // implement (part of) RPG_Graphics_IWindowBase
  virtual void draw (SDL_Surface* = NULL, // target surface (default: screen)
                     unsigned int = 0,    // offset x (top-left = [0,0])
                     unsigned int = 0);   // offset y (top-left = [0,0])
  virtual void handleEvent (const SDL_Event&,          // event
                            RPG_Graphics_IWindowBase*, // target window (NULL: this)
                            SDL_Rect&);                // return value: "dirty" region

 private:
  typedef RPG_Graphics_SDLWindowBase inherited;

  ACE_UNIMPLEMENTED_FUNC (RPG_Client_Window_MiniMap ());
  ACE_UNIMPLEMENTED_FUNC (RPG_Client_Window_MiniMap (const RPG_Client_Window_MiniMap&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Client_Window_MiniMap& operator= (const RPG_Client_Window_MiniMap&));

  RPG_Client_Engine* myClient;
  RPG_Engine*        myEngine;
  bool               myDebug;

  SDL_Surface*       myBG;
  SDL_Surface*       mySurface;
};

#endif // RPG_CLIENT_WINDOW_MINIMAP_H
