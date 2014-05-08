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

#ifndef RPG_GRAPHICS_IWINDOW_H
#define RPG_GRAPHICS_IWINDOW_H

#include "rpg_graphics_common.h"
#include "rpg_graphics_iwindow_base.h"
#include "rpg_graphics_colorname.h"
#include "rpg_graphics_cursor.h"

#include <SDL.h>

class RPG_Graphics_IWindow
 : public virtual RPG_Graphics_IWindowBase
{
 public:
  // exposed interface
  virtual RPG_Graphics_Size_t getSize(const bool& = false) const = 0; // top-level ?
  //virtual void setView(const RPG_Graphics_Position_t&) = 0; // view (map coordinates !)
  //virtual RPG_Graphics_Position_t getView() const = 0; // return value: view (map coordinates !)

  virtual void clear(const RPG_Graphics_ColorName& = COLOR_BLACK) = 0;
  virtual RPG_Graphics_IWindowBase* child(const RPG_Graphics_WindowType&) = 0; // type

  virtual void notify(const RPG_Graphics_Cursor&) const = 0;
};

#endif
