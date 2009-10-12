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
#include "rpg_item_dictionary.h"

// #include "rpg_item_schema_dictionary_types.h"
// #include "rpg_item_dictionary_parser_base.h"
#include "rpg_item_dictionary_parser.h"

#include <ace/Log_Msg.h>

#include <iostream>

RPG_Item_Dictionary::RPG_Item_Dictionary()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary::RPG_Item_Dictionary"));

}

RPG_Item_Dictionary::~RPG_Item_Dictionary()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary::~RPG_Item_Dictionary"));

}

void RPG_Item_Dictionary::initItemDictionary(const std::string& filename_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary::initItemDictionary"));

  // Construct the parser.
  //
  ::xml_schema::unsigned_int_pimpl         unsigned_int_p;
  RPG_Chance_Dice_Type_pimpl               chanceDiceType_p;
  ::xml_schema::integer_pimpl              int_p;

  RPG_Item_Weapon_Type_pimpl               weaponType_p;
  RPG_Item_Weapon_Category_Type_pimpl      weaponCategory_p;
  RPG_Item_Weapon_Class_Type_pimpl         weaponClass_p;
  RPG_Item_Store_Price_Type_pimpl          baseStorePrice_p;
  baseStorePrice_p.parsers(unsigned_int_p,
                           unsigned_int_p);
  RPG_Chance_Roll_Type_pimpl               baseDamage_p;
  baseDamage_p.parsers(unsigned_int_p,
                       chanceDiceType_p,
                       int_p);
  RPG_Item_CriticalHit_Modifier_Type_pimpl criticalModifier_p;
  criticalModifier_p.parsers(unsigned_int_p,
                             unsigned_int_p);
//   unsigned_int_pimpl                       rangeIncrement_p;
//   unsigned_int_pimpl                       baseWeight_p;
  RPG_Item_Weapon_Damage_Type_pimpl        damageType_p;

  RPG_Item_Armor_Type_pimpl                armorType_p;
  RPG_Item_Armor_Category_Type_pimpl       armorCategory_p;
//  RPG_Item_Store_Price_Type_pimpl          baseStorePrice_p;
//  unsigned_int_pimpl                       baseArmorBonus_p;
//  unsigned_int_pimpl                       maxDexterityBonus_p;
//  int_pimpl                                armorCheckPenalty_p;
//  unsigned_int_pimpl                       arcaneSpellFailure_p;
//  unsigned_int_pimpl                       baseSpeed_p;
//  unsigned_int_pimpl                       baseWeight_p;

  RPG_Item_Weapon_Properties_Type_pimpl weaponProperties_p;
  weaponProperties_p.parsers(weaponType_p,
                             weaponCategory_p,
                             weaponClass_p,
                             baseStorePrice_p,
                             baseDamage_p,
                             criticalModifier_p,
                             unsigned_int_p,
                             unsigned_int_p,
                             damageType_p);
  RPG_Item_Armor_Properties_Type_pimpl  armorProperties_p;
  armorProperties_p.parsers(armorType_p,
                            armorCategory_p,
                            baseStorePrice_p,
                            unsigned_int_p,
                            unsigned_int_p,
                            int_p,
                            unsigned_int_p,
                            unsigned_int_p,
                            unsigned_int_p);

  RPG_Item_Weapon_Dictionary_Type_pimpl weaponDictionary_p;
  weaponDictionary_p.parsers(weaponProperties_p);
  RPG_Item_Armor_Dictionary_Type_pimpl  armorDictionary_p;
  armorDictionary_p.parsers(armorProperties_p);

  RPG_Item_Dictionary_Parser itemDictionary_p(&myWeaponDictionary,
                                              &myArmorDictionary);
  itemDictionary_p.parsers(weaponDictionary_p,
                           armorDictionary_p);

  // Parse the document to obtain the object model.
  //
  ::xml_schema::document doc_p(itemDictionary_p,
                               ACE_TEXT_ALWAYS_CHAR("RPG_Item_Dictionary"));

  itemDictionary_p.pre();

  // OK: parse the file...
  try
  {
    doc_p.parse(filename_in);
  }
  catch(const ::xml_schema::parsing& exception)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Item_Dictionary::initItemDictionary(): exception occurred, returning\n")));

    std::cerr << exception << std::endl;
//     exception.print(std::cerr);

    return;
  }
  catch(...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Item_Dictionary::initItemDictionary(): exception occurred, returning\n")));

    return;
  }

  itemDictionary_p.post_RPG_Item_Dictionary_Type();

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("parsed dictionary file \"%s\", retrieved %d weapons and %d armors...\n"),
             filename_in.c_str(),
             myWeaponDictionary.size(),
             myArmorDictionary.size()));
}
