
// -------------------------------- * * * ----------------------------------- //
// PLEASE NOTE: this file was/is generated by XML2CppCode 0.0.1-devel
// -------------------------------- * * * ----------------------------------- //

#ifndef RPG_COMMON_CONDITION_H
#define RPG_COMMON_CONDITION_H

enum RPG_Common_Condition
{
  CONDITION_ALTERNATE_FORM = 0,
  CONDITION_NORMAL,
  CONDITION_ABILITY_DAMAGED,
  CONDITION_ABILITY_DRAINED,
  CONDITION_BLINDED,
  CONDITION_BLOWN_AWAY,
  CONDITION_BOUND,
  CONDITION_CHECKED,
  CONDITION_CONFUSED,
  CONDITION_COWERING,
  CONDITION_DAZED,
  CONDITION_DAZZLED,
  CONDITION_DEAD,
  CONDITION_DEAFENED,
  CONDITION_DISABLED,
  CONDITION_DYING,
  CONDITION_ENERGY_DRAINED,
  CONDITION_ENTANGLED,
  CONDITION_EXHAUSTED,
  CONDITION_FASCINATED,
  CONDITION_FATIGUED,
  CONDITION_FLAT_FOOTED,
  CONDITION_FRIGHTENED,
  CONDITION_GRAPPLING,
  CONDITION_HELD,
  CONDITION_HELPLESS,
  CONDITION_INCORPOREAL,
  CONDITION_INVISIBLE,
  CONDITION_KNOCKED_DOWN,
  CONDITION_NAUSEATED,
  CONDITION_PANICKED,
  CONDITION_PARALYZED,
  CONDITION_PETRIFIED,
  CONDITION_PINNED,
  CONDITION_PRONE,
  CONDITION_SHAKEN,
  CONDITION_SICKENED,
  CONDITION_SLEEPING,
  CONDITION_STABLE,
  CONDITION_STAGGERED,
  CONDITION_STUNNED,
  CONDITION_TURNED,
  CONDITION_UNCONSCIOUS,
  //
  RPG_COMMON_CONDITION_MAX,
  RPG_COMMON_CONDITION_INVALID
};

#include "ace/Global_Macros.h"

#include <map>
#include <string>

typedef std::map<RPG_Common_Condition, std::string> RPG_Common_ConditionToStringTable_t;
typedef RPG_Common_ConditionToStringTable_t::const_iterator RPG_Common_ConditionToStringTableIterator_t;

