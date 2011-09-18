
// -------------------------------- * * * -------------------------------- //
// PLEASE NOTE: this file was/is generated by XML2CppCode 0.1
// -------------------------------- * * * -------------------------------- //

#pragma once
#ifndef RPG_CHARACTER_MONSTER_ATTACK_H
#define RPG_CHARACTER_MONSTER_ATTACK_H

struct RPG_Character_Monster_Attack
{
  signed char baseAttackBonus;
  signed char grappleBonus;
  std::vector<RPG_Character_Monster_AttackAction> standardAttackActions;
  std::vector<RPG_Character_Monster_AttackAction> fullAttackActions;
  bool actionsAreInclusive;
};

#endif