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

#ifndef RPG_MONSTER_SPECIALATTACKPRECONDITION_H
#define RPG_MONSTER_SPECIALATTACKPRECONDITION_H

struct RPG_Monster_SpecialAttackPreCondition
{
  RPG_Monster_ActionTrigger action;
  RPG_Character_Alignment targetAlignment;
  std::vector<RPG_Character_Condition> ownConditions;
  std::vector<RPG_Character_Condition> targetConditions;
  RPG_Character_Size minTargetSize;
  RPG_Character_Size maxTargetSize;
  RPG_Combat_Check check;
};

#endif
