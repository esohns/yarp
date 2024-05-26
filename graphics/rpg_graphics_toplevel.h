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

#ifndef RPG_GRAPHICS_TOPLEVEL_H
#define RPG_GRAPHICS_TOPLEVEL_H

#include <string>

// #define _SDL_main_h
// #define SDL_main_h_
// #include "SDL.h"

#include "ace/Global_Macros.h"

#include "rpg_graphics_common.h"
// #include "rpg_graphics_cursor.h"
// #include "rpg_graphics_font.h"
// #include "rpg_graphics_image.h"
// #include "rpg_graphics_sprite.h"
// #include "rpg_graphics_tilegraphic.h"
// #include "rpg_graphics_tilesetgraphic.h"
#include "rpg_graphics_graphictypeunion.h"
#include "rpg_graphics_SDL_window_base.h"

class RPG_Graphics_TopLevel
 : public RPG_Graphics_SDLWindowBase
{
  typedef RPG_Graphics_SDLWindowBase inherited;

 public:
  RPG_Graphics_TopLevel (const RPG_Graphics_Size_t&,                  // size
                         const struct RPG_Graphics_GraphicTypeUnion&, // (element) type
                         const std::string&);                         // title
  virtual ~RPG_Graphics_TopLevel ();

 protected:
  // window element graphic(s)
  struct RPG_Graphics_GraphicTypeUnion myElementGraphicsType;
  RPG_Graphics_InterfaceElements_t     myElementGraphics;

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Graphics_TopLevel ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Graphics_TopLevel (const RPG_Graphics_TopLevel&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Graphics_TopLevel& operator= (const RPG_Graphics_TopLevel&))

  // helper methods
  bool loadGraphics (const struct RPG_Graphics_GraphicTypeUnion&); // (element) type
};

#endif
