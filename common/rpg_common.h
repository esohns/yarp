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

#ifndef RPG_COMMON_H
#define RPG_COMMON_H

#include "rpg_common_physicaldamagetype.h"
#include "rpg_common_terrain.h"

#include <set>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
typedef std::set<RPG_Common_PhysicalDamageType> RPG_Common_PhysicalDamageList_t;
typedef RPG_Common_PhysicalDamageList_t::const_iterator RPG_Common_PhysicalDamageListIterator_t;

typedef std::set<RPG_Common_Terrain> RPG_Common_Terrains_t;
typedef RPG_Common_Terrains_t::const_iterator RPG_Common_TerrainsIterator_t;

#endif
