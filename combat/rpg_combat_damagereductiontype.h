
// -------------------------------- * * * ----------------------------------- //
// PLEASE NOTE: this file was/is generated by XML2CppCode 0.0.1-devel
// -------------------------------- * * * ----------------------------------- //

#ifndef RPG_COMBAT_DAMAGEREDUCTIONTYPE_H
#define RPG_COMBAT_DAMAGEREDUCTIONTYPE_H

enum RPG_Combat_DamageReductionType
{
  REDUCTION_FULL = 0,
  REDUCTION_HALF,
  REDUCTION_PARTIAL,
  REDUCTION_TEMPORARY,
  //
  RPG_COMBAT_DAMAGEREDUCTIONTYPE_MAX,
  RPG_COMBAT_DAMAGEREDUCTIONTYPE_INVALID
};

#include "ace/Global_Macros.h"

#include <map>
#include <string>

typedef std::map<RPG_Combat_DamageReductionType, std::string> RPG_Combat_DamageReductionTypeToStringTable_t;
typedef RPG_Combat_DamageReductionTypeToStringTable_t::const_iterator RPG_Combat_DamageReductionTypeToStringTableIterator_t;

class RPG_Combat_DamageReductionTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Combat_DamageReductionTypeToStringTable.clear();
    myRPG_Combat_DamageReductionTypeToStringTable.insert(std::make_pair(REDUCTION_FULL, ACE_TEXT_ALWAYS_CHAR("REDUCTION_FULL")));
    myRPG_Combat_DamageReductionTypeToStringTable.insert(std::make_pair(REDUCTION_HALF, ACE_TEXT_ALWAYS_CHAR("REDUCTION_HALF")));
    myRPG_Combat_DamageReductionTypeToStringTable.insert(std::make_pair(REDUCTION_PARTIAL, ACE_TEXT_ALWAYS_CHAR("REDUCTION_PARTIAL")));
    myRPG_Combat_DamageReductionTypeToStringTable.insert(std::make_pair(REDUCTION_TEMPORARY, ACE_TEXT_ALWAYS_CHAR("REDUCTION_TEMPORARY")));
  };

  inline static std::string RPG_Combat_DamageReductionTypeToString(const RPG_Combat_DamageReductionType& element_in)
  {
    std::string result;
    RPG_Combat_DamageReductionTypeToStringTableIterator_t iterator = myRPG_Combat_DamageReductionTypeToStringTable.find(element_in);
    if (iterator != myRPG_Combat_DamageReductionTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMBAT_DAMAGEREDUCTIONTYPE_INVALID");

    return result;
  };

  inline static RPG_Combat_DamageReductionType stringToRPG_Combat_DamageReductionType(const std::string& string_in)
  {
    RPG_Combat_DamageReductionTypeToStringTableIterator_t iterator = myRPG_Combat_DamageReductionTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Combat_DamageReductionTypeToStringTable.end());

    return RPG_COMBAT_DAMAGEREDUCTIONTYPE_INVALID;
  };

  static RPG_Combat_DamageReductionTypeToStringTable_t myRPG_Combat_DamageReductionTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_DamageReductionTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_DamageReductionTypeHelper(const RPG_Combat_DamageReductionTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Combat_DamageReductionTypeHelper& operator=(const RPG_Combat_DamageReductionTypeHelper&));
};

#endif
