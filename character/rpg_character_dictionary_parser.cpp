/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
 *   erik.sohns@web.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "rpg_character_dictionary_parser.h"

#include "rpg_character_common_tools.h"
#include "rpg_character_skills_common_tools.h"
#include "rpg_character_monster_common.h"
#include "rpg_character_monster_common_tools.h"

#include <rpg_chance_dice_common_tools.h>

#include <ace/Log_Msg.h>

RPG_Character_Dictionary_Parser::RPG_Character_Dictionary_Parser(RPG_Character_MonsterDictionary_t* monsterDictionary_in)
 : myMonsterDictionary(monsterDictionary_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Dictionary_Parser::RPG_Character_Dictionary_Parser"));

}

RPG_Character_Dictionary_Parser::~RPG_Character_Dictionary_Parser()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Dictionary_Parser::~RPG_Character_Dictionary_Parser"));

}

// void RPG_Character_Dictionary_Parser::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Character_Dictionary_Parser::pre"));
// 
// }

// void RPG_Character_Dictionary_Parser::RPG_Character_MonsterDictionary()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::RPG_Character_MonsterDictionary"));
// 
//   // TODO
//   //
// }

void RPG_Character_Dictionary_Parser::post_RPG_Character_Dictionary_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Dictionary_Parser::post_RPG_Character_Dictionary_Type"));

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("finished parsing character dictionary, retrieved %d monster types...\n"),
//              myMonsterDictionary->size()));
}

RPG_Character_MonsterDictionary_Type::RPG_Character_MonsterDictionary_Type(RPG_Character_MonsterDictionary_t* monsterDictionary_in)
 : myMonsterDictionary(monsterDictionary_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterDictionary_Type::RPG_Character_MonsterDictionary_Type"));

}

// void RPG_Character_MonsterDictionary_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Character_MonsterDictionary_Type::pre"));
// 
// }

void RPG_Character_MonsterDictionary_Type::monster(const RPG_Character_MonsterProperties_XML& monster_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterDictionary_Type::monster"));

  RPG_Character_MonsterProperties properties;
  properties.size             = monster_in.size;
  properties.type             = monster_in.type;
  properties.hitDice          = monster_in.hitDice;
  properties.initiative       = monster_in.initiative;
  properties.speed            = monster_in.speed;
  properties.armorClass       = monster_in.armorClass;
  properties.attack           = monster_in.attack;
  properties.space            = monster_in.space;
  properties.reach            = monster_in.reach;
  properties.saves            = monster_in.saves;
  properties.attributes       = monster_in.attributes;
  properties.skills           = monster_in.skills;
  properties.feats            = monster_in.feats;
  properties.environment      = monster_in.environment;
  properties.organizations    = monster_in.organizations;
  properties.challengeRating  = monster_in.challengeRating;
  properties.treasureModifier = monster_in.treasureModifier;
  properties.alignment        = monster_in.alignment;
  properties.advancement      = monster_in.advancement;
  properties.levelAdjustment  = monster_in.levelAdjustment;

  myMonsterDictionary->insert(std::make_pair(monster_in.name, properties));
}

void RPG_Character_MonsterDictionary_Type::post_RPG_Character_MonsterDictionary_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterDictionary_Type::post_RPG_Character_MonsterDictionary_Type"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished parsing monster dictionary, retrieved %d types...\n"),
             myMonsterDictionary->size()));
}

RPG_Character_Size RPG_Character_Size_Type::post_RPG_Character_Size_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Size_Type::post_RPG_Character_Size_Type"));

  return RPG_Character_Monster_Common_Tools::stringToSize(post_string());
}

RPG_Character_MonsterType_Type::RPG_Character_MonsterType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterType_Type::RPG_Character_MonsterType_Type"));

  myCurrentMonsterType.metaType = MONSTERMETATYPE_INVALID;
  myCurrentMonsterType.subTypes.clear();
}

void RPG_Character_MonsterType_Type::metaType(const RPG_Character_MonsterMetaType& metaType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterType_Type::metaType"));

  myCurrentMonsterType.metaType = metaType_in;
}

void RPG_Character_MonsterType_Type::subType(const RPG_Character_MonsterSubType& subType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterType_Type::subType"));

  myCurrentMonsterType.subTypes.insert(subType_in);
}

RPG_Character_MonsterType RPG_Character_MonsterType_Type::post_RPG_Character_MonsterType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterType_Type::post_RPG_Character_MonsterType_Type"));

  RPG_Character_MonsterType result = myCurrentMonsterType;

  // clear structure
  myCurrentMonsterType.metaType = MONSTERMETATYPE_INVALID;
  myCurrentMonsterType.subTypes.clear();

  return result;
}

RPG_Character_MonsterMetaType RPG_Character_MonsterMetaType_Type::post_RPG_Character_MonsterMetaType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterMetaType_Type::post_RPG_Character_MonsterMetaType_Type"));

  return RPG_Character_Monster_Common_Tools::stringToMonsterMetaType(post_string());
}

RPG_Character_MonsterSubType RPG_Character_MonsterSubType_Type::post_RPG_Character_MonsterSubType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterSubType_Type::post_RPG_Character_MonsterSubType_Type"));

  return RPG_Character_Monster_Common_Tools::stringToMonsterSubType(post_string());
}

RPG_Chance_DiceType RPG_Chance_DiceType_Type::post_RPG_Chance_DiceType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_DiceType_Type::post_RPG_Chance_DiceType_Type"));

  return RPG_Chance_Dice_Common_Tools::stringToDiceType(post_string());
}

RPG_Chance_Roll_Type::RPG_Chance_Roll_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_Roll_Type::RPG_Chance_Roll_Type"));

  myCurrentRoll.numDice = 0;
  myCurrentRoll.typeDice = D_TYPE_INVALID;
  myCurrentRoll.modifier = 0;
}

void RPG_Chance_Roll_Type::numDice(unsigned int numDice_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_Roll_Type::numDice"));

  myCurrentRoll.numDice = numDice_in;
}

void RPG_Chance_Roll_Type::typeDice(const RPG_Chance_DiceType& typeDice_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_Roll_Type::typeDice"));

  myCurrentRoll.typeDice = typeDice_in;
}

void RPG_Chance_Roll_Type::modifier(long long modifier_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_Roll_Type::modifier"));

  myCurrentRoll.modifier = modifier_in;
}

RPG_Chance_Roll RPG_Chance_Roll_Type::post_RPG_Chance_Roll_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Chance_Roll_Type::post_RPG_Chance_Roll_Type"));

  RPG_Character_HitDice result = myCurrentRoll;

  // clear structure
  myCurrentRoll.numDice = 0;
  myCurrentRoll.typeDice = D_TYPE_INVALID;
  myCurrentRoll.modifier = 0;

  return result;
}

RPG_Character_MonsterWeapon RPG_Character_MonsterWeapon_Type::post_RPG_Character_MonsterWeapon_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterWeapon_Type::post_RPG_Character_MonsterWeapon_Type"));

  return RPG_Character_Monster_Common_Tools::stringToMonsterWeapon(post_string());
}

RPG_Character_MonsterAttackForm RPG_Character_MonsterAttackForm_Type::post_RPG_Character_MonsterAttackForm_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterAttackForm_Type::post_RPG_Character_MonsterAttackForm_Type"));

  return RPG_Character_Monster_Common_Tools::stringToMonsterAttackForm(post_string());
}

RPG_Character_MonsterArmorClass_Type::RPG_Character_MonsterArmorClass_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterArmorClass_Type::RPG_Character_MonsterArmorClass_Type"));

  myCurrentArmorClass.normal = 0;
  myCurrentArmorClass.touch = 0;
  myCurrentArmorClass.flatFooted = 0;
}

void RPG_Character_MonsterArmorClass_Type::normal(unsigned int normal_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterArmorClass_Type::normal"));

  myCurrentArmorClass.normal = normal_in;
}

void RPG_Character_MonsterArmorClass_Type::touch(unsigned int touch_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterArmorClass_Type::touch"));

  myCurrentArmorClass.touch = touch_in;
}

void RPG_Character_MonsterArmorClass_Type::flatFooted(unsigned int flatFooted_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterArmorClass_Type::flatFooted"));

  myCurrentArmorClass.flatFooted = flatFooted_in;
}

RPG_Character_MonsterArmorClass RPG_Character_MonsterArmorClass_Type::post_RPG_Character_MonsterArmorClass_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterArmorClass_Type::post_RPG_Character_MonsterArmorClass_Type"));

  RPG_Character_MonsterArmorClass result = myCurrentArmorClass;

  // clear structure
  myCurrentArmorClass.normal = 0;
  myCurrentArmorClass.touch = 0;
  myCurrentArmorClass.flatFooted = 0;

  return result;
}

RPG_Character_MonsterAttackAction_Type::RPG_Character_MonsterAttackAction_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterAttackAction_Type::RPG_Character_MonsterAttackAction_Type"));

  myCurrentMonsterAttackAction.monsterWeapon = MONSTERWEAPON_INVALID;
  myCurrentMonsterAttackAction.attackBonus = 0;
  myCurrentMonsterAttackAction.attackForm = ATTACK_INVALID;
  myCurrentMonsterAttackAction.damage.numDice = 0;
  myCurrentMonsterAttackAction.damage.typeDice = D_TYPE_INVALID;
  myCurrentMonsterAttackAction.damage.modifier = 0;
  myCurrentMonsterAttackAction.numAttacksPerRound = 0;
}

void RPG_Character_MonsterAttackAction_Type::monsterWeapon(const RPG_Character_MonsterWeapon& monsterWeapon_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterAttackAction_Type::monsterWeapon"));

  myCurrentMonsterAttackAction.monsterWeapon = monsterWeapon_in;
}

void RPG_Character_MonsterAttackAction_Type::attackBonus(int attackBonus_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterAttackAction_Type::attackBonus"));

  myCurrentMonsterAttackAction.attackBonus = attackBonus_in;
}

void RPG_Character_MonsterAttackAction_Type::attackForm(const RPG_Character_MonsterAttackForm& attackForm_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterAttackAction_Type::attackForm"));

  myCurrentMonsterAttackAction.attackForm = attackForm_in;
}

void RPG_Character_MonsterAttackAction_Type::damage(const RPG_Character_Damage& damage_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterAttackAction_Type::damage"));

  myCurrentMonsterAttackAction.damage = damage_in;
}

void RPG_Character_MonsterAttackAction_Type::numAttacksPerRound(unsigned int numAttacksPerRound_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterAttackAction_Type::numAttacksPerRound"));

  myCurrentMonsterAttackAction.numAttacksPerRound = numAttacksPerRound_in;
}

RPG_Character_MonsterAttackAction RPG_Character_MonsterAttackAction_Type::post_RPG_Character_MonsterAttackAction_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterAttackAction_Type::post_RPG_Character_MonsterAttackAction_Type"));

  RPG_Character_MonsterAttackAction result = myCurrentMonsterAttackAction;

  // clear structure
  myCurrentMonsterAttackAction.monsterWeapon = MONSTERWEAPON_INVALID;
  myCurrentMonsterAttackAction.attackBonus = 0;
  myCurrentMonsterAttackAction.attackForm = ATTACK_INVALID;
  myCurrentMonsterAttackAction.damage.numDice = 0;
  myCurrentMonsterAttackAction.damage.typeDice = D_TYPE_INVALID;
  myCurrentMonsterAttackAction.damage.modifier = 0;
  myCurrentMonsterAttackAction.numAttacksPerRound = 0;

  return result;
}

RPG_Character_MonsterAttack_Type::RPG_Character_MonsterAttack_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterAttack_Type::RPG_Character_MonsterAttack_Type"));

  myCurrentMonsterAttack.baseAttackBonus = 0;
  myCurrentMonsterAttack.grappleBonus = 0;
  myCurrentMonsterAttack.attackActions.clear();
}

void RPG_Character_MonsterAttack_Type::baseAttackBonus(int baseAttackBonus_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterAttack_Type::baseAttackBonus"));

  myCurrentMonsterAttack.baseAttackBonus = baseAttackBonus_in;
}

void RPG_Character_MonsterAttack_Type::grappleBonus(int grappleBonus_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterAttack_Type::grappleBonus"));

  myCurrentMonsterAttack.grappleBonus = grappleBonus_in;
}

void RPG_Character_MonsterAttack_Type::attackAction(const RPG_Character_MonsterAttackAction& attackAction_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterAttack_Type::attackAction"));

  myCurrentMonsterAttack.attackActions.push_back(attackAction_in);
}

RPG_Character_MonsterAttack RPG_Character_MonsterAttack_Type::post_RPG_Character_MonsterAttack_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterAttack_Type::post_RPG_Character_MonsterAttack_Type"));

  RPG_Character_MonsterAttack result = myCurrentMonsterAttack;

  // clear structure
  myCurrentMonsterAttack.baseAttackBonus = 0;
  myCurrentMonsterAttack.grappleBonus = 0;
  myCurrentMonsterAttack.attackActions.clear();

  return result;
}

RPG_Character_SavingThrowModifiers_Type::RPG_Character_SavingThrowModifiers_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_SavingThrowModifiers_Type::RPG_Character_SavingThrowModifiers_Type"));

  myCurrentSavingThrowModifiers.fortitude = 0;
  myCurrentSavingThrowModifiers.reflex = 0;
  myCurrentSavingThrowModifiers.will = 0;
}

void RPG_Character_SavingThrowModifiers_Type::fortitude(int fortitude_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_SavingThrowModifiers_Type::fortitude"));

  myCurrentSavingThrowModifiers.fortitude = fortitude_in;
}

void RPG_Character_SavingThrowModifiers_Type::reflex(int reflex_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_SavingThrowModifiers_Type::reflex"));

  myCurrentSavingThrowModifiers.reflex = reflex_in;
}

void RPG_Character_SavingThrowModifiers_Type::will(int will_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_SavingThrowModifiers_Type::will"));

  myCurrentSavingThrowModifiers.will = will_in;
}

RPG_Character_SavingThrowModifiers RPG_Character_SavingThrowModifiers_Type::post_RPG_Character_SavingThrowModifiers_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_SavingThrowModifiers_Type::post_RPG_Character_SavingThrowModifiers_Type"));

  RPG_Character_SavingThrowModifiers result = myCurrentSavingThrowModifiers;

  // clear structure
  myCurrentSavingThrowModifiers.fortitude = 0;
  myCurrentSavingThrowModifiers.reflex = 0;
  myCurrentSavingThrowModifiers.will = 0;

  return result;
}

RPG_Character_Attributes_Type::RPG_Character_Attributes_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::RPG_Character_Attributes_Type"));

  myCurrentAttributes.strength = 0;
  myCurrentAttributes.dexterity = 0;
  myCurrentAttributes.constitution = 0;
  myCurrentAttributes.intelligence = 0;
  myCurrentAttributes.wisdom = 0;
  myCurrentAttributes.charisma = 0;
}

void RPG_Character_Attributes_Type::strength(unsigned int strength_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::strength"));

  myCurrentAttributes.strength = strength_in;
}

void RPG_Character_Attributes_Type::dexterity(unsigned int dexterity_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::dexterity"));

  myCurrentAttributes.dexterity = dexterity_in;
}

void RPG_Character_Attributes_Type::constitution(unsigned int constitution_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::constitution"));

  myCurrentAttributes.constitution = constitution_in;
}

void RPG_Character_Attributes_Type::intelligence(unsigned int intelligence_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::intelligence"));

  myCurrentAttributes.intelligence = intelligence_in;
}

void RPG_Character_Attributes_Type::wisdom(unsigned int wisdom_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::wisdom"));

  myCurrentAttributes.wisdom = wisdom_in;
}

void RPG_Character_Attributes_Type::charisma(unsigned int charisma_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::charisma"));

  myCurrentAttributes.charisma = charisma_in;
}

RPG_Character_Attributes RPG_Character_Attributes_Type::post_RPG_Character_Attributes_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Attributes_Type::post_RPG_Character_Attributes_Type"));

  RPG_Character_Attributes result = myCurrentAttributes;

  // clear structure
  myCurrentAttributes.strength = 0;
  myCurrentAttributes.dexterity = 0;
  myCurrentAttributes.constitution = 0;
  myCurrentAttributes.intelligence = 0;
  myCurrentAttributes.wisdom = 0;
  myCurrentAttributes.charisma = 0;

  return result;
}

RPG_Character_Skill RPG_Character_Skill_Type::post_RPG_Character_Skill_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skill_Type::post_RPG_Character_Skill_Type"));

  return RPG_Character_Skills_Common_Tools::stringToSkill(post_string());
}

RPG_Character_SkillValue_Type::RPG_Character_SkillValue_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_SkillValue_Type::RPG_Character_SkillValue_Type"));

  myCurrentSkill.first = SKILL_INVALID;
  myCurrentSkill.second = 0;
}

void RPG_Character_SkillValue_Type::skill(const RPG_Character_Skill& skill_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_SkillValue_Type::skill"));

  myCurrentSkill.first = skill_in;
}

void RPG_Character_SkillValue_Type::rank(int rank_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_SkillValue_Type::rank"));

  myCurrentSkill.second = rank_in;
}

RPG_Character_SkillsItem_t RPG_Character_SkillValue_Type::post_RPG_Character_SkillValue_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_SkillValue_Type::post_RPG_Character_SkillValue_Type"));

  RPG_Character_SkillsItem_t result = myCurrentSkill;

  // clear structure
  myCurrentSkill.first = SKILL_INVALID;
  myCurrentSkill.second = 0;

  return result;
}

RPG_Character_Skills_Type::RPG_Character_Skills_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Type::RPG_Character_Skills_Type"));

  myCurrentSkills.clear();
}

void RPG_Character_Skills_Type::skill(const RPG_Character_SkillsItem_t& skill_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Type::skill"));

  myCurrentSkills.insert(skill_in);
}

RPG_Character_Skills_t RPG_Character_Skills_Type::post_RPG_Character_Skills_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Skills_Type::post_RPG_Character_Skills_Type"));

  RPG_Character_Skills_t result = myCurrentSkills;

  // clear structure
  myCurrentSkills.clear();

  return result;
}

RPG_Character_Feat RPG_Character_Feat_Type::post_RPG_Character_Feat_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Feat_Type::post_RPG_Character_Feat_Type"));

  return RPG_Character_Skills_Common_Tools::stringToFeat(post_string());
}

RPG_Character_Feats_Type::RPG_Character_Feats_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Feats_Type::RPG_Character_Feats_Type"));

  myCurrentFeats.clear();
}

void RPG_Character_Feats_Type::feat(const RPG_Character_Feat& feat_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Feats_Type::feat"));

  myCurrentFeats.insert(feat_in);
}

RPG_Character_Feats_t RPG_Character_Feats_Type::post_RPG_Character_Feats_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Feats_Type::post_RPG_Character_Feats_Type"));

  RPG_Character_Feats_t result = myCurrentFeats;

  // clear structure
  myCurrentFeats.clear();

  return result;
}

RPG_Character_Environment RPG_Character_Environment_Type::post_RPG_Character_Environment_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Environment_Type::post_RPG_Character_Environment_Type"));

  return RPG_Character_Monster_Common_Tools::stringToEnvironment(post_string());
}

RPG_Character_Organization RPG_Character_Organization_Type::post_RPG_Character_Organization_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Organization_Type::post_RPG_Character_Organization_Type"));

  return RPG_Character_Monster_Common_Tools::stringToOrganization(post_string());
}

RPG_Character_AlignmentCivic RPG_Character_AlignmentCivic_Type::post_RPG_Character_AlignmentCivic_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_AlignmentCivic_Type::post_RPG_Character_AlignmentCivic_Type"));

  return RPG_Character_Common_Tools::stringToAlignmentCivic(post_string());
}

RPG_Character_AlignmentEthic RPG_Character_AlignmentEthic_Type::post_RPG_Character_AlignmentEthic_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_AlignmentEthic_Type::post_RPG_Character_AlignmentEthic_Type"));

  return RPG_Character_Common_Tools::stringToAlignmentEthic(post_string());
}

RPG_Character_Alignment_Type::RPG_Character_Alignment_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Alignment_Type::RPG_Character_Alignment_Type"));

  myCurrentAlignment.civicAlignment = ALIGNMENTCIVIC_INVALID;
  myCurrentAlignment.ethicAlignment = ALIGNMENTETHIC_INVALID;
}

void RPG_Character_Alignment_Type::civic(const RPG_Character_AlignmentCivic& alignmentCivic_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Alignment_Type::civic"));

  myCurrentAlignment.civicAlignment = alignmentCivic_in;
}

void RPG_Character_Alignment_Type::ethic(const RPG_Character_AlignmentEthic& alignmentEthic_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Alignment_Type::ethic"));

  myCurrentAlignment.ethicAlignment = alignmentEthic_in;
}

RPG_Character_Alignment RPG_Character_Alignment_Type::post_RPG_Character_Alignment_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Alignment_Type::post_RPG_Character_Alignment_Type"));

  RPG_Character_Alignment result = myCurrentAlignment;

  // clear structure
  myCurrentAlignment.civicAlignment = ALIGNMENTCIVIC_INVALID;
  myCurrentAlignment.ethicAlignment = ALIGNMENTETHIC_INVALID;

  return result;
}

RPG_Character_MonsterAdvancementStep_Type::RPG_Character_MonsterAdvancementStep_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterAdvancementStep_Type::RPG_Character_MonsterAdvancementStep_Type"));

  myCurrentAdvancementStep.first = SIZE_INVALID;
  myCurrentAdvancementStep.second.numDice = 0;
  myCurrentAdvancementStep.second.typeDice = D_TYPE_INVALID;
  myCurrentAdvancementStep.second.modifier = 0;
}

void RPG_Character_MonsterAdvancementStep_Type::size(const RPG_Character_Size& size_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterAdvancementStep_Type::size"));

  myCurrentAdvancementStep.first = size_in;
}

void RPG_Character_MonsterAdvancementStep_Type::range(const RPG_Character_Damage& range_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterAdvancementStep_Type::range"));

  myCurrentAdvancementStep.second = range_in;
}

RPG_Character_MonsterAdvancementStep_t RPG_Character_MonsterAdvancementStep_Type::post_RPG_Character_MonsterAdvancementStep_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterAdvancementStep_Type::post_RPG_Character_MonsterAdvancementStep_Type"));

  RPG_Character_MonsterAdvancementStep_t result = myCurrentAdvancementStep;

  // clear structure
  myCurrentAdvancementStep.first = SIZE_INVALID;
  myCurrentAdvancementStep.second.numDice = 0;
  myCurrentAdvancementStep.second.typeDice = D_TYPE_INVALID;
  myCurrentAdvancementStep.second.modifier = 0;

  return result;
}

RPG_Character_MonsterAdvancement_Type::RPG_Character_MonsterAdvancement_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterAdvancement_Type::RPG_Character_MonsterAdvancement_Type"));

  myCurrentMonsterAdvancement.clear();
}

void RPG_Character_MonsterAdvancement_Type::step(const RPG_Character_MonsterAdvancementStep_t& advancementStep_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterAdvancement_Type::step"));

  myCurrentMonsterAdvancement.push_back(advancementStep_in);
}

RPG_Character_MonsterAdvancement_t RPG_Character_MonsterAdvancement_Type::post_RPG_Character_MonsterAdvancement_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterAdvancement_Type::post_RPG_Character_MonsterAdvancement_Type"));

  RPG_Character_MonsterAdvancement_t result = myCurrentMonsterAdvancement;

  // clear container
  myCurrentMonsterAdvancement.clear();

  return result;
}

RPG_Character_MonsterProperties_Type::RPG_Character_MonsterProperties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::RPG_Character_MonsterProperties_Type"));

  myCurrentProperties.name.resize(0);
  myCurrentProperties.size = SIZE_INVALID;
  myCurrentProperties.type.metaType = MONSTERMETATYPE_INVALID;
  myCurrentProperties.type.subTypes.clear();
  myCurrentProperties.hitDice.numDice = 0;
  myCurrentProperties.hitDice.typeDice = D_TYPE_INVALID;
  myCurrentProperties.hitDice.modifier = 0;
  myCurrentProperties.initiative = 0;
  myCurrentProperties.speed = 0;
  myCurrentProperties.armorClass.normal = 0;
  myCurrentProperties.armorClass.touch = 0;
  myCurrentProperties.armorClass.flatFooted = 0;
  myCurrentProperties.attack.baseAttackBonus = 0;
  myCurrentProperties.attack.grappleBonus = 0;
  myCurrentProperties.attack.attackActions.clear();
  myCurrentProperties.space = 0;
  myCurrentProperties.reach = 0;
  myCurrentProperties.saves.fortitude = 0;
  myCurrentProperties.saves.reflex = 0;
  myCurrentProperties.saves.will = 0;
  myCurrentProperties.attributes.strength = 0;
  myCurrentProperties.attributes.dexterity = 0;
  myCurrentProperties.attributes.constitution = 0;
  myCurrentProperties.attributes.intelligence = 0;
  myCurrentProperties.attributes.wisdom = 0;
  myCurrentProperties.attributes.charisma = 0;
  myCurrentProperties.skills.clear();
  myCurrentProperties.feats.clear();
  myCurrentProperties.environment = ENVIRONMENT_INVALID;
  myCurrentProperties.organizations.clear();
  myCurrentProperties.challengeRating = 0;
  myCurrentProperties.treasureModifier = 0;
  myCurrentProperties.alignment.civicAlignment = ALIGNMENTCIVIC_INVALID;
  myCurrentProperties.alignment.ethicAlignment = ALIGNMENTETHIC_INVALID;
  myCurrentProperties.advancement.clear();
  myCurrentProperties.levelAdjustment = 0;
}

void RPG_Character_MonsterProperties_Type::name(const ::std::string& name_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::name"));

  myCurrentProperties.name = name_in;
}

void RPG_Character_MonsterProperties_Type::size(const RPG_Character_Size& size_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::size"));

  myCurrentProperties.size = size_in;
}

void RPG_Character_MonsterProperties_Type::type(const RPG_Character_MonsterType& type_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::type"));

  myCurrentProperties.type = type_in;
}

void RPG_Character_MonsterProperties_Type::hitDice(const RPG_Chance_Roll& hitDice_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::hitDice"));

  myCurrentProperties.hitDice = hitDice_in;
}

void RPG_Character_MonsterProperties_Type::initiative(long long initiative_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::initiative"));

  myCurrentProperties.initiative = initiative_in;
}

void RPG_Character_MonsterProperties_Type::speed(unsigned int speed_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::speed"));

  myCurrentProperties.speed = speed_in;
}

void RPG_Character_MonsterProperties_Type::armorClass(const RPG_Character_MonsterArmorClass& armorClass_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::armorClass"));

  myCurrentProperties.armorClass = armorClass_in;
}

void RPG_Character_MonsterProperties_Type::attack(const RPG_Character_MonsterAttack& attack_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::attack"));

  myCurrentProperties.attack = attack_in;
}

void RPG_Character_MonsterProperties_Type::space(unsigned int space_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::space"));

  myCurrentProperties.space = space_in;
}

void RPG_Character_MonsterProperties_Type::reach(unsigned int reach_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::reach"));

  myCurrentProperties.reach = reach_in;
}

void RPG_Character_MonsterProperties_Type::saves(const RPG_Character_SavingThrowModifiers& saves_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::saves"));

  myCurrentProperties.saves = saves_in;
}

void RPG_Character_MonsterProperties_Type::attributes(const RPG_Character_Attributes& attributes_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::attributes"));

  myCurrentProperties.attributes = attributes_in;
}

