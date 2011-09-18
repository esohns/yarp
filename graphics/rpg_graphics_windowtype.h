
// -------------------------------- * * * -------------------------------- //
// PLEASE NOTE: this file was/is generated by XML2CppCode 0.1
// -------------------------------- * * * -------------------------------- //

#pragma once
#ifndef RPG_GRAPHICS_WINDOWTYPE_H
#define RPG_GRAPHICS_WINDOWTYPE_H

enum RPG_Graphics_WindowType
{
  WINDOW_HOTSPOT = 0,
  WINDOW_MAIN,
  WINDOW_MAP,
  WINDOW_MENU,
  WINDOW_MINIMAP,
  //
  RPG_GRAPHICS_WINDOWTYPE_MAX,
  RPG_GRAPHICS_WINDOWTYPE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Graphics_WindowType, std::string> RPG_Graphics_WindowTypeToStringTable_t;
typedef RPG_Graphics_WindowTypeToStringTable_t::const_iterator RPG_Graphics_WindowTypeToStringTableIterator_t;

class RPG_Graphics_WindowTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Graphics_WindowTypeToStringTable.clear();
    myRPG_Graphics_WindowTypeToStringTable.insert(std::make_pair(WINDOW_HOTSPOT, ACE_TEXT_ALWAYS_CHAR("WINDOW_HOTSPOT")));
    myRPG_Graphics_WindowTypeToStringTable.insert(std::make_pair(WINDOW_MAIN, ACE_TEXT_ALWAYS_CHAR("WINDOW_MAIN")));
    myRPG_Graphics_WindowTypeToStringTable.insert(std::make_pair(WINDOW_MAP, ACE_TEXT_ALWAYS_CHAR("WINDOW_MAP")));
    myRPG_Graphics_WindowTypeToStringTable.insert(std::make_pair(WINDOW_MENU, ACE_TEXT_ALWAYS_CHAR("WINDOW_MENU")));
    myRPG_Graphics_WindowTypeToStringTable.insert(std::make_pair(WINDOW_MINIMAP, ACE_TEXT_ALWAYS_CHAR("WINDOW_MINIMAP")));
  };

  inline static std::string RPG_Graphics_WindowTypeToString(const RPG_Graphics_WindowType& element_in)
  {
    std::string result;
    RPG_Graphics_WindowTypeToStringTableIterator_t iterator = myRPG_Graphics_WindowTypeToStringTable.find(element_in);
    if (iterator != myRPG_Graphics_WindowTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_GRAPHICS_WINDOWTYPE_INVALID");

    return result;
  };

  inline static RPG_Graphics_WindowType stringToRPG_Graphics_WindowType(const std::string& string_in)
  {
    RPG_Graphics_WindowTypeToStringTableIterator_t iterator = myRPG_Graphics_WindowTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Graphics_WindowTypeToStringTable.end());

    return RPG_GRAPHICS_WINDOWTYPE_INVALID;
  };

  static RPG_Graphics_WindowTypeToStringTable_t myRPG_Graphics_WindowTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_WindowTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_WindowTypeHelper(const RPG_Graphics_WindowTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_WindowTypeHelper& operator=(const RPG_Graphics_WindowTypeHelper&));
};

#endif