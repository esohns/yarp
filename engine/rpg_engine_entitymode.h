
// -------------------------------- * * * -------------------------------- //
// PLEASE NOTE: this file was/is generated by XML2CppCode 0.1
// -------------------------------- * * * -------------------------------- //

#pragma once
#ifndef RPG_ENGINE_ENTITYMODE_H
#define RPG_ENGINE_ENTITYMODE_H

enum RPG_Engine_EntityMode
{
  ENTITYMODE_FIGHTING = 0,
  ENTITYMODE_SEARCHING,
  ENTITYMODE_TRAVELLING,
  //
  RPG_ENGINE_ENTITYMODE_MAX,
  RPG_ENGINE_ENTITYMODE_INVALID
};

#include "rpg_engine_exports.h"

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Engine_EntityMode, std::string> RPG_Engine_EntityModeToStringTable_t;
typedef RPG_Engine_EntityModeToStringTable_t::const_iterator RPG_Engine_EntityModeToStringTableIterator_t;

class RPG_Engine_Export RPG_Engine_EntityModeHelper
{
 public:
  inline static void init()
  {
    myRPG_Engine_EntityModeToStringTable.clear();
    myRPG_Engine_EntityModeToStringTable.insert(std::make_pair(ENTITYMODE_FIGHTING, ACE_TEXT_ALWAYS_CHAR("ENTITYMODE_FIGHTING")));
    myRPG_Engine_EntityModeToStringTable.insert(std::make_pair(ENTITYMODE_SEARCHING, ACE_TEXT_ALWAYS_CHAR("ENTITYMODE_SEARCHING")));
    myRPG_Engine_EntityModeToStringTable.insert(std::make_pair(ENTITYMODE_TRAVELLING, ACE_TEXT_ALWAYS_CHAR("ENTITYMODE_TRAVELLING")));
  };

  inline static std::string RPG_Engine_EntityModeToString(const RPG_Engine_EntityMode& element_in)
  {
    std::string result;
    RPG_Engine_EntityModeToStringTableIterator_t iterator = myRPG_Engine_EntityModeToStringTable.find(element_in);
    if (iterator != myRPG_Engine_EntityModeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_ENGINE_ENTITYMODE_INVALID");

    return result;
  };

  inline static RPG_Engine_EntityMode stringToRPG_Engine_EntityMode(const std::string& string_in)
  {
    RPG_Engine_EntityModeToStringTableIterator_t iterator = myRPG_Engine_EntityModeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Engine_EntityModeToStringTable.end());

    return RPG_ENGINE_ENTITYMODE_INVALID;
  };

  static RPG_Engine_EntityModeToStringTable_t myRPG_Engine_EntityModeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Engine_EntityModeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Engine_EntityModeHelper(const RPG_Engine_EntityModeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Engine_EntityModeHelper& operator=(const RPG_Engine_EntityModeHelper&));
};

#endif