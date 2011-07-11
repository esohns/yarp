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

#ifndef SDL_GUI_MINIMAPWINDOW_H
#define SDL_GUI_MINIMAPWINDOW_H

#include <rpg_graphics_SDL_window_base.h>

#include <SDL/SDL.h>

#include <ace/Global_Macros.h>

// forward declarations
class RPG_Client_Engine;
class RPG_Engine_Level;

class SDL_GUI_MinimapWindow
 : public RPG_Graphics_SDLWindowBase
{
 public:
  SDL_GUI_MinimapWindow(const RPG_Graphics_SDLWindowBase&, // parent
                        // *NOTE*: offset doesn't include any border(s) !
                        const RPG_Graphics_Offset_t&,      // offset
                        RPG_Client_Engine*,                // engine handle
                        RPG_Engine_Level*);                // state handle
  virtual ~SDL_GUI_MinimapWindow();

  // implement (part of) RPG_Graphics_IWindow
  virtual void draw(SDL_Surface* = NULL,       // target surface (default: screen)
                    const unsigned long& = 0,  // offset x (top-left = [0,0])
                    const unsigned long& = 0); // offset y (top-left = [0,0])
  virtual void handleEvent(const SDL_Event&,      // event
                           RPG_Graphics_IWindow*, // target window (NULL: this)
                           bool&);                // return value: redraw ?

 private:
  typedef RPG_Graphics_SDLWindowBase inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_MinimapWindow());
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_MinimapWindow(const SDL_GUI_MinimapWindow&));
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_MinimapWindow& operator=(const SDL_GUI_MinimapWindow&));

  RPG_Client_Engine* myEngine;
  RPG_Engine_Level*  myLevelState;

  SDL_Surface*       mySurface;
};

#endif // RPG_GRAPHICS_MINIMAP_H
