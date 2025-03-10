
// -------------------------------- * * * ----------------------------------- //
// PLEASE NOTE: this file was/is generated by XML2CppCode 0.0.1-devel
// -------------------------------- * * * ----------------------------------- //

#ifndef RPG_COMMON_AREAOFEFFECT_H
#define RPG_COMMON_AREAOFEFFECT_H

enum RPG_Common_AreaOfEffect
{
  AREA_CONE = 0,
  AREA_CUBE,
  AREA_CYLINDER,
  AREA_LINE_HORIZONTAL,
  AREA_LINE_VERTICAL,
  AREA_RING,
  AREA_SPHERE,
  AREA_WALL,
  //
  RPG_COMMON_AREAOFEFFECT_MAX,
  RPG_COMMON_AREAOFEFFECT_INVALID
};

#include "ace/Global_Macros.h"

#include <map>
#include <string>

typedef std::map<RPG_Common_AreaOfEffect, std::string> RPG_Common_AreaOfEffectToStringTable_t;
typedef RPG_Common_AreaOfEffectToStringTable_t::const_iterator RPG_Common_AreaOfEffectToStringTableIterator_t;

class RPG_Common_AreaOfEffectHelper
{
 public:
  inline static void init()
  {
    myRPG_Common_AreaOfEffectToStringTable.clear();
    myRPG_Common_AreaOfEffectToStringTable.insert(std::make_pair(AREA_CONE, ACE_TEXT_ALWAYS_CHAR("AREA_CONE")));
    myRPG_Common_AreaOfEffectToStringTable.insert(std::make_pair(AREA_CUBE, ACE_TEXT_ALWAYS_CHAR("AREA_CUBE")));
    myRPG_Common_AreaOfEffectToStringTable.insert(std::make_pair(AREA_CYLINDER, ACE_TEXT_ALWAYS_CHAR("AREA_CYLINDER")));
    myRPG_Common_AreaOfEffectToStringTable.insert(std::make_pair(AREA_LINE_HORIZONTAL, ACE_TEXT_ALWAYS_CHAR("AREA_LINE_HORIZONTAL")));
    myRPG_Common_AreaOfEffectToStringTable.insert(std::make_pair(AREA_LINE_VERTICAL, ACE_TEXT_ALWAYS_CHAR("AREA_LINE_VERTICAL")));
    myRPG_Common_AreaOfEffectToStringTable.insert(std::make_pair(AREA_RING, ACE_TEXT_ALWAYS_CHAR("AREA_RING")));
    myRPG_Common_AreaOfEffectToStringTable.insert(std::make_pair(AREA_SPHERE, ACE_TEXT_ALWAYS_CHAR("AREA_SPHERE")));
    myRPG_Common_AreaOfEffectToStringTable.insert(std::make_pair(AREA_WALL, ACE_TEXT_ALWAYS_CHAR("AREA_WALL")));
  };

  inline static std::string RPG_Common_AreaOfEffectToString(const RPG_Common_AreaOfEffect& element_in)
  {
    std::string result;
    RPG_Common_AreaOfEffectToStringTableIterator_t iterator = myRPG_Common_AreaOfEffectToStringTable.find(element_in);
    if (iterator != myRPG_Common_AreaOfEffectToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMMON_AREAOFEFFECT_INVALID");

    return result;
  };

  inline static RPG_Common_AreaOfEffect stringToRPG_Common_AreaOfEffect(const std::string& string_in)
  {
    RPG_Common_AreaOfEffectToStringTableIterator_t iterator = myRPG_Common_AreaOfEffectToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Common_AreaOfEffectToStringTable.end());

    return RPG_COMMON_AREAOFEFFECT_INVALID;
  };

  static RPG_Common_AreaOfEffectToStringTable_t myRPG_Common_AreaOfEffectToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_AreaOfEffectHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_AreaOfEffectHelper(const RPG_Common_AreaOfEffectHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_AreaOfEffectHelper& operator=(const RPG_Common_AreaOfEffectHelper&));
};

#endif