class RPG_Common_ConditionHelper
{
 public:
  inline static void init()
  {
    myRPG_Common_ConditionToStringTable.clear();
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_ALTERNATE_FORM, ACE_TEXT_ALWAYS_CHAR("CONDITION_ALTERNATE_FORM")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_NORMAL, ACE_TEXT_ALWAYS_CHAR("CONDITION_NORMAL")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_ABILITY_DAMAGED, ACE_TEXT_ALWAYS_CHAR("CONDITION_ABILITY_DAMAGED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_ABILITY_DRAINED, ACE_TEXT_ALWAYS_CHAR("CONDITION_ABILITY_DRAINED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_BLINDED, ACE_TEXT_ALWAYS_CHAR("CONDITION_BLINDED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_BLOWN_AWAY, ACE_TEXT_ALWAYS_CHAR("CONDITION_BLOWN_AWAY")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_BOUND, ACE_TEXT_ALWAYS_CHAR("CONDITION_BOUND")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_CHECKED, ACE_TEXT_ALWAYS_CHAR("CONDITION_CHECKED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_CONFUSED, ACE_TEXT_ALWAYS_CHAR("CONDITION_CONFUSED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_COWERING, ACE_TEXT_ALWAYS_CHAR("CONDITION_COWERING")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_DAZED, ACE_TEXT_ALWAYS_CHAR("CONDITION_DAZED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_DAZZLED, ACE_TEXT_ALWAYS_CHAR("CONDITION_DAZZLED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_DEAD, ACE_TEXT_ALWAYS_CHAR("CONDITION_DEAD")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_DEAFENED, ACE_TEXT_ALWAYS_CHAR("CONDITION_DEAFENED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_DISABLED, ACE_TEXT_ALWAYS_CHAR("CONDITION_DISABLED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_DYING, ACE_TEXT_ALWAYS_CHAR("CONDITION_DYING")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_ENERGY_DRAINED, ACE_TEXT_ALWAYS_CHAR("CONDITION_ENERGY_DRAINED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_ENTANGLED, ACE_TEXT_ALWAYS_CHAR("CONDITION_ENTANGLED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_EXHAUSTED, ACE_TEXT_ALWAYS_CHAR("CONDITION_EXHAUSTED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_FASCINATED, ACE_TEXT_ALWAYS_CHAR("CONDITION_FASCINATED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_FATIGUED, ACE_TEXT_ALWAYS_CHAR("CONDITION_FATIGUED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_FLAT_FOOTED, ACE_TEXT_ALWAYS_CHAR("CONDITION_FLAT_FOOTED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_FRIGHTENED, ACE_TEXT_ALWAYS_CHAR("CONDITION_FRIGHTENED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_GRAPPLING, ACE_TEXT_ALWAYS_CHAR("CONDITION_GRAPPLING")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_HELD, ACE_TEXT_ALWAYS_CHAR("CONDITION_HELD")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_HELPLESS, ACE_TEXT_ALWAYS_CHAR("CONDITION_HELPLESS")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_INCORPOREAL, ACE_TEXT_ALWAYS_CHAR("CONDITION_INCORPOREAL")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_INVISIBLE, ACE_TEXT_ALWAYS_CHAR("CONDITION_INVISIBLE")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_KNOCKED_DOWN, ACE_TEXT_ALWAYS_CHAR("CONDITION_KNOCKED_DOWN")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_NAUSEATED, ACE_TEXT_ALWAYS_CHAR("CONDITION_NAUSEATED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_PANICKED, ACE_TEXT_ALWAYS_CHAR("CONDITION_PANICKED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_PARALYZED, ACE_TEXT_ALWAYS_CHAR("CONDITION_PARALYZED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_PETRIFIED, ACE_TEXT_ALWAYS_CHAR("CONDITION_PETRIFIED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_PINNED, ACE_TEXT_ALWAYS_CHAR("CONDITION_PINNED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_PRONE, ACE_TEXT_ALWAYS_CHAR("CONDITION_PRONE")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_SHAKEN, ACE_TEXT_ALWAYS_CHAR("CONDITION_SHAKEN")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_SICKENED, ACE_TEXT_ALWAYS_CHAR("CONDITION_SICKENED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_SLEEPING, ACE_TEXT_ALWAYS_CHAR("CONDITION_SLEEPING")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_STABLE, ACE_TEXT_ALWAYS_CHAR("CONDITION_STABLE")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_STAGGERED, ACE_TEXT_ALWAYS_CHAR("CONDITION_STAGGERED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_STUNNED, ACE_TEXT_ALWAYS_CHAR("CONDITION_STUNNED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_TURNED, ACE_TEXT_ALWAYS_CHAR("CONDITION_TURNED")));
    myRPG_Common_ConditionToStringTable.insert(std::make_pair(CONDITION_UNCONSCIOUS, ACE_TEXT_ALWAYS_CHAR("CONDITION_UNCONSCIOUS")));
  };

  inline static std::string RPG_Common_ConditionToString(const RPG_Common_Condition& element_in)
  {
    std::string result;
    RPG_Common_ConditionToStringTableIterator_t iterator = myRPG_Common_ConditionToStringTable.find(element_in);
    if (iterator != myRPG_Common_ConditionToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMMON_CONDITION_INVALID");

    return result;
  };

  inline static RPG_Common_Condition stringToRPG_Common_Condition(const std::string& string_in)
  {
    RPG_Common_ConditionToStringTableIterator_t iterator = myRPG_Common_ConditionToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Common_ConditionToStringTable.end());

    return RPG_COMMON_CONDITION_INVALID;
  };

  static RPG_Common_ConditionToStringTable_t myRPG_Common_ConditionToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_ConditionHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_ConditionHelper(const RPG_Common_ConditionHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_ConditionHelper& operator=(const RPG_Common_ConditionHelper&));
};

#endif
