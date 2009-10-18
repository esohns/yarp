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
#ifndef RPG_CHARACTER_COMMON_TOOLS_H
#define RPG_CHARACTER_COMMON_TOOLS_H

#include "rpg_character_common.h"

#include <ace/Global_Macros.h>

#include <string>
#include <map>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Character_Common_Tools
{
 public:
  // some handy types
  typedef std::map<std::string, RPG_Character_Gender> RPG_String2Gender_t;
  typedef RPG_String2Gender_t::const_iterator RPG_String2GenderIterator_t;
  typedef std::map<std::string, RPG_Character_AlignmentCivic> RPG_String2AlignmentCivic_t;
  typedef RPG_String2AlignmentCivic_t::const_iterator RPG_String2AlignmentCivicIterator_t;
  typedef std::map<std::string, RPG_Character_AlignmentEthic> RPG_String2AlignmentEthic_t;
  typedef RPG_String2AlignmentEthic_t::const_iterator RPG_String2AlignmentEthicIterator_t;
  typedef std::map<std::string, RPG_Character_Condition> RPG_String2Condition_t;
  typedef RPG_String2Condition_t::const_iterator RPG_String2ConditionIterator_t;

  static void initStringConversionTables();
  static RPG_Character_Gender stringToGender(const std::string&); // string
  static RPG_Character_AlignmentCivic stringToAlignmentCivic(const std::string&); // string
  static RPG_Character_AlignmentEthic stringToAlignmentEthic(const std::string&); // string
  static RPG_Character_Condition stringToCondition(const std::string&); // string

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Character_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Common_Tools(const RPG_Character_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Common_Tools& operator=(const RPG_Character_Common_Tools&));

  static RPG_String2Gender_t         myString2GenderTable;
  static RPG_String2AlignmentCivic_t myString2AlignmentCivicTable;
  static RPG_String2AlignmentEthic_t myString2AlignmentEthicTable;
  static RPG_String2Condition_t      myString2ConditionTable;
};

#endif
