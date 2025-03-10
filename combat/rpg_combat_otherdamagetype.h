
// -------------------------------- * * * ----------------------------------- //
// PLEASE NOTE: this file was/is generated by XML2CppCode 0.0.1-devel
// -------------------------------- * * * ----------------------------------- //

#ifndef RPG_COMBAT_OTHERDAMAGETYPE_H
#define RPG_COMBAT_OTHERDAMAGETYPE_H

enum RPG_Combat_OtherDamageType
{
  OTHERDAMAGE_DISSOLVEARMOR = 0,
  OTHERDAMAGE_EXTRAHITPOINTS,
  OTHERDAMAGE_NATURALARMOR,
  OTHERDAMAGE_DISSOLVEWEAPON,
  //
  RPG_COMBAT_OTHERDAMAGETYPE_MAX,
  RPG_COMBAT_OTHERDAMAGETYPE_INVALID
};

#include "ace/Global_Macros.h"

#include <map>
#include <string>

typedef std::map<RPG_Combat_OtherDamageType, std::string> RPG_Combat_OtherDamageTypeToStringTable_t;
typedef RPG_Combat_OtherDamageTypeToStringTable_t::const_iterator RPG_Combat_OtherDamageTypeToStringTableIterator_t;

class RPG_Combat_OtherDamageTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Combat_OtherDamageTypeToStringTable.clear();
    myRPG_Combat_OtherDamageTypeToStringTable.insert(std::make_pair(OTHERDAMAGE_DISSOLVEARMOR, ACE_TEXT_ALWAYS_CHAR("OTHERDAMAGE_DISSOLVEARMOR")));
    myRPG_Combat_OtherDamageTypeToStringTable.insert(std::make_pair(OTHERDAMAGE_EXTRAHITPOINTS, ACE_TEXT_ALWAYS_CHAR("OTHERDAMAGE_EXTRAHITPOINTS")));
    myRPG_Combat_OtherDamageTypeToStringTable.insert(std::make_pair(OTHERDAMAGE_NATURALARMOR, ACE_TEXT_ALWAYS_CHAR("OTHERDAMAGE_NATURALARMOR")));
    myRPG_Combat_OtherDamageTypeToStringTable.insert(std::make_pair(OTHERDAMAGE_DISSOLVEWEAPON, ACE_TEXT_ALWAYS_CHAR("OTHERDAMAGE_DISSOLVEWEAPON")));
  };

  inline static std::string RPG_Combat_OtherDamageTypeToString(const RPG_Combat_OtherDamageType& element_in)
  {
    std::string result;
    RPG_Combat_OtherDamageTypeToStringTableIterator_t iterator = myRPG_Combat_OtherDamageTypeToStringTable.find(element_in);
    if (iterator != myRPG_Combat_OtherDamageTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMBAT_OTHERDAMAGETYPE_INVALID");

    return result;
  };

  inline static RPG_Combat_OtherDamageType stringToRPG_Combat_OtherDamageType(const std::string& string_in)
  {
    RPG_Combat_OtherDamageTypeToStringTableIterator_t iterator = myRPG_Combat_OtherDamageTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Combat_OtherDamageTypeToStringTable.end());

    return RPG_COMBAT_OTHERDAMAGETYPE_INVALID;
  };

  static RPG_Combat_OtherDamageTypeToStringTable_t myRPG_Combat_OtherDamageTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_OtherDamageTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_OtherDamageTypeHelper(const RPG_Combat_OtherDamageTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_OtherDamageTypeHelper& operator=(const RPG_Combat_OtherDamageTypeHelper&));
};

#endif
