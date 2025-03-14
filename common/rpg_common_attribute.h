
// -------------------------------- * * * ----------------------------------- //
// PLEASE NOTE: this file was/is generated by XML2CppCode 0.0.1-devel
// -------------------------------- * * * ----------------------------------- //

#ifndef RPG_COMMON_ATTRIBUTE_H
#define RPG_COMMON_ATTRIBUTE_H

enum RPG_Common_Attribute
{
  ATTRIBUTE_CHARISMA = 0,
  ATTRIBUTE_CONSTITUTION,
  ATTRIBUTE_DEXTERITY,
  ATTRIBUTE_INTELLIGENCE,
  ATTRIBUTE_STRENGTH,
  ATTRIBUTE_WISDOM,
  ATTRIBUTE_ANY,
  //
  RPG_COMMON_ATTRIBUTE_MAX,
  RPG_COMMON_ATTRIBUTE_INVALID
};

#include "ace/Global_Macros.h"

#include <map>
#include <string>

typedef std::map<RPG_Common_Attribute, std::string> RPG_Common_AttributeToStringTable_t;
typedef RPG_Common_AttributeToStringTable_t::const_iterator RPG_Common_AttributeToStringTableIterator_t;

class RPG_Common_AttributeHelper
{
 public:
  inline static void init()
  {
    myRPG_Common_AttributeToStringTable.clear();
    myRPG_Common_AttributeToStringTable.insert(std::make_pair(ATTRIBUTE_CHARISMA, ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_CHARISMA")));
    myRPG_Common_AttributeToStringTable.insert(std::make_pair(ATTRIBUTE_CONSTITUTION, ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_CONSTITUTION")));
    myRPG_Common_AttributeToStringTable.insert(std::make_pair(ATTRIBUTE_DEXTERITY, ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_DEXTERITY")));
    myRPG_Common_AttributeToStringTable.insert(std::make_pair(ATTRIBUTE_INTELLIGENCE, ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_INTELLIGENCE")));
    myRPG_Common_AttributeToStringTable.insert(std::make_pair(ATTRIBUTE_STRENGTH, ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_STRENGTH")));
    myRPG_Common_AttributeToStringTable.insert(std::make_pair(ATTRIBUTE_WISDOM, ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_WISDOM")));
    myRPG_Common_AttributeToStringTable.insert(std::make_pair(ATTRIBUTE_ANY, ACE_TEXT_ALWAYS_CHAR("ATTRIBUTE_ANY")));
  };

  inline static std::string RPG_Common_AttributeToString(const RPG_Common_Attribute& element_in)
  {
    std::string result;
    RPG_Common_AttributeToStringTableIterator_t iterator = myRPG_Common_AttributeToStringTable.find(element_in);
    if (iterator != myRPG_Common_AttributeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMMON_ATTRIBUTE_INVALID");

    return result;
  };

  inline static RPG_Common_Attribute stringToRPG_Common_Attribute(const std::string& string_in)
  {
    RPG_Common_AttributeToStringTableIterator_t iterator = myRPG_Common_AttributeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Common_AttributeToStringTable.end());

    return RPG_COMMON_ATTRIBUTE_INVALID;
  };

  static RPG_Common_AttributeToStringTable_t myRPG_Common_AttributeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_AttributeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_AttributeHelper(const RPG_Common_AttributeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_AttributeHelper& operator=(const RPG_Common_AttributeHelper&));
};

#endif
