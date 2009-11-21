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
#ifndef RPG_ITEM_XML_PARSER_H
#define RPG_ITEM_XML_PARSER_H

#include "rpg_item_common.h"
#include "rpg_item_XML_parser_base.h"

#include <ace/Global_Macros.h>

/**
  @author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Item_Dictionary_Type
 : public RPG_Item_Dictionary_Type_pimpl
{
 public:
  RPG_Item_Dictionary_Type(RPG_Item_WeaponDictionary_t*, // weapon dictionary
                           RPG_Item_ArmorDictionary_t*); // armor dictionary
  virtual ~RPG_Item_Dictionary_Type();

//   virtual void pre();
//   virtual void RPG_Item_WeaponDictionary();
//   virtual void RPG_Item_ArmorDictionary();
  virtual void post_RPG_Item_Dictionary_Type();

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Dictionary_Type());
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Dictionary_Type(const RPG_Item_Dictionary_Type&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Dictionary_Type& operator=(const RPG_Item_Dictionary_Type&));

  RPG_Item_WeaponDictionary_t* myWeaponDictionary;
  RPG_Item_ArmorDictionary_t*  myArmorDictionary;
};

class RPG_Item_WeaponDictionary_Type
 : public RPG_Item_WeaponDictionary_Type_pimpl
{
 public:
  RPG_Item_WeaponDictionary_Type(RPG_Item_WeaponDictionary_t*); // weapon dictionary

//   virtual void pre();
  virtual void weapon(const RPG_Item_WeaponProperties_XML&);
  virtual void post_RPG_Item_WeaponDictionary_Type();

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_WeaponDictionary_Type());
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_WeaponDictionary_Type(const RPG_Item_WeaponDictionary_Type&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_WeaponDictionary_Type& operator=(const RPG_Item_WeaponDictionary_Type&));

  RPG_Item_WeaponDictionary_t* myWeaponDictionary;
};

class RPG_Item_ArmorDictionary_Type
 : public RPG_Item_ArmorDictionary_Type_pimpl
{
 public:
  RPG_Item_ArmorDictionary_Type(RPG_Item_ArmorDictionary_t*); // armor dictionary

//   virtual void pre();
  virtual void armor(const RPG_Item_ArmorProperties_XML&);
  virtual void post_RPG_Item_ArmorDictionary_Type();

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_ArmorDictionary_Type());
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_ArmorDictionary_Type(const RPG_Item_ArmorDictionary_Type&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_ArmorDictionary_Type& operator=(const RPG_Item_ArmorDictionary_Type&));

  RPG_Item_ArmorDictionary_t* myArmorDictionary;
};

class RPG_Item_WeaponCategory_Type
 : public RPG_Item_WeaponCategory_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Item_WeaponCategory post_RPG_Item_WeaponCategory_Type();
};

class RPG_Item_WeaponClass_Type
 : public RPG_Item_WeaponClass_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Item_WeaponClass post_RPG_Item_WeaponClass_Type();
};

class RPG_Item_WeaponType_Type
 : public RPG_Item_WeaponType_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Item_WeaponType post_RPG_Item_WeaponType_Type();
};

class RPG_Item_StorePrice_Type
 : public RPG_Item_StorePrice_Type_pimpl
{
 public:
  RPG_Item_StorePrice_Type();

//   virtual void pre();
  virtual void numGoldPieces(unsigned int);
  virtual void numSilverPieces(unsigned int);
  virtual RPG_Item_StorePrice post_RPG_Item_StorePrice_Type();

 private:
  RPG_Item_StorePrice myCurrentStorePrice;
};

class RPG_Item_CriticalHitModifier_Type
 : public RPG_Item_CriticalHitModifier_Type_pimpl
{
 public:
  RPG_Item_CriticalHitModifier_Type();

//   virtual void pre();
  virtual void minToHitRoll(unsigned int);
  virtual void damageModifier(unsigned int);
  virtual RPG_Item_CriticalHitModifier post_RPG_Item_CriticalHitModifier_Type();

 private:
  RPG_Item_CriticalHitModifier myCurrentCriticalHitModifier;
};

class RPG_Item_WeaponDamageType_Type
 : public RPG_Item_WeaponDamageType_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Item_WeaponDamageType post_RPG_Item_WeaponDamageType_Type();
};

class RPG_Item_WeaponProperties_Type
 : public RPG_Item_WeaponProperties_Type_pimpl
{
 public:
  RPG_Item_WeaponProperties_Type();

//   virtual void pre();
  virtual void weaponType(const RPG_Item_WeaponType&);
  virtual void weaponCategory(const RPG_Item_WeaponCategory&);
  virtual void weaponClass(const RPG_Item_WeaponClass&);
  virtual void baseStorePrice(const RPG_Item_StorePrice&);
  virtual void baseDamage(const RPG_Item_Damage&);
  virtual void criticalHitModifier(const RPG_Item_CriticalHitModifier&);
  virtual void rangeIncrement(unsigned int);
  virtual void baseWeight(unsigned int);
  virtual void typeOfDamage(const RPG_Item_WeaponDamageType&);
  virtual RPG_Item_WeaponProperties_XML post_RPG_Item_WeaponProperties_Type();

 private:
  RPG_Item_WeaponProperties_XML myCurrentWeaponProperty;
};

class RPG_Item_ArmorCategory_Type
 : public RPG_Item_ArmorCategory_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Item_ArmorCategory post_RPG_Item_ArmorCategory_Type();
};

class RPG_Item_ArmorType_Type
 : public RPG_Item_ArmorType_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Item_ArmorType post_RPG_Item_ArmorType_Type();
};

class RPG_Item_ArmorProperties_Type
 : public RPG_Item_ArmorProperties_Type_pimpl
{
 public:
  RPG_Item_ArmorProperties_Type();

//   virtual void pre();
  virtual void armorType(const RPG_Item_ArmorType&);
  virtual void armorCategory(const RPG_Item_ArmorCategory&);
  virtual void baseStorePrice(const RPG_Item_StorePrice&);
  virtual void baseArmorBonus(unsigned int);
  virtual void maxDexterityBonus(unsigned int);
  virtual void armorCheckPenalty(long long);
  virtual void arcaneSpellFailure(unsigned int);
  virtual void baseSpeed(unsigned int);
  virtual void baseWeight(unsigned int);
  virtual RPG_Item_ArmorProperties_XML post_RPG_Item_ArmorProperties_Type();

 private:
  RPG_Item_ArmorProperties_XML myCurrentArmorProperty;
};

#endif
