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

#ifndef RPG_CHARACTER_ABILITY_H
#define RPG_CHARACTER_ABILITY_H

enum RPG_Character_Ability
{
  ABILITY_ACID_ATTACK = 0,
  ABILITY_SPELL,
  ABILITY_CORROSIVE_SLIME,
  ABILITY_ENTANGLE,
  ABILITY_LEAP_ATTACK,
  ABILITY_SCORE_LOSS,
  ABILITY_THROW_WEB,
  ABILITY_ALTERNATE_FORM,
  ABILITY_ANTIMAGIC,
  ABILITY_BLINDSIGHT,
  ABILITY_BLINDSENSE,
  ABILITY_BREATH_WEAPON,
  ABILITY_CHANGE_SHAPE,
  ABILITY_CHARM,
  ABILITY_COMPULSION,
  ABILITY_IMMUNITY_COLD,
  ABILITY_CONSTRICT,
  ABILITY_RESISTANCE_TO_DAMAGE,
  ABILITY_DARKVISION,
  ABILITY_DEATH_ATTACK,
  ABILITY_DISEASE,
  ABILITY_ENERGY_DRAIN,
  ABILITY_ETHEREALNESS,
  ABILITY_EVASION,
  ABILITY_IMPROVED_EVASION,
  ABILITY_FAST_HEALING,
  ABILITY_FEAR,
  ABILITY_IMMUNITY_FIRE,
  ABILITY_GASEOUS_FORM,
  ABILITY_GAZE_ATTACK,
  ABILITY_IMPROVED_GRAB,
  ABILITY_INCORPOREALITY,
  ABILITY_INVISIBILITY,
  ABILITY_LEVEL_LOSS,
  ABILITY_LOWLIGHT_VISION,
  ABILITY_MANUFACTURED_WEAPONS,
  ABILITY_MOVEMENT_MODES,
  ABILITY_NATURAL_WEAPONS,
  ABILITY_NONABILITIES,
  ABILITY_PARALYZE_ATTACK,
  ABILITY_POISON_ATTACK,
  ABILITY_IMMUNITY_POISON,
  ABILITY_POLYMORPH,
  ABILITY_POUNCE,
  ABILITY_POWERFUL_CHARGE,
  ABILITY_PSIONICS,
  ABILITY_RAKE,
  ABILITY_RAY_ATTACK,
  ABILITY_REGENERATION,
  ABILITY_RESISTANCE_TO_ENERGY,
  ABILITY_SCENT,
  ABILITY_SONIC_ATTACK,
  ABILITY_IMMUNITY_SPELL,
  ABILITY_RESISTANCE_TO_SPELL,
  ABILITY_SPELLS,
  ABILITY_SUMMON,
  ABILITY_SWALLOW_WHOLE,
  ABILITY_TELEPATHY,
  ABILITY_TRAMPLE,
  ABILITY_TREMOR_SENSE,
  ABILITY_RESISTANCE_TO_TURNING,
  ABILITY_VULNERABILITY_TO_ENERGY,
  ABILITY_IMMUNITY_DISEASE,
  ABILITY_ANIMAL_COMPANION,
  ABILITY_WOODLAND_STRIDE,
  ABILITY_SENSE_TRAPS,
  ABILITY_UNCANNY_DODGE,
  ABILITY_IMPROVED_UNCANNY_DODGE,
  ABILITY_CRIPPLING_STRIKE,
  ABILITY_DEFENSIVE_ROLL,
  ABILITY_OPPORTUNIST,
  ABILITY_SKILL_MASTERY,
  ABILITY_SLIPPERY_MIND,
  ABILITY_BONUS_FEAT,
  //
  RPG_CHARACTER_ABILITY_MAX,
  RPG_CHARACTER_ABILITY_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Character_Ability, std::string> RPG_Character_AbilityToStringTable_t;
typedef RPG_Character_AbilityToStringTable_t::const_iterator RPG_Character_AbilityToStringTableIterator_t;

class RPG_Character_AbilityHelper
{
 public:
  inline static void init()
  {
    myRPG_Character_AbilityToStringTable.clear();
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_ACID_ATTACK, ACE_TEXT_ALWAYS_CHAR("ABILITY_ACID_ATTACK")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_SPELL, ACE_TEXT_ALWAYS_CHAR("ABILITY_SPELL")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_CORROSIVE_SLIME, ACE_TEXT_ALWAYS_CHAR("ABILITY_CORROSIVE_SLIME")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_ENTANGLE, ACE_TEXT_ALWAYS_CHAR("ABILITY_ENTANGLE")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_LEAP_ATTACK, ACE_TEXT_ALWAYS_CHAR("ABILITY_LEAP_ATTACK")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_SCORE_LOSS, ACE_TEXT_ALWAYS_CHAR("ABILITY_SCORE_LOSS")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_THROW_WEB, ACE_TEXT_ALWAYS_CHAR("ABILITY_THROW_WEB")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_ALTERNATE_FORM, ACE_TEXT_ALWAYS_CHAR("ABILITY_ALTERNATE_FORM")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_ANTIMAGIC, ACE_TEXT_ALWAYS_CHAR("ABILITY_ANTIMAGIC")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_BLINDSIGHT, ACE_TEXT_ALWAYS_CHAR("ABILITY_BLINDSIGHT")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_BLINDSENSE, ACE_TEXT_ALWAYS_CHAR("ABILITY_BLINDSENSE")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_BREATH_WEAPON, ACE_TEXT_ALWAYS_CHAR("ABILITY_BREATH_WEAPON")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_CHANGE_SHAPE, ACE_TEXT_ALWAYS_CHAR("ABILITY_CHANGE_SHAPE")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_CHARM, ACE_TEXT_ALWAYS_CHAR("ABILITY_CHARM")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_COMPULSION, ACE_TEXT_ALWAYS_CHAR("ABILITY_COMPULSION")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_IMMUNITY_COLD, ACE_TEXT_ALWAYS_CHAR("ABILITY_IMMUNITY_COLD")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_CONSTRICT, ACE_TEXT_ALWAYS_CHAR("ABILITY_CONSTRICT")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_RESISTANCE_TO_DAMAGE, ACE_TEXT_ALWAYS_CHAR("ABILITY_RESISTANCE_TO_DAMAGE")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_DARKVISION, ACE_TEXT_ALWAYS_CHAR("ABILITY_DARKVISION")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_DEATH_ATTACK, ACE_TEXT_ALWAYS_CHAR("ABILITY_DEATH_ATTACK")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_DISEASE, ACE_TEXT_ALWAYS_CHAR("ABILITY_DISEASE")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_ENERGY_DRAIN, ACE_TEXT_ALWAYS_CHAR("ABILITY_ENERGY_DRAIN")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_ETHEREALNESS, ACE_TEXT_ALWAYS_CHAR("ABILITY_ETHEREALNESS")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_EVASION, ACE_TEXT_ALWAYS_CHAR("ABILITY_EVASION")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_IMPROVED_EVASION, ACE_TEXT_ALWAYS_CHAR("ABILITY_IMPROVED_EVASION")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_FAST_HEALING, ACE_TEXT_ALWAYS_CHAR("ABILITY_FAST_HEALING")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_FEAR, ACE_TEXT_ALWAYS_CHAR("ABILITY_FEAR")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_IMMUNITY_FIRE, ACE_TEXT_ALWAYS_CHAR("ABILITY_IMMUNITY_FIRE")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_GASEOUS_FORM, ACE_TEXT_ALWAYS_CHAR("ABILITY_GASEOUS_FORM")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_GAZE_ATTACK, ACE_TEXT_ALWAYS_CHAR("ABILITY_GAZE_ATTACK")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_IMPROVED_GRAB, ACE_TEXT_ALWAYS_CHAR("ABILITY_IMPROVED_GRAB")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_INCORPOREALITY, ACE_TEXT_ALWAYS_CHAR("ABILITY_INCORPOREALITY")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_INVISIBILITY, ACE_TEXT_ALWAYS_CHAR("ABILITY_INVISIBILITY")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_LEVEL_LOSS, ACE_TEXT_ALWAYS_CHAR("ABILITY_LEVEL_LOSS")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_LOWLIGHT_VISION, ACE_TEXT_ALWAYS_CHAR("ABILITY_LOWLIGHT_VISION")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_MANUFACTURED_WEAPONS, ACE_TEXT_ALWAYS_CHAR("ABILITY_MANUFACTURED_WEAPONS")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_MOVEMENT_MODES, ACE_TEXT_ALWAYS_CHAR("ABILITY_MOVEMENT_MODES")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_NATURAL_WEAPONS, ACE_TEXT_ALWAYS_CHAR("ABILITY_NATURAL_WEAPONS")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_NONABILITIES, ACE_TEXT_ALWAYS_CHAR("ABILITY_NONABILITIES")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_PARALYZE_ATTACK, ACE_TEXT_ALWAYS_CHAR("ABILITY_PARALYZE_ATTACK")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_POISON_ATTACK, ACE_TEXT_ALWAYS_CHAR("ABILITY_POISON_ATTACK")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_IMMUNITY_POISON, ACE_TEXT_ALWAYS_CHAR("ABILITY_IMMUNITY_POISON")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_POLYMORPH, ACE_TEXT_ALWAYS_CHAR("ABILITY_POLYMORPH")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_POUNCE, ACE_TEXT_ALWAYS_CHAR("ABILITY_POUNCE")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_POWERFUL_CHARGE, ACE_TEXT_ALWAYS_CHAR("ABILITY_POWERFUL_CHARGE")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_PSIONICS, ACE_TEXT_ALWAYS_CHAR("ABILITY_PSIONICS")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_RAKE, ACE_TEXT_ALWAYS_CHAR("ABILITY_RAKE")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_RAY_ATTACK, ACE_TEXT_ALWAYS_CHAR("ABILITY_RAY_ATTACK")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_REGENERATION, ACE_TEXT_ALWAYS_CHAR("ABILITY_REGENERATION")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_RESISTANCE_TO_ENERGY, ACE_TEXT_ALWAYS_CHAR("ABILITY_RESISTANCE_TO_ENERGY")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_SCENT, ACE_TEXT_ALWAYS_CHAR("ABILITY_SCENT")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_SONIC_ATTACK, ACE_TEXT_ALWAYS_CHAR("ABILITY_SONIC_ATTACK")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_IMMUNITY_SPELL, ACE_TEXT_ALWAYS_CHAR("ABILITY_IMMUNITY_SPELL")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_RESISTANCE_TO_SPELL, ACE_TEXT_ALWAYS_CHAR("ABILITY_RESISTANCE_TO_SPELL")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_SPELLS, ACE_TEXT_ALWAYS_CHAR("ABILITY_SPELLS")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_SUMMON, ACE_TEXT_ALWAYS_CHAR("ABILITY_SUMMON")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_SWALLOW_WHOLE, ACE_TEXT_ALWAYS_CHAR("ABILITY_SWALLOW_WHOLE")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_TELEPATHY, ACE_TEXT_ALWAYS_CHAR("ABILITY_TELEPATHY")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_TRAMPLE, ACE_TEXT_ALWAYS_CHAR("ABILITY_TRAMPLE")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_TREMOR_SENSE, ACE_TEXT_ALWAYS_CHAR("ABILITY_TREMOR_SENSE")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_RESISTANCE_TO_TURNING, ACE_TEXT_ALWAYS_CHAR("ABILITY_RESISTANCE_TO_TURNING")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_VULNERABILITY_TO_ENERGY, ACE_TEXT_ALWAYS_CHAR("ABILITY_VULNERABILITY_TO_ENERGY")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_IMMUNITY_DISEASE, ACE_TEXT_ALWAYS_CHAR("ABILITY_IMMUNITY_DISEASE")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_ANIMAL_COMPANION, ACE_TEXT_ALWAYS_CHAR("ABILITY_ANIMAL_COMPANION")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_WOODLAND_STRIDE, ACE_TEXT_ALWAYS_CHAR("ABILITY_WOODLAND_STRIDE")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_SENSE_TRAPS, ACE_TEXT_ALWAYS_CHAR("ABILITY_SENSE_TRAPS")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_UNCANNY_DODGE, ACE_TEXT_ALWAYS_CHAR("ABILITY_UNCANNY_DODGE")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_IMPROVED_UNCANNY_DODGE, ACE_TEXT_ALWAYS_CHAR("ABILITY_IMPROVED_UNCANNY_DODGE")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_CRIPPLING_STRIKE, ACE_TEXT_ALWAYS_CHAR("ABILITY_CRIPPLING_STRIKE")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_DEFENSIVE_ROLL, ACE_TEXT_ALWAYS_CHAR("ABILITY_DEFENSIVE_ROLL")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_OPPORTUNIST, ACE_TEXT_ALWAYS_CHAR("ABILITY_OPPORTUNIST")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_SKILL_MASTERY, ACE_TEXT_ALWAYS_CHAR("ABILITY_SKILL_MASTERY")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_SLIPPERY_MIND, ACE_TEXT_ALWAYS_CHAR("ABILITY_SLIPPERY_MIND")));
    myRPG_Character_AbilityToStringTable.insert(std::make_pair(ABILITY_BONUS_FEAT, ACE_TEXT_ALWAYS_CHAR("ABILITY_BONUS_FEAT")));
  };

  inline static std::string RPG_Character_AbilityToString(const RPG_Character_Ability& element_in)
  {
    std::string result;
    RPG_Character_AbilityToStringTableIterator_t iterator = myRPG_Character_AbilityToStringTable.find(element_in);
    if (iterator != myRPG_Character_AbilityToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_CHARACTER_ABILITY_INVALID");

    return result;
  };

  inline static RPG_Character_Ability stringToRPG_Character_Ability(const std::string& string_in)
  {
    RPG_Character_AbilityToStringTableIterator_t iterator = myRPG_Character_AbilityToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Character_AbilityToStringTable.end());

    return RPG_CHARACTER_ABILITY_INVALID;
  };

  static RPG_Character_AbilityToStringTable_t myRPG_Character_AbilityToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_AbilityHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_AbilityHelper(const RPG_Character_AbilityHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_AbilityHelper& operator=(const RPG_Character_AbilityHelper&));
};

#endif
