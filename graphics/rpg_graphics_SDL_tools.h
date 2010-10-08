/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
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
#ifndef RPG_GRAPHICS_SDL_TOOLS_H
#define RPG_GRAPHICS_SDL_TOOLS_H

#include <SDL/SDL.h>

#include <ace/Global_Macros.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Graphics_SDL_Tools
{
  // grant access to initColors()
  friend class RPG_Graphics_Common_Tools;

 public:
  // some colors
  static Uint32 CLR32_BLACK_A0; // == "transparency"
  static Uint32 CLR32_BLACK_A10;
  static Uint32 CLR32_BLACK_A30;
  static Uint32 CLR32_BLACK_A50;
  static Uint32 CLR32_BLACK_A70;
  static Uint32 CLR32_BLACK_A90;
  static Uint32 CLR32_BLACK;
  static Uint32 CLR32_GREEN;
  static Uint32 CLR32_YELLOW;
  static Uint32 CLR32_ORANGE;
  static Uint32 CLR32_RED;
  static Uint32 CLR32_GRAY20;
  static Uint32 CLR32_GRAY20_A10;
  static Uint32 CLR32_GRAY70;
  static Uint32 CLR32_GRAY77;
  static Uint32 CLR32_PURPLE44;
  static Uint32 CLR32_LIGHTPINK;
  static Uint32 CLR32_LIGHTGREEN;
  static Uint32 CLR32_BROWN;
  static Uint32 CLR32_WHITE;
  static Uint32 CLR32_BLESS_BLUE;
  static Uint32 CLR32_CURSE_RED;
  static Uint32 CLR32_GOLD_SHADE;

  static SDL_Surface* initScreen(const int&,   // width
                                 const int&,   // height
                                 const int&,   // color depth
                                 const bool&,  // double-buffer ?
                                 const bool&); // run fullscreen ?

  static const std::string keyToString(const SDL_keysym&);

  static const SDL_Color colorToSDLColor(const Uint32&,       // RGBA value
                                         const SDL_Surface&); // target surface

  static const SDL_Rect boundingBox(const SDL_Rect&,  // rect 1
                                    const SDL_Rect&); // rect 2
  static const SDL_Rect intersect(const SDL_Rect&,  // rect 1
                                  const SDL_Rect&); // rect 2

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_SDL_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Graphics_SDL_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_SDL_Tools(const RPG_Graphics_SDL_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_SDL_Tools& operator=(const RPG_Graphics_SDL_Tools&));

  // helper methods
  static void initColors();
};

#endif
