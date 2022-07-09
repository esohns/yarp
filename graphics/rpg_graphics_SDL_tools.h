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

#include <string>

#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"

#include "ace/Global_Macros.h"

#include "rpg_graphics_common.h"
#include "rpg_graphics_colorname.h"

class RPG_Graphics_SDL_Tools
{
  // grant access to initColors()
  friend class RPG_Graphics_Common_Tools;

 public:
  static bool preInitializeVideo (const struct RPG_Graphics_SDL_VideoConfiguration&, // configuration
                                  const std::string&);                               // window/icon caption
#if defined (SDL_USE)
  static bool initializeVideo (const struct RPG_Graphics_SDL_VideoConfiguration&, // configuration
                               const std::string&,                                // window/icon caption
                               SDL_Surface*&,                                     // return value: window surface
                               const bool& = true);                               // init window surface ?
  static SDL_Surface* initializeScreen (const struct RPG_Graphics_SDL_VideoConfiguration&); // configuration
#elif defined (SDL2_USE)
  static bool initializeVideo (const struct RPG_Graphics_SDL_VideoConfiguration&, // configuration
                               const std::string&,                                // window/icon caption
                               SDL_Window*&,                                      // return value: window handle
                               bool = true);                                      // initialize window surface ?
  static SDL_Window* initializeScreen (const struct RPG_Graphics_SDL_VideoConfiguration&, // configuration
                                       const std::string&);                               // window/icon caption
#endif // SDL_USE || SDL2_USE

#if defined (SDL_USE)
  static std::string keyToString (const SDL_keysym&);
#elif defined (SDL2_USE)
  static std::string keyToString (const SDL_Keysym&);
#endif // SDL_USE || SDL2_USE

  static SDL_Color colorToSDLColor (ACE_UINT32,          // RGBA value
                                    const SDL_Surface&); // target surface
	static Uint32 getColor (const RPG_Graphics_ColorName&, // color name
                          const SDL_PixelFormat&,        // pixel format
                          const float& = 1.0F);          // blend factor (--> opacity)

  static SDL_Rect boundingBox (const SDL_Rect&,  // rect 1
                               const SDL_Rect&); // rect 2
  static SDL_Rect intersect (const SDL_Rect&,  // rect 1
                             const SDL_Rect&); // rect 2
  static bool equal (const SDL_Rect&,  // rect 1
                     const SDL_Rect&); // rect 2
  // *NOTE*: this returns rect1 - rect2; off course, this works only if the
  // result is a rectangular area, otherwise the result will be {0, 0, 0, 0}
  static SDL_Rect difference (const SDL_Rect&,  // rect 1
                              const SDL_Rect&); // rect 2

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Graphics_SDL_Tools())
  ACE_UNIMPLEMENTED_FUNC (~RPG_Graphics_SDL_Tools())
  ACE_UNIMPLEMENTED_FUNC (RPG_Graphics_SDL_Tools(const RPG_Graphics_SDL_Tools&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Graphics_SDL_Tools& operator=(const RPG_Graphics_SDL_Tools&))

  // helper methods
  static void initColors ();

  static bool                    myVideoPreInitialized;
  // predefined colors
  static RPG_Graphics_ColorMap_t myColors;
};

#endif
