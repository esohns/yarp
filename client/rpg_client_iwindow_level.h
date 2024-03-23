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

#ifndef RPG_CLIENT_IWINDOW_LEVEL_H
#define RPG_CLIENT_IWINDOW_LEVEL_H

#include <string>

#include "rpg_map_common.h"

#include "rpg_graphics_common.h"
#include "rpg_graphics_doorstyle.h"
#include "rpg_graphics_edgestyle.h"
#include "rpg_graphics_floorstyle.h"
#include "rpg_graphics_style.h"
#include "rpg_graphics_wallstyle.h"

#include "rpg_client_iwindow.h"

class RPG_Client_IWindowLevel
 : public RPG_Client_IWindow
{
 public:
  virtual ~RPG_Client_IWindowLevel () {}

  virtual void initialize (const struct RPG_Graphics_Style&) = 0; // style
  virtual void setView (int,              // offset x (map coordinates !)
                        int,              // offset y (map coordinates !)
                        bool = true) = 0; // locked access ?
  virtual void setView (const RPG_Map_Position_t&) = 0; // view (map coordinates !)
  virtual RPG_Graphics_Position_t getView () const = 0; // return value: view (map coordinates !)

  virtual void toggleDoor (const RPG_Map_Position_t&) = 0; // door position
  virtual void setBlendRadius (ACE_UINT8) = 0; // radius
  virtual void updateMinimap () = 0;
  virtual void updateMessageWindow (const std::string&) = 0; // message
};

#endif
