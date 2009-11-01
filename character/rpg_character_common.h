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
#ifndef RPG_CHARACTER_COMMON_H
#define RPG_CHARACTER_COMMON_H

enum RPG_Character_Gender
{
  GENDER_NONE = 0,
  //
  GENDER_MALE = 1,
  GENDER_FEMALE = 2,
  //
  GENDER_MAX,
  GENDER_INVALID
};

enum RPG_Character_AlignmentCivic
{
  ALIGNMENTCIVIC_LAWFUL = 0,
  ALIGNMENTCIVIC_NEUTRAL,
  ALIGNMENTCIVIC_CHAOTIC,
  //
  ALIGNMENTCIVIC_MAX,
  ALIGNMENTCIVIC_INVALID
};

enum RPG_Character_AlignmentEthic
{
  ALIGNMENTETHIC_GOOD = 0,
  ALIGNMENTETHIC_NEUTRAL,
  ALIGNMENTETHIC_EVIL,
  //
  ALIGNMENTETHIC_MAX,
  ALIGNMENTETHIC_INVALID
};

struct RPG_Character_Alignment
{
  RPG_Character_AlignmentCivic civicAlignment;
  RPG_Character_AlignmentEthic ethicAlignment;
};

enum RPG_Character_Attribute
{
  ATTRIBUTE_STRENGTH = 0,
  ATTRIBUTE_DEXTERITY,
  ATTRIBUTE_CONSTITUTION,
  ATTRIBUTE_INTELLIGENCE,
  ATTRIBUTE_WISDOM,
  ATTRIBUTE_CHARISMA,
  //
  ATTRIBUTE_MAX,
  ATTRIBUTE_INVALID
};

struct RPG_Character_Attributes
{
  unsigned char strength;
  unsigned char dexterity;
  unsigned char constitution;
  unsigned char intelligence;
  unsigned char wisdom;
  unsigned char charisma;
};

enum RPG_Character_Condition
{
  CONDITION_NORMAL = 0, // dummy default
  CONDITION_ABILITY_DAMAGED,
  CONDITION_ABILITY_DRAINED,
  CONDITION_BLINDED,
  CONDITION_BLOWN_AWAY,
  CONDITION_CHECKED,
  CONDITION_CONFUSED,
  CONDITION_COWERING,
  CONDITION_DAZED,
  CONDITION_DAZZLED,
  CONDITION_DEAD,
  CONDITION_DEAFENED,
  CONDITION_DISABLED,
  CONDITION_DYING,
  CONDITION_ENERGY_DRAINED,
  CONDITION_ENTANGLED,
  CONDITION_EXHAUSTED,
  CONDITION_FASCINATED,
  CONDITION_FATIGUED,
  CONDITION_FLAT_FOOTED,
  CONDITION_FRIGHTENED,
  CONDITION_GRAPPLING,
  CONDITION_HELPLESS,
  CONDITION_INCORPOREAL,
  CONDITION_INVISIBLE,
  CONDITION_KNOCKED_DOWN,
  CONDITION_NAUSEATED,
  CONDITION_PANICKED,
  CONDITION_PARALYZED,
  CONDITION_PETRIFIED,
  CONDITION_PINNED,
  CONDITION_PRONE,
  CONDITION_SHAKEN,
  CONDITION_SICKENED,
  CONDITION_STABLE,
  CONDITION_STAGGERED,
  CONDITION_STUNNED,
  CONDITION_TURNED,
  CONDITION_UNCONSCIOUS,
  //
  CONDITION_MAX,
  CONDITION_INVALID
};

#endif
