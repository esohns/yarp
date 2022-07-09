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

#ifndef RPG_GRAPHICS_TEXTURE_H
#define RPG_GRAPHICS_TEXTURE_H

#include <string>

#include "ace/Global_Macros.h"

#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"

#include "rpg_graphics_common.h"
#include "rpg_graphics_cursor.h"
#include "rpg_graphics_font.h"
#include "rpg_graphics_image.h"
#include "rpg_graphics_sprite.h"
#include "rpg_graphics_tilegraphic.h"
#include "rpg_graphics_tilesetgraphic.h"
#include "rpg_graphics_graphictypeunion.h"

class RPG_Graphics_Texture
{
 public:
  RPG_Graphics_Texture ();
  RPG_Graphics_Texture (const RPG_Graphics_Texture&);
  RPG_Graphics_Texture (SDL_Renderer*, // renderer handle
                        SDL_Surface*); // surface handle
  RPG_Graphics_Texture (SDL_Texture*, // texture handle
                        bool);        // assume ownership ?
  virtual ~RPG_Graphics_Texture ();

  void initialize (SDL_Texture*, // texture handle
                   bool);        // assume ownership ?
  // *WARNING*: NEVER SDL_DestroyTexture() the return argument !
  inline SDL_Texture* texture () const { return myTexture; }

  // *NOTE*: results need to be SDL_DestroyTexture()d !
  static SDL_Texture* load (SDL_Renderer*,       // renderer handle
                            const std::string&); // file

  static void savePNG (const SDL_Texture&, // image
                       const std::string&, // file
                       bool);              // with alpha ?

  // *NOTE*: results need to be SDL_DestroyTexture()d !
  static SDL_Texture* create (SDL_Renderer*, // renderer handle
                              unsigned int,  // width
                              unsigned int); // height
  static void copy (SDL_Renderer*,      // renderer handle
                    const SDL_Texture&, // source surface
                    SDL_Texture&);      // target surface
  // *NOTE*: results need to be SDL_DestroyTexture()d !
  static SDL_Texture* copy (SDL_Renderer*,       // renderer handle
                            const SDL_Texture&); // source surface

  // *NOTE*: results need to be SDL_DestroyTexture()d !
  static SDL_Texture* get (SDL_Renderer*,                // renderer handle
                           const RPG_Graphics_Offset_t&, // offset (top left == 0,0)
                           unsigned int,                 // width
                           unsigned int,                 // height
                           const SDL_Texture&);          // source texture
  static void get (SDL_Renderer*,                // renderer handle
                   const RPG_Graphics_Offset_t&, // offset (top left == 0,0)
                   const SDL_Texture&,           // source texture
                   SDL_Texture&);                // target texture
  static void put (SDL_Renderer*,                // renderer handle
                   const RPG_Graphics_Offset_t&, // offset (top left == 0,0)
                   const SDL_Texture&,           // source texture
                   SDL_Texture*,                 // target texture (e.g. screen)
                   SDL_Rect&);                   // return value: "dirty" region
  static void put (const RPG_Graphics_Offset_t&, // offset (top left == 0,0)
                   const SDL_Surface&,           // source surface
                   SDL_Texture*,                 // target texture (e.g. screen)
                   SDL_Rect&);                   // return value: "dirty" region
  static bool putText (const RPG_Graphics_Font&,     // font
                       const std::string&,           // string
                       const SDL_Color&,             // color
                       bool,                         // shade ?
                       const SDL_Color&,             // shade color
                       const RPG_Graphics_Offset_t&, // offset (top left == 0,0)
                       SDL_Texture*,                 // target surface (e.g. screen)
                       SDL_Rect&);                   // return value: "dirty" region
  static void putRectangle (SDL_Renderer*,   // renderer handle
                            const SDL_Rect&, // rectangle
                            Uint32,          // color
                            SDL_Texture*);   // target texture (e.g. screen)

  static void alpha (Uint8,         // alpha (0: transparent --> 255: opaque)
                     SDL_Texture&); // target texture
  // *NOTE*: results need to be SDL_DestroyTexture()d !
  static SDL_Texture* alpha (SDL_Renderer*,             // renderer handle
                             const SDL_Texture&,        // source texture
                             Uint8 = SDL_ALPHA_OPAQUE); // alpha (0: transparent --> 255: opaque)
  static void clear (SDL_Renderer*,           // renderer handle
                     SDL_Texture*,            // target texture
                     const SDL_Rect* = NULL); // clip area (if any)
  static void fill (SDL_Renderer*,           // renderer handle
                    Uint32,                  // color
                    SDL_Texture*,            // target texture
                    const SDL_Rect* = NULL); // clip area (if any)

 private:
  // helper methods
  static SDL_Texture* loadPNG (SDL_Renderer*,         // renderer handle
                               const std::string&,    // filename
                               FILE*);                // file handle
//                              const unsigned char*); // source buffer
//                              const unsigned char&); // alpha (0: transparent --> 255: opaque)

  SDL_Texture* myTexture;
  bool         myOwnTexture;
};

#endif
