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
#ifndef RPG_GRAPHICS_COMMON_H
#define RPG_GRAPHICS_COMMON_H

#include "rpg_graphics_incl.h"

#include <SDL/SDL.h>

#include <string>
#include <map>
#include <vector>

typedef RPG_Graphics_Graphic RPG_Graphics_t;
typedef std::map<RPG_Graphics_Type, RPG_Graphics_t> RPG_Graphics_Dictionary_t;
typedef RPG_Graphics_Dictionary_t::const_iterator RPG_Graphics_DictionaryIterator_t;

struct RPG_Graphics_GraphicsCacheNode_t
{
  RPG_Graphics_Type type;
  SDL_Surface* image;

  inline bool operator==(const RPG_Graphics_GraphicsCacheNode_t& rhs_in) const
  { return (this->type == rhs_in.type); }
};
typedef std::vector<RPG_Graphics_GraphicsCacheNode_t> RPG_Graphics_GraphicsCache_t;
typedef RPG_Graphics_GraphicsCache_t::iterator RPG_Graphics_GraphicsCacheIterator_t;

#endif