
// -------------------------------- * * * -------------------------------- //
// PLEASE NOTE: this file was/is generated by XML2CppCode 0.1
// -------------------------------- * * * -------------------------------- //

#pragma once
#ifndef RPG_MONSTER_PROPERTIESXML_H
#define RPG_MONSTER_PROPERTIESXML_H

struct RPG_Monster_PropertiesXML
{
  std::string name;
  RPG_Common_Size size;
  RPG_Common_CreatureType type;
  RPG_Dice_Roll hitDice;
  signed char initiative;
  unsigned char speed;
  RPG_Monster_NaturalArmorClass armorClass;
  std::vector<RPG_Monster_SpecialDefenseProperties> specialDefenses;
  RPG_Monster_Attack attack;
  std::vector<RPG_Monster_SpecialAttackProperties> specialAttacks;
  std::vector<RPG_Monster_SpecialAbilityProperties> specialAbilities;
  unsigned char space;
  unsigned char reach;
  RPG_Monster_SavingThrowModifiers saves;
  RPG_Character_Attributes attributes;
  RPG_Character_Skills skills;
  RPG_Character_Feats feats;
  RPG_Common_Environment environment;
  RPG_Monster_Organizations organizations;
  unsigned char challengeRating;
  unsigned char treasureModifier;
  RPG_Character_Alignment alignment;
  RPG_Monster_Advancement advancements;
  unsigned char levelAdjustment;
};

#endif