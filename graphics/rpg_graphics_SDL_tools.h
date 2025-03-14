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
                               bool = true);                                      // initialize window surface ?
  static SDL_Surface* initializeScreen (const struct RPG_Graphics_SDL_VideoConfiguration&); // configuration
#elif defined (SDL2_USE) || defined (SDL3_USE)
  static bool initializeVideo (const struct RPG_Graphics_SDL_VideoConfiguration&, // configuration
                               const std::string&,                                // window/icon caption
                               SDL_Window*&,                                      // return value: window handle
                               SDL_Renderer*&,                                    // return value: renderer handle
                               SDL_GLContext&,                                    // return value: main GL context handle
                               bool = true);                                      // initialize window surface ?
  static bool initializeScreen (const struct RPG_Graphics_SDL_VideoConfiguration&, // configuration
                                const std::string&,                                // window/icon caption
                                SDL_Window*&,                                      // return value: main window handle
                                SDL_Renderer*&,                                    // return value: renderer handle
                                SDL_GLContext&);                                   // return value: main GL context handle
#endif // SDL_USE || SDL2_USE || SDL3_USE

#if defined (SDL_USE)
  static std::string keyToString (const SDL_keysym&);
#elif defined (SDL2_USE) || defined (SDL3_USE)
  static std::string keyToString (const SDL_Keysym&);
#endif // SDL_USE || SDL2_USE || SDL3_USE

  static struct SDL_Color colorToSDLColor (Uint32,                         // RGBA value (SDL format)
                                           const struct SDL_PixelFormat&); // (target) pixel format
  static Uint32 getColor (enum RPG_Graphics_ColorName,   // color name
                          const struct SDL_PixelFormat&, // pixel format
                          float = 1.0f);                 // blend factor (--> opacity)

  static SDL_Rect boundingBox (const struct SDL_Rect&,  // rect 1
                               const struct SDL_Rect&); // rect 2
  static SDL_Rect intersect (const struct SDL_Rect&,    // rect 1
                             const struct SDL_Rect&);   // rect 2
  static bool equal (const struct SDL_Rect&,            // rect 1
                     const struct SDL_Rect&);           // rect 2
  // *NOTE*: this returns rect1 - rect2; off course, this works only if the
  // result is a rectangular area, otherwise the result will be {0, 0, 0, 0}
  static SDL_Rect difference (const struct SDL_Rect&, // rect 1
                              const struct SDL_Rect&); // rect 2

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Graphics_SDL_Tools())
  ACE_UNIMPLEMENTED_FUNC (~RPG_Graphics_SDL_Tools())
  ACE_UNIMPLEMENTED_FUNC (RPG_Graphics_SDL_Tools(const RPG_Graphics_SDL_Tools&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Graphics_SDL_Tools& operator=(const RPG_Graphics_SDL_Tools&))

  // helper methods
  static void initializeColors ();

  static bool                    myVideoPreInitialized;
  // predefined colors
  static RPG_Graphics_ColorMap_t myColors;
};

#endif
