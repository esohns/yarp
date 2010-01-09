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

class RPG_Item_CriticalHitProperties_Type
 : public RPG_Item_CriticalHitProperties_Type_pimpl
{
 public:
  RPG_Item_CriticalHitProperties_Type();

//   virtual void pre();
  virtual void minToHitRoll(unsigned char);
  virtual void damageModifier(unsigned char);
  virtual RPG_Item_CriticalHitProperties post_RPG_Item_CriticalHitProperties_Type();

 private:
  RPG_Item_CriticalHitProperties myCurrentCriticalHitProperties;
};

class RPG_Item_CreationCost_Type
 : public RPG_Item_CreationCost_Type_pimpl
{
 public:
  RPG_Item_CreationCost_Type();

//   virtual void pre();
  virtual void numGoldPieces(unsigned int);
  virtual void numExperiencePoints(unsigned int);
  virtual RPG_Item_CreationCost post_RPG_Item_CreationCost_Type();

 private:
  RPG_Item_CreationCost myCurrentCreationCost;
};

class RPG_Item_BaseProperties_Type
 : public RPG_Item_BaseProperties_Type_pimpl
{
 public:
  RPG_Item_BaseProperties_Type();

//   virtual void pre();
  virtual void baseWeight(unsigned short);
  virtual void baseStorePrice(const RPG_Item_StorePrice&);
  virtual void costToCreate(const RPG_Item_CreationCost&);
  virtual RPG_Item_BaseProperties post_RPG_Item_BaseProperties_Type();

 private:
  RPG_Item_BaseProperties myBaseProperties;
};

class RPG_Item_MagicalPrerequisites_Type
 : public RPG_Item_MagicalPrerequisites_Type_pimpl
{
 public:
  RPG_Item_MagicalPrerequisites_Type();

//   virtual void pre();
  virtual void minCasterLevel(unsigned char);
  virtual RPG_Item_MagicalPrerequisites post_RPG_Item_MagicalPrerequisites_Type();

 private:
  RPG_Item_MagicalPrerequisites myCurrentPrerequisites;
};

class RPG_Item_MagicalProperties_Type
 : public RPG_Item_MagicalProperties_Type_pimpl
{
 public:
  RPG_Item_MagicalProperties_Type();

//   virtual void pre();
  virtual void Aura(const RPG_Magic_School&);
  virtual void Prerequisites(const RPG_Item_MagicalPrerequisites&);
  virtual RPG_Item_MagicalProperties post_RPG_Item_MagicalProperties_Type();

 private:
  RPG_Item_MagicalProperties myCurrentProperties;
};

class RPG_Item_WeaponPropertiesXML_Type
 : public RPG_Item_WeaponPropertiesXML_Type_pimpl
{
 public:
  RPG_Item_WeaponPropertiesXML_Type();

//   virtual void pre();
  virtual void weaponType(const RPG_Item_WeaponType&);
  virtual void weaponCategory(const RPG_Item_WeaponCategory&);
  virtual void weaponClass(const RPG_Item_WeaponClass&);
  virtual void baseStorePrice(const RPG_Item_StorePrice&);
  virtual void baseDamage(const RPG_Item_Damage&);
  virtual void criticalHit(const RPG_Item_CriticalHitProperties&);
  virtual void rangeIncrement(unsigned char);
  virtual void baseWeight(unsigned short);
  virtual void typeOfDamage(const RPG_Common_PhysicalDamageType&);
  virtual void isNonLethal(bool);
  virtual void isReachWeapon(bool);
  virtual void isDoubleWeapon(bool);
  virtual RPG_Item_WeaponPropertiesXML post_RPG_Item_WeaponPropertiesXML_Type();

 private:
  RPG_Item_WeaponPropertiesXML myCurrentWeaponProperties;
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

class RPG_Item_ArmorPropertiesXML_Type
 : public RPG_Item_ArmorPropertiesXML_Type_pimpl
{
 public:
  RPG_Item_ArmorPropertiesXML_Type();

//   virtual void pre();
  virtual void armorType(const RPG_Item_ArmorType&);
  virtual void armorCategory(const RPG_Item_ArmorCategory&);
  virtual void baseStorePrice(const RPG_Item_StorePrice&);
  virtual void baseArmorBonus(unsigned char);
  virtual void maxDexterityBonus(unsigned char);
  virtual void armorCheckPenalty(signed char);
  virtual void arcaneSpellFailure(unsigned char);
  virtual void baseSpeed(unsigned short);
  virtual void baseWeight(unsigned short);
  virtual RPG_Item_ArmorPropertiesXML post_RPG_Item_ArmorPropertiesXML_Type();

 private:
  RPG_Item_ArmorPropertiesXML myCurrentArmorProperties;
};

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
    virtual void weapon(const RPG_Item_WeaponPropertiesXML&);
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
    virtual void armor(const RPG_Item_ArmorPropertiesXML&);
    virtual void post_RPG_Item_ArmorDictionary_Type();

  private:
  // safety measures
    ACE_UNIMPLEMENTED_FUNC(RPG_Item_ArmorDictionary_Type());
    ACE_UNIMPLEMENTED_FUNC(RPG_Item_ArmorDictionary_Type(const RPG_Item_ArmorDictionary_Type&));
    ACE_UNIMPLEMENTED_FUNC(RPG_Item_ArmorDictionary_Type& operator=(const RPG_Item_ArmorDictionary_Type&));

    RPG_Item_ArmorDictionary_t* myArmorDictionary;
};

#endif
