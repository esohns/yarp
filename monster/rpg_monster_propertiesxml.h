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

#ifndef RPG_MONSTER_PROPERTIESXML_H
#define RPG_MONSTER_PROPERTIESXML_H

struct RPG_Monster_PropertiesXML
{
  std::string name;
  RPG_Character_Size size;
  RPG_Monster_Type type;
  RPG_Dice_Roll hitDice;
  char initiative;
  unsigned char speed;
  RPG_Monster_NaturalArmorClass armorClass;
  RPG_Monster_Attack attack;
  std::vector<RPG_Monster_SpecialAttackProperties> specialAttacks;
  unsigned char space;
  unsigned char reach;
  RPG_Monster_SavingThrowModifiers saves;
  RPG_Character_Attributes attributes;
  RPG_Character_Skills skills;
  RPG_Character_Feats feats;
  RPG_Character_Environment environment;
  RPG_Monster_Organizations organizations;
  unsigned char challengeRating;
  unsigned char treasureModifier;
  RPG_Character_Alignment alignment;
  RPG_Monster_Advancement advancements;
  unsigned char levelAdjustment;
};

#endif
