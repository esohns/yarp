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

#include "SDL_gui_common.h"

#include "rpg_graphics_SDL_window_sub.h"

#include "SDL.h"

#include "ace/Global_Macros.h"

// forward declarations
class RPG_Engine;

class SDL_GUI_MinimapWindow
 : public RPG_Graphics_SDLWindowSub
{
 public:
  SDL_GUI_MinimapWindow(const RPG_Graphics_SDLWindowBase&, // parent
                        // *NOTE*: offset doesn't include any border(s) !
                        const RPG_Graphics_Offset_t&,      // offset
                        RPG_Engine*);                      // (level) state handle
  virtual ~SDL_GUI_MinimapWindow();

  // implement (part of) RPG_Graphics_IWindow
  virtual RPG_Graphics_Position_t getView() const; // return value: view (map coordinates !)
  virtual void draw(SDL_Surface* = NULL,      // target surface (default: screen)
                    const unsigned int& = 0,  // offset x (top-left = [0,0])
                    const unsigned int& = 0); // offset y (top-left = [0,0])
  virtual void handleEvent(const SDL_Event&,          // event
													 RPG_Graphics_IWindowBase*, // target window (NULL: this)
                           SDL_Rect&);                // return value: "dirty" region

  void init(state_t*,           // state handle
            RPG_Common_ILock*); // screen lock interface handle

 private:
  typedef RPG_Graphics_SDLWindowSub inherited;

  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_MinimapWindow());
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_MinimapWindow(const SDL_GUI_MinimapWindow&));
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_MinimapWindow& operator=(const SDL_GUI_MinimapWindow&));

  RPG_Engine*  myEngine;
  state_t*     myState;

  SDL_Surface* myBG;
  SDL_Surface* mySurface;
};

#endif // RPG_GRAPHICS_MINIMAP_H
