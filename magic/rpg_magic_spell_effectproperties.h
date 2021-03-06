
// -------------------------------- * * * ----------------------------------- //
// PLEASE NOTE: this file was/is generated by // -------------------------------- * * * ----------------------------------- //

#pragma once
#ifndef RPG_MAGIC_SPELL_EFFECTPROPERTIES_H
#define RPG_MAGIC_SPELL_EFFECTPROPERTIES_H

struct RPG_Magic_Spell_EffectProperties
{
  RPG_Magic_Spell_Effect type;
  RPG_Magic_Spell_DamageTypeUnion damage;
  RPG_Common_Amount base;
  unsigned int increment;
  RPG_Common_Amount levelIncrement;
  unsigned char levelIncrementMax;
  RPG_Common_Attribute attribute;
  RPG_Magic_Domain domain;
  RPG_Common_CreatureType creature;
  RPG_Common_EffectDuration duration;
  std::vector<RPG_Magic_Spell_PreconditionProperties> preconditions;
  unsigned char maxRange;
  std::vector<RPG_Magic_CounterMeasure> counterMeasures;
  bool includeAdjacent;
  bool incrementIsReciprocal;
};

#endif
