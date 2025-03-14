
// -------------------------------- * * * ----------------------------------- //
// PLEASE NOTE: this file was/is generated by XML2CppCode 0.0.1-devel
// -------------------------------- * * * ----------------------------------- //

#ifndef RPG_CHARACTER_GENDER_H
#define RPG_CHARACTER_GENDER_H

enum RPG_Character_Gender
{
  GENDER_NONE = 0,
  GENDER_FEMALE,
  GENDER_MALE,
  GENDER_ANY,
  //
  RPG_CHARACTER_GENDER_MAX,
  RPG_CHARACTER_GENDER_INVALID
};

#include "ace/Global_Macros.h"

#include <map>
#include <string>

typedef std::map<RPG_Character_Gender, std::string> RPG_Character_GenderToStringTable_t;
typedef RPG_Character_GenderToStringTable_t::const_iterator RPG_Character_GenderToStringTableIterator_t;

class RPG_Character_GenderHelper
{
 public:
  inline static void init()
  {
    myRPG_Character_GenderToStringTable.clear();
    myRPG_Character_GenderToStringTable.insert(std::make_pair(GENDER_NONE, ACE_TEXT_ALWAYS_CHAR("GENDER_NONE")));
    myRPG_Character_GenderToStringTable.insert(std::make_pair(GENDER_FEMALE, ACE_TEXT_ALWAYS_CHAR("GENDER_FEMALE")));
    myRPG_Character_GenderToStringTable.insert(std::make_pair(GENDER_MALE, ACE_TEXT_ALWAYS_CHAR("GENDER_MALE")));
    myRPG_Character_GenderToStringTable.insert(std::make_pair(GENDER_ANY, ACE_TEXT_ALWAYS_CHAR("GENDER_ANY")));
  };

  inline static std::string RPG_Character_GenderToString(const RPG_Character_Gender& element_in)
  {
    std::string result;
    RPG_Character_GenderToStringTableIterator_t iterator = myRPG_Character_GenderToStringTable.find(element_in);
    if (iterator != myRPG_Character_GenderToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_CHARACTER_GENDER_INVALID");

    return result;
  };

  inline static RPG_Character_Gender stringToRPG_Character_Gender(const std::string& string_in)
  {
    RPG_Character_GenderToStringTableIterator_t iterator = myRPG_Character_GenderToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Character_GenderToStringTable.end());

    return RPG_CHARACTER_GENDER_INVALID;
  };

  static RPG_Character_GenderToStringTable_t myRPG_Character_GenderToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_GenderHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_GenderHelper(const RPG_Character_GenderHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_GenderHelper& operator=(const RPG_Character_GenderHelper&));
};

#endif
