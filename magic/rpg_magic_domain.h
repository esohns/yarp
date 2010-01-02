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

#ifndef RPG_MAGIC_DOMAIN_H
#define RPG_MAGIC_DOMAIN_H

enum RPG_Magic_Domain
{
  DOMAIN_GOOD = 0,
  DOMAIN_EVIL,
  DOMAIN_WAR,
  DOMAIN_PEACE,
  //
  RPG_MAGIC_DOMAIN_MAX,
  RPG_MAGIC_DOMAIN_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Magic_Domain, std::string> RPG_Magic_DomainToStringTable_t;
typedef RPG_Magic_DomainToStringTable_t::const_iterator RPG_Magic_DomainToStringTableIterator_t;

class RPG_Magic_DomainHelper
{
 public:
  inline static void init()
  {
    myRPG_Magic_DomainToStringTable.clear();
    myRPG_Magic_DomainToStringTable.insert(std::make_pair(DOMAIN_GOOD, ACE_TEXT_ALWAYS_CHAR("DOMAIN_GOOD")));
    myRPG_Magic_DomainToStringTable.insert(std::make_pair(DOMAIN_EVIL, ACE_TEXT_ALWAYS_CHAR("DOMAIN_EVIL")));
    myRPG_Magic_DomainToStringTable.insert(std::make_pair(DOMAIN_WAR, ACE_TEXT_ALWAYS_CHAR("DOMAIN_WAR")));
    myRPG_Magic_DomainToStringTable.insert(std::make_pair(DOMAIN_PEACE, ACE_TEXT_ALWAYS_CHAR("DOMAIN_PEACE")));
  };

  inline static std::string RPG_Magic_DomainToString(const RPG_Magic_Domain& element_in)
  {
    std::string result;
    RPG_Magic_DomainToStringTableIterator_t iterator = myRPG_Magic_DomainToStringTable.find(element_in);
    if (iterator != myRPG_Magic_DomainToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_MAGIC_DOMAIN_INVALID");

    return result;
  };

  inline static RPG_Magic_Domain stringToRPG_Magic_Domain(const std::string& string_in)
  {
    RPG_Magic_DomainToStringTableIterator_t iterator = myRPG_Magic_DomainToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Magic_DomainToStringTable.end());

    return RPG_MAGIC_DOMAIN_INVALID;
  };

  static RPG_Magic_DomainToStringTable_t myRPG_Magic_DomainToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_DomainHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_DomainHelper(const RPG_Magic_DomainHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_DomainHelper& operator=(const RPG_Magic_DomainHelper&));
};

#endif