void RPG_Character_MonsterProperties_Type::skills(const RPG_Character_Skills_t& skills_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::skills"));

  myCurrentProperties.skills = skills_in;
}

void RPG_Character_MonsterProperties_Type::feats(const RPG_Character_Feats_t& feats_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::feats"));

  myCurrentProperties.feats = feats_in;
}

void RPG_Character_MonsterProperties_Type::environment(const RPG_Character_Environment& environment_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::environment"));

  myCurrentProperties.environment = environment_in;
}

void RPG_Character_MonsterProperties_Type::organization(const RPG_Character_Organization& organization_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::organization"));

  myCurrentProperties.organizations.insert(organization_in);
}

void RPG_Character_MonsterProperties_Type::challengeRating(unsigned int challengeRating_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::challengeRating"));

  myCurrentProperties.challengeRating = challengeRating_in;
}

void RPG_Character_MonsterProperties_Type::treasureModifier(unsigned int treasureModifier_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::treasureModifier"));

  myCurrentProperties.treasureModifier = treasureModifier_in;
}

void RPG_Character_MonsterProperties_Type::alignment(const RPG_Character_Alignment& alignment_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::alignment"));

  myCurrentProperties.alignment = alignment_in;
}

void RPG_Character_MonsterProperties_Type::advancement(const RPG_Character_MonsterAdvancement_t& advancement_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::advancement"));

  myCurrentProperties.advancement = advancement_in;
}

void RPG_Character_MonsterProperties_Type::levelAdjustment(unsigned int levelAdjustment_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::levelAdjustment"));

  myCurrentProperties.levelAdjustment = levelAdjustment_in;
}

RPG_Character_MonsterProperties_XML RPG_Character_MonsterProperties_Type::post_RPG_Character_MonsterProperties_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_MonsterProperties_Type::post_RPG_Character_MonsterProperties_Type"));

  RPG_Character_MonsterProperties_XML result = myCurrentProperties;

  // clear structure
  myCurrentProperties.name.resize(0);
  myCurrentProperties.size = SIZE_INVALID;
  myCurrentProperties.type.metaType = MONSTERMETATYPE_INVALID;
  myCurrentProperties.type.subTypes.clear();
  myCurrentProperties.hitDice.numDice = 0;
  myCurrentProperties.hitDice.typeDice = D_TYPE_INVALID;
  myCurrentProperties.hitDice.modifier = 0;
  myCurrentProperties.initiative = 0;
  myCurrentProperties.speed = 0;
  myCurrentProperties.armorClass.normal = 0;
  myCurrentProperties.armorClass.touch = 0;
  myCurrentProperties.armorClass.flatFooted = 0;
  myCurrentProperties.attack.baseAttackBonus = 0;
  myCurrentProperties.attack.grappleBonus = 0;
  myCurrentProperties.attack.attackActions.clear();
  myCurrentProperties.space = 0;
  myCurrentProperties.reach = 0;
  myCurrentProperties.saves.fortitude = 0;
  myCurrentProperties.saves.reflex = 0;
  myCurrentProperties.saves.will = 0;
  myCurrentProperties.attributes.strength = 0;
  myCurrentProperties.attributes.dexterity = 0;
  myCurrentProperties.attributes.constitution = 0;
  myCurrentProperties.attributes.intelligence = 0;
  myCurrentProperties.attributes.wisdom = 0;
  myCurrentProperties.attributes.charisma = 0;
  myCurrentProperties.skills.clear();
  myCurrentProperties.feats.clear();
  myCurrentProperties.environment = ENVIRONMENT_INVALID;
  myCurrentProperties.organizations.clear();
  myCurrentProperties.challengeRating = 0;
  myCurrentProperties.treasureModifier = 0;
  myCurrentProperties.alignment.civicAlignment = ALIGNMENTCIVIC_INVALID;
  myCurrentProperties.alignment.ethicAlignment = ALIGNMENTETHIC_INVALID;
  myCurrentProperties.advancement.clear();
  myCurrentProperties.levelAdjustment = 0;

  return result;
}
