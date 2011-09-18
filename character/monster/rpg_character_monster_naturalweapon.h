
// -------------------------------- * * * -------------------------------- //
// PLEASE NOTE: this file was/is generated by XML2CppCode 0.1
// -------------------------------- * * * -------------------------------- //

#pragma once
#ifndef RPG_CHARACTER_MONSTER_NATURALWEAPON_H
#define RPG_CHARACTER_MONSTER_NATURALWEAPON_H

enum RPG_Character_Monster_NaturalWeapon
{
  NATURALWEAPON_ANY = 0,
  NATURALWEAPON_BITE,
  NATURALWEAPON_BLAST,
  NATURALWEAPON_CLAW_TALON,
  NATURALWEAPON_CONSTRICT,
  NATURALWEAPON_GORE,
  NATURALWEAPON_HOOF,
  NATURALWEAPON_PINCER,
  NATURALWEAPON_ROCK_STONE,
  NATURALWEAPON_SLAP_SLAM,
  NATURALWEAPON_SPIT,
  NATURALWEAPON_STING,
  NATURALWEAPON_TAIL,
  NATURALWEAPON_TENTACLE,
  NATURALWEAPON_WEB,
  //
  RPG_CHARACTER_MONSTER_NATURALWEAPON_MAX,
  RPG_CHARACTER_MONSTER_NATURALWEAPON_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Character_Monster_NaturalWeapon, std::string> RPG_Character_Monster_NaturalWeaponToStringTable_t;
typedef RPG_Character_Monster_NaturalWeaponToStringTable_t::const_iterator RPG_Character_Monster_NaturalWeaponToStringTableIterator_t;

class RPG_Character_Monster_NaturalWeaponHelper
{
 public:
  inline static void init()
  {
    myRPG_Character_Monster_NaturalWeaponToStringTable.clear();
    myRPG_Character_Monster_NaturalWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_ANY, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_ANY")));
    myRPG_Character_Monster_NaturalWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_BITE, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_BITE")));
    myRPG_Character_Monster_NaturalWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_BLAST, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_BLAST")));
    myRPG_Character_Monster_NaturalWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_CLAW_TALON, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_CLAW_TALON")));
    myRPG_Character_Monster_NaturalWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_CONSTRICT, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_CONSTRICT")));
    myRPG_Character_Monster_NaturalWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_GORE, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_GORE")));
    myRPG_Character_Monster_NaturalWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_HOOF, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_HOOF")));
    myRPG_Character_Monster_NaturalWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_PINCER, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_PINCER")));
    myRPG_Character_Monster_NaturalWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_ROCK_STONE, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_ROCK_STONE")));
    myRPG_Character_Monster_NaturalWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_SLAP_SLAM, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_SLAP_SLAM")));
    myRPG_Character_Monster_NaturalWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_SPIT, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_SPIT")));
    myRPG_Character_Monster_NaturalWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_STING, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_STING")));
    myRPG_Character_Monster_NaturalWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_TAIL, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_TAIL")));
    myRPG_Character_Monster_NaturalWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_TENTACLE, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_TENTACLE")));
    myRPG_Character_Monster_NaturalWeaponToStringTable.insert(std::make_pair(NATURALWEAPON_WEB, ACE_TEXT_ALWAYS_CHAR("NATURALWEAPON_WEB")));
  };

  inline static std::string RPG_Character_Monster_NaturalWeaponToString(const RPG_Character_Monster_NaturalWeapon& element_in)
  {
    std::string result;
    RPG_Character_Monster_NaturalWeaponToStringTableIterator_t iterator = myRPG_Character_Monster_NaturalWeaponToStringTable.find(element_in);
    if (iterator != myRPG_Character_Monster_NaturalWeaponToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_CHARACTER_MONSTER_NATURALWEAPON_INVALID");

    return result;
  };

  inline static RPG_Character_Monster_NaturalWeapon stringToRPG_Character_Monster_NaturalWeapon(const std::string& string_in)
  {
    RPG_Character_Monster_NaturalWeaponToStringTableIterator_t iterator = myRPG_Character_Monster_NaturalWeaponToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Character_Monster_NaturalWeaponToStringTable.end());

    return RPG_CHARACTER_MONSTER_NATURALWEAPON_INVALID;
  };

  static RPG_Character_Monster_NaturalWeaponToStringTable_t myRPG_Character_Monster_NaturalWeaponToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Monster_NaturalWeaponHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Monster_NaturalWeaponHelper(const RPG_Character_Monster_NaturalWeaponHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Monster_NaturalWeaponHelper& operator=(const RPG_Character_Monster_NaturalWeaponHelper&));
};

#endif