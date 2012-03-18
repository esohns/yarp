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

#include "rpg_graphics_exports.h"
#include "rpg_graphics_common.h"
#include "rpg_graphics_cursor.h"
#include "rpg_graphics_font.h"
#include "rpg_graphics_image.h"
#include "rpg_graphics_tilegraphic.h"
#include "rpg_graphics_tilesetgraphic.h"
#include "rpg_graphics_graphictypeunion.h"
#include "rpg_graphics_SDL_window_base.h"

#include <SDL/SDL.h>

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Graphics_Export RPG_Graphics_TopLevel
 : public RPG_Graphics_SDLWindowBase
{
 public:
  RPG_Graphics_TopLevel(const RPG_Graphics_Size_t&,           // size
                        const RPG_Graphics_GraphicTypeUnion&, // (element) type
                        const std::string&);                  // title
//                         SDL_Surface* = NULL);                 // background
  virtual ~RPG_Graphics_TopLevel();

  // implement (part of) RPG_Graphics_IWindow
  // *IMPORTANT NOTE*: dummy stub --> DO NOT CALL
  virtual const RPG_Graphics_Position_t getView() const; // return value: view (map coordinates !)

//   // realize child window
//   void child(const RPG_Graphics_WindowSize_t&, // size
//              const RPG_Graphics_WindowType&,   // type
//              // *NOTE*: offset doesn't include any border(s) !
//              const RPG_Graphics_Offset_t&,     // offset
//              SDL_Surface* = NULL);             // background

 protected:
  // window element graphic(s)
  RPG_Graphics_GraphicTypeUnion    myElementGraphicsType;
  RPG_Graphics_InterfaceElements_t myElementGraphics;

 private:
  typedef RPG_Graphics_SDLWindowBase inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_TopLevel());
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_TopLevel(const RPG_Graphics_TopLevel&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_TopLevel& operator=(const RPG_Graphics_TopLevel&));

  // helper methods
  const bool loadGraphics(const RPG_Graphics_GraphicTypeUnion&); // (element) type
};

#endif
