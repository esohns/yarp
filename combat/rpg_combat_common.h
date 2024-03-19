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

#ifndef RPG_COMBAT_COMMON_H
#define RPG_COMBAT_COMMON_H

#include <vector>

#include "rpg_dice_incl.h"

#include "rpg_common_incl.h"

#include "rpg_character_alignmentcivic.h"
#include "rpg_character_alignmentethic.h"
#include "rpg_character_alignment.h"

#include "rpg_magic_spelltype.h"

#include "rpg_combat_incl.h"

// some useful types
typedef std::vector<enum RPG_Combat_AttackForm> RPG_Combat_AttackForms_t;
typedef RPG_Combat_AttackForms_t::const_iterator RPG_Combat_AttackFormsIterator_t;

typedef std::vector<struct RPG_Combat_Damage> RPG_Combat_Damages_t;
typedef RPG_Combat_Damages_t::const_iterator RPG_Combat_DamagesConstIterator_t;

typedef std::vector<RPG_Combat_DamageElement>::const_iterator RPG_Combat_DamageElementsConstIterator_t;

#endif
