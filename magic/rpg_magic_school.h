
// -------------------------------- * * * ----------------------------------- //
// PLEASE NOTE: this file was/is generated by XML2CppCode 0.0.1-devel
// -------------------------------- * * * ----------------------------------- //

#ifndef RPG_MAGIC_SCHOOL_H
#define RPG_MAGIC_SCHOOL_H

enum RPG_Magic_School
{
  SCHOOL_NONE = 0,
  SCHOOL_ABJURATION,
  SCHOOL_CONJURATION,
  SCHOOL_DIVINATION,
  SCHOOL_ENCHANTMENT,
  SCHOOL_EVOCATION,
  SCHOOL_ILLUSION,
  SCHOOL_NECROMANCY,
  SCHOOL_TRANSMUTATION,
  //
  RPG_MAGIC_SCHOOL_MAX,
  RPG_MAGIC_SCHOOL_INVALID
};

#include "ace/Global_Macros.h"

#include <map>
#include <string>

typedef std::map<RPG_Magic_School, std::string> RPG_Magic_SchoolToStringTable_t;
typedef RPG_Magic_SchoolToStringTable_t::const_iterator RPG_Magic_SchoolToStringTableIterator_t;

class RPG_Magic_SchoolHelper
{
 public:
  inline static void init()
  {
    myRPG_Magic_SchoolToStringTable.clear();
    myRPG_Magic_SchoolToStringTable.insert(std::make_pair(SCHOOL_NONE, ACE_TEXT_ALWAYS_CHAR("SCHOOL_NONE")));
    myRPG_Magic_SchoolToStringTable.insert(std::make_pair(SCHOOL_ABJURATION, ACE_TEXT_ALWAYS_CHAR("SCHOOL_ABJURATION")));
    myRPG_Magic_SchoolToStringTable.insert(std::make_pair(SCHOOL_CONJURATION, ACE_TEXT_ALWAYS_CHAR("SCHOOL_CONJURATION")));
    myRPG_Magic_SchoolToStringTable.insert(std::make_pair(SCHOOL_DIVINATION, ACE_TEXT_ALWAYS_CHAR("SCHOOL_DIVINATION")));
    myRPG_Magic_SchoolToStringTable.insert(std::make_pair(SCHOOL_ENCHANTMENT, ACE_TEXT_ALWAYS_CHAR("SCHOOL_ENCHANTMENT")));
    myRPG_Magic_SchoolToStringTable.insert(std::make_pair(SCHOOL_EVOCATION, ACE_TEXT_ALWAYS_CHAR("SCHOOL_EVOCATION")));
    myRPG_Magic_SchoolToStringTable.insert(std::make_pair(SCHOOL_ILLUSION, ACE_TEXT_ALWAYS_CHAR("SCHOOL_ILLUSION")));
    myRPG_Magic_SchoolToStringTable.insert(std::make_pair(SCHOOL_NECROMANCY, ACE_TEXT_ALWAYS_CHAR("SCHOOL_NECROMANCY")));
    myRPG_Magic_SchoolToStringTable.insert(std::make_pair(SCHOOL_TRANSMUTATION, ACE_TEXT_ALWAYS_CHAR("SCHOOL_TRANSMUTATION")));
  };

  inline static std::string RPG_Magic_SchoolToString(const RPG_Magic_School& element_in)
  {
    std::string result;
    RPG_Magic_SchoolToStringTableIterator_t iterator = myRPG_Magic_SchoolToStringTable.find(element_in);
    if (iterator != myRPG_Magic_SchoolToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_MAGIC_SCHOOL_INVALID");

    return result;
  };

  inline static RPG_Magic_School stringToRPG_Magic_School(const std::string& string_in)
  {
    RPG_Magic_SchoolToStringTableIterator_t iterator = myRPG_Magic_SchoolToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Magic_SchoolToStringTable.end());

    return RPG_MAGIC_SCHOOL_INVALID;
  };

  static RPG_Magic_SchoolToStringTable_t myRPG_Magic_SchoolToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_SchoolHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_SchoolHelper(const RPG_Magic_SchoolHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_SchoolHelper& operator=(const RPG_Magic_SchoolHelper&));
};

#endif
