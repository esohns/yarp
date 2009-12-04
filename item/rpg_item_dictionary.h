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

#ifndef RPG_ITEM_DICTIONARY_H
#define RPG_ITEM_DICTIONARY_H

struct RPG_Item_Dictionary
{
<<<<<<< .mine
  // we use the singleton pattern, so we need to enable access to the ctor/dtors
  friend class ACE_Singleton<RPG_Item_Dictionary,
                             ACE_Thread_Mutex>;

 public:
  // init item dictionary
  void initItemDictionary(const std::string&); // filename
  const RPG_Item_WeaponProperties getWeaponProperties(const RPG_Item_WeaponType&) const;
  const RPG_Item_ArmorProperties getArmorProperties(const RPG_Item_ArmorType&) const;

  // debug info
  void dump() const;

 private:
  // safety measures
  RPG_Item_Dictionary();
  virtual ~RPG_Item_Dictionary();
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Dictionary(const RPG_Item_Dictionary&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Dictionary& operator=(const RPG_Item_Dictionary&));

  RPG_Item_WeaponDictionary_t myWeaponDictionary;
  RPG_Item_ArmorDictionary_t  myArmorDictionary;
=======
  RPG_Item_WeaponDictionary weaponDictionary;
  RPG_Item_ArmorDictionary armorDictionary;
>>>>>>> .r47
};

#endif
