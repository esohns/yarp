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
#ifndef RPG_CHARACTER_CLASS_COMMON_H
#define RPG_CHARACTER_CLASS_COMMON_H

enum RPG_Character_MetaClass
{
  METACLASS_WARRIOR = 1,
  METACLASS_WIZARD = 2,
  METACLASS_PRIEST = 4,
  METACLASS_ROGUE = 8
};

enum RPG_Character_SubClass
{
  // warrior subclasses
  SUBCLASS_FIGHTER = 1,
  SUBCLASS_PALADIN = 2,
  SUBCLASS_RANGER = 4,
  SUBCLASS_BARBARIAN = 8,
//  SUBCLASS_WARLORD = 16,
  // wizard subclasses
  SUBCLASS_WIZARD = 32,
  SUBCLASS_SORCERER = 64,
//  SUBCLASS_WARLOCK = 128,
  // priest subclasses
  SUBCLASS_CLERIC = 256,
  SUBCLASS_DRUID = 512,
  SUBCLASS_MONK = 1024,
//  SUBCLASS_AVENGER = 2048,
//  SUBCLASS_INVOKER = 4096,
//  SUBCLASS_SHAMAN = 8192,
  // rogue subclasses
  SUBCLASS_THIEF = 16384,
  SUBCLASS_BARD = 32768
};

struct RPG_Character_Class
{
  RPG_Character_MetaClass metaClass;
  RPG_Character_SubClass subClass;
};

#endif
