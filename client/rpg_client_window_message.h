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

#ifndef RPG_CLIENT_WINDOW_MESSAGE_H
#define RPG_CLIENT_WINDOW_MESSAGE_H

#include "rpg_client_common.h"

#include "rpg_graphics_SDL_window_sub.h"
#include "rpg_graphics_font.h"

#include <SDL.h>

#include <ace/Global_Macros.h>
#include <ace/Synch.h>

#include <string>

// forward declarations
class RPG_Common_ILock;

class RPG_Client_Window_Message
 : public RPG_Graphics_SDLWindowSub
{
 public:
  RPG_Client_Window_Message(const RPG_Graphics_SDLWindowBase&); // parent
  virtual ~RPG_Client_Window_Message();

  void init(RPG_Common_ILock*,        // screen lock interface handle
            const RPG_Graphics_Font&, // font
            const unsigned int&);     // lines
  void push(const std::string&); // message

  // implement (part of) RPG_Graphics_IWindowBase
  virtual void draw(SDL_Surface* = NULL,      // target surface (default: screen)
                    const unsigned int& = 0,  // offset x (top-left = [0,0])
                    const unsigned int& = 0); // offset y (top-left = [0,0])
  virtual void handleEvent(const SDL_Event&,          // event
                           RPG_Graphics_IWindowBase*, // target window (NULL: this)
                           SDL_Rect&);                // return value: "dirty" region

 private:
  typedef RPG_Graphics_SDLWindowSub inherited;

  ACE_UNIMPLEMENTED_FUNC(RPG_Client_Window_Message());
  ACE_UNIMPLEMENTED_FUNC(RPG_Client_Window_Message(const RPG_Client_Window_Message&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Client_Window_Message& operator=(const RPG_Client_Window_Message&));

  // helper methods
  void initScrollSpots();

  RPG_Client_Engine*        myClient;
  RPG_Graphics_Font         myFont;
  unsigned int              myNumLines;

  ACE_Thread_Mutex          myLock;
  RPG_Client_MessageStack_t myMessages;

  SDL_Surface*              myBG;
};

#endif // RPG_CLIENT_WINDOW_MESSAGE_H
