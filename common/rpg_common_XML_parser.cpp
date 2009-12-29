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
#include "rpg_common_XML_parser.h"

#include "rpg_common_incl.h"

#include <ace/Log_Msg.h>

RPG_Common_Attribute RPG_Common_Attribute_Type::post_RPG_Common_Attribute_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Common_Attribute_Type::post_RPG_Common_Attribute_Type"));

  return RPG_Common_AttributeHelper::stringToRPG_Common_Attribute(post_string());
}

RPG_Common_SavingThrow RPG_Common_SavingThrow_Type::post_RPG_Common_SavingThrow_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Common_SavingThrow_Type::post_RPG_Common_SavingThrow_Type"));

  return RPG_Common_SavingThrowHelper::stringToRPG_Common_SavingThrow(post_string());
}

RPG_Common_SavingThrowUnion_Type::RPG_Common_SavingThrowUnion_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Common_SavingThrowUnion_Type::RPG_Common_SavingThrowUnion_Type"));

  myCurrentSavingThrowUnion.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
//   myCurrentSavingThrowUnion.savingthrow = RPG_COMMON_SAVINGTHROW_INVALID;
}

void RPG_Common_SavingThrowUnion_Type::_characters(const ::xml_schema::ro_string& savingThrowUnion_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Common_SavingThrowUnion_Type::_characters"));

  // can be either:
  // - RPG_Common_Attribute_Type   --> "ATTRIBUTE_xxx"
  // - RPG_Common_SavingThrow_Type --> "SAVE_xxx"
  std::string type = savingThrowUnion_in;
  if (type.find(ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_")) == 0)
    myCurrentSavingThrowUnion.attribute = RPG_Common_AttributeHelper::stringToRPG_Common_Attribute(type);
  else
    myCurrentSavingThrowUnion.savingthrow = RPG_Common_SavingThrowHelper::stringToRPG_Common_SavingThrow(type);
}

RPG_Common_SavingThrowUnion RPG_Common_SavingThrowUnion_Type::post_RPG_Common_SavingThrowUnion_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Common_SavingThrowUnion_Type::post_RPG_Common_SavingThrowUnion_Type"));

  RPG_Common_SavingThrowUnion result = myCurrentSavingThrowUnion;

  // clear structure
  myCurrentSavingThrowUnion.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
//   myCurrentSavingThrowUnion.savingthrow = RPG_COMMON_SAVINGTHROW_INVALID;

  return result;
}
