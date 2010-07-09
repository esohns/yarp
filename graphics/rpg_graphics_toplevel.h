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

#include "rpg_graphics_SDLwindow.h"
#include "rpg_graphics_common.h"
#include "rpg_graphics_type.h"

#include <SDL/SDL.h>

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Graphics_TopLevel
 : public RPG_Graphics_SDLWindow
{
 public:
  RPG_Graphics_TopLevel(const RPG_Graphics_WindowSize_t&,                 // size
                        const RPG_Graphics_Type&,                         // style
                        const std::string&,                               // title
                        const RPG_Graphics_Type& = TYPE_FONT_MAIN_LARGE); // title font
  virtual ~RPG_Graphics_TopLevel();

 protected:
  // window element graphic(s)
  RPG_Graphics_Type                myElementGraphicsType;
  RPG_Graphics_InterfaceElements_t myElementGraphics;

 private:
  typedef RPG_Graphics_SDLWindow inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_TopLevel());
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_TopLevel(const RPG_Graphics_TopLevel&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_TopLevel& operator=(const RPG_Graphics_TopLevel&));

  // helper methods
  const bool loadGraphics(const RPG_Graphics_Type&); // (hover) cursor graphic

  bool                             myInitialized;
};

#endif
