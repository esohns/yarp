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

#include "rpg_item_XML_parser.h"
#include "rpg_item_common_tools.h"

#include "rpg_chance_dice_XML_parser.h"

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
  ::xml_schema::unsigned_int_pimpl        unsigned_int_p;
  RPG_Chance_DiceType_Type                chanceDiceType_p;
  ::xml_schema::integer_pimpl             int_p;

  RPG_Item_WeaponType_Type                weaponType_p;
  RPG_Item_WeaponCategory_Type            weaponCategory_p;
  RPG_Item_WeaponClass_Type               weaponClass_p;
  RPG_Item_StorePrice_Type                baseStorePrice_p;
  baseStorePrice_p.parsers(unsigned_int_p,
                           unsigned_int_p);
  RPG_Chance_DiceRoll_Type                baseDamage_p;
  baseDamage_p.parsers(unsigned_int_p,
                       chanceDiceType_p,
                       int_p);
  RPG_Item_CriticalHitModifier_Type       criticalModifier_p;
  criticalModifier_p.parsers(unsigned_int_p,
                             unsigned_int_p);
//   unsigned_int_pimpl                       rangeIncrement_p;
//   unsigned_int_pimpl                       baseWeight_p;
  RPG_Item_WeaponDamageType_Type          damageType_p;

  RPG_Item_ArmorType_Type                 armorType_p;
  RPG_Item_ArmorCategory_Type             armorCategory_p;
//  RPG_Item_StorePrice_Type_pimpl          baseStorePrice_p;
//  unsigned_int_pimpl                      baseArmorBonus_p;
//  unsigned_int_pimpl                      maxDexterityBonus_p;
//  int_pimpl                               armorCheckPenalty_p;
//  unsigned_int_pimpl                      arcaneSpellFailure_p;
//  unsigned_int_pimpl                      baseSpeed_p;
//  unsigned_int_pimpl                      baseWeight_p;

  RPG_Item_WeaponProperties_Type          weaponProperties_p;
  weaponProperties_p.parsers(weaponType_p,
                             weaponCategory_p,
                             weaponClass_p,
                             baseStorePrice_p,
                             baseDamage_p,
                             criticalModifier_p,
                             unsigned_int_p,
                             unsigned_int_p,
                             damageType_p);
  RPG_Item_ArmorProperties_Type           armorProperties_p;
  armorProperties_p.parsers(armorType_p,
                            armorCategory_p,
                            baseStorePrice_p,
                            unsigned_int_p,
                            unsigned_int_p,
                            int_p,
                            unsigned_int_p,
                            unsigned_int_p,
                            unsigned_int_p);

  RPG_Item_WeaponDictionary_Type          weaponDictionary_p(&myWeaponDictionary);
  weaponDictionary_p.parsers(weaponProperties_p);
  RPG_Item_ArmorDictionary_Type           armorDictionary_p(&myArmorDictionary);
  armorDictionary_p.parsers(armorProperties_p);

  RPG_Item_Dictionary_Type                itemDictionary_p(&myWeaponDictionary,
                                                           &myArmorDictionary);
  itemDictionary_p.parsers(weaponDictionary_p,
                           armorDictionary_p);

  // Parse the document to obtain the object model.
  //
  ::xml_schema::document doc_p(itemDictionary_p,
                               ACE_TEXT_ALWAYS_CHAR("urn:rpg"),
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

    return;
  }
  catch(...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Item_Dictionary::initItemDictionary(): exception occurred, returning\n")));

    return;
  }

  itemDictionary_p.post_RPG_Item_Dictionary_Type();

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("finished parsing item dictionary file \"%s\"...\n"),
//              filename_in.c_str()));
}

const RPG_Item_WeaponProperties RPG_Item_Dictionary::getWeaponProperties(const RPG_Item_WeaponType& weaponType_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary::getWeaponProperties"));

  RPG_Item_WeaponDictionaryIterator_t iterator = myWeaponDictionary.find(weaponType_in);
  if (iterator == myWeaponDictionary.end())
  {
    std::string weaponType_string = RPG_Item_Common_Tools::weaponTypeToString(weaponType_in);
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("invalid weapon type \"%s\" --> check implementation !, aborting\n"),
               weaponType_string.c_str()));

    ACE_ASSERT(false);
  } // end IF

  return iterator->second;
}

const RPG_Item_ArmorProperties RPG_Item_Dictionary::getArmorProperties(const RPG_Item_ArmorType& armorType_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary::getArmorProperties"));

  RPG_Item_ArmorDictionaryIterator_t iterator = myArmorDictionary.find(armorType_in);
  if (iterator == myArmorDictionary.end())
  {
    std::string armorType_string = RPG_Item_Common_Tools::armorTypeToString(armorType_in);
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("invalid armor type \"%s\" --> check implementation !, aborting\n"),
               armorType_string.c_str()));

    ACE_ASSERT(false);
  } // end IF

  return iterator->second;
}
