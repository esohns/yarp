
// -------------------------------- * * * ----------------------------------- //
// PLEASE NOTE: this file was/is generated by XML2CppCode 0.0.1-devel
// -------------------------------- * * * ----------------------------------- //

#ifndef RPG_MAGIC_SPELL_EFFECT_H
#define RPG_MAGIC_SPELL_EFFECT_H

enum RPG_Magic_Spell_Effect
{
  SPELLEFFECT_ABILITY_LOSS_MAGICAL_ALL = 0,
  SPELLEFFECT_ABILITY_LOSS_MAGICAL_EXTRAORDINARY,
  SPELLEFFECT_ABILITY_LOSS_MAGICAL_SPELLLIKE,
  SPELLEFFECT_ABILITY_LOSS_MAGICAL_SUPERNATURAL,
  SPELLEFFECT_ALIGN_WEAPON,
  SPELLEFFECT_ANIMATE_DEAD,
  SPELLEFFECT_ANIMATE_OBJECT,
  SPELLEFFECT_ATONEMENT,
  SPELLEFFECT_AUGURY,
  SPELLEFFECT_BANISH_CREATURE,
  SPELLEFFECT_BARRIER_CREATURE,
  SPELLEFFECT_BARRIER_MAGIC,
  SPELLEFFECT_BLESS_WATER,
  SPELLEFFECT_BLESS_WEAPON,
  SPELLEFFECT_BLIND,
  SPELLEFFECT_CALM_CREATURE,
  SPELLEFFECT_CHARM_CREATURE,
  SPELLEFFECT_COMMAND,
  SPELLEFFECT_COMPREHEND_LANGUAGES,
  SPELLEFFECT_CONFUSE,
  SPELLEFFECT_CONSUME,
  SPELLEFFECT_CONTACT_DIETY,
  SPELLEFFECT_CONTROL_CREATURE,
  SPELLEFFECT_CONTROL_WATER,
  SPELLEFFECT_CONTROL_WEATHER,
  SPELLEFFECT_CONTROL_WINDS,
  SPELLEFFECT_CONTINGENCY,
  SPELLEFFECT_CREATE_FOOD_AND_WATER,
  SPELLEFFECT_CREATE_WATER,
  SPELLEFFECT_CURSE_WATER,
  SPELLEFFECT_DAMAGE,
  SPELLEFFECT_DARKNESS,
  SPELLEFFECT_DARKVISION,
  SPELLEFFECT_DAZE,
  SPELLEFFECT_DEAFEN,
  SPELLEFFECT_DETECT_ANIMALS_PLANTS,
  SPELLEFFECT_DETECT_MAGIC,
  SPELLEFFECT_DETECT_POISON,
  SPELLEFFECT_DETECT_SCRYING,
  SPELLEFFECT_DETECT_SECRET_DOORS,
  SPELLEFFECT_DETECT_SNARES_AND_PITS,
  SPELLEFFECT_DETECT_THOUGHTS,
  SPELLEFFECT_DETECT_UNDEAD,
  SPELLEFFECT_DIMENSIONAL_ANCHOR,
  SPELLEFFECT_DIMINISH,
  SPELLEFFECT_DISCERN_LIES,
  SPELLEFFECT_DISGUISE,
  SPELLEFFECT_DISINTEGRATE,
  SPELLEFFECT_DISPEL_MAGIC,
  SPELLEFFECT_ENTANGLE,
  SPELLEFFECT_ETHEREAL,
  SPELLEFFECT_FASCINATE,
  SPELLEFFECT_FEAR,
  SPELLEFFECT_GROW,
  SPELLEFFECT_HEAL,
  SPELLEFFECT_HOLD_CREATURE,
  SPELLEFFECT_IDENTIFY,
  SPELLEFFECT_IDENTIFY_CONDITION,
  SPELLEFFECT_IMMUNITY_MAGIC_DEATH,
  SPELLEFFECT_IMMUNITY_MAGIC_MIND_AFFECTING,
  SPELLEFFECT_IMMUNITY_POISON,
  SPELLEFFECT_IMMUNITY_POSSESSION,
  SPELLEFFECT_INVISIBLE,
  SPELLEFFECT_KILL,
  SPELLEFFECT_KNOCKOUT,
  SPELLEFFECT_LEVITATE,
  SPELLEFFECT_LIGHT_MAGICAL,
  SPELLEFFECT_LOCATE,
  SPELLEFFECT_LOCK,
  SPELLEFFECT_MARK,
  SPELLEFFECT_MESSENGER,
  SPELLEFFECT_MISS,
  SPELLEFFECT_MODIFIER_ARMOR_CLASS,
  SPELLEFFECT_MODIFIER_ATTACK_ROLL,
  SPELLEFFECT_MODIFIER_ATTRIBUTE,
  SPELLEFFECT_MODIFIER_CHECK_ATTRIBUTE_ALL,
  SPELLEFFECT_MODIFIER_CHECK_ATTRIBUTE_CHARISMA,
  SPELLEFFECT_MODIFIER_CHECK_SKILL_ALL,
  SPELLEFFECT_MODIFIER_DAMAGE_ROLL,
  SPELLEFFECT_MODIFIER_HP,
  SPELLEFFECT_MODIFIER_LEVEL_CASTER,
  SPELLEFFECT_MODIFIER_MORALE,
  SPELLEFFECT_MODIFIER_SAVE_ALL,
  SPELLEFFECT_MODIFIER_SAVE_FEAR,
  SPELLEFFECT_MODIFIER_SAVE_REFLEX,
  SPELLEFFECT_MODIFIER_WEAPON_DAMAGE_ROLL,
  SPELLEFFECT_PARALYSIS,
  SPELLEFFECT_POLYMORPH,
  SPELLEFFECT_RAISE_DEAD,
  SPELLEFFECT_RESISTANCE_SPELL_LAWFUL,
  SPELLEFFECT_SENSOR_AUDIAL,
  SPELLEFFECT_SENSOR_VISUAL,
  SPELLEFFECT_SLEEP,
  SPELLEFFECT_SLOW,
  SPELLEFFECT_STUN,
  SPELLEFFECT_SUGGESTION,
  SPELLEFFECT_SUMMON_CREATURE,
  SPELLEFFECT_TELEPORT,
  SPELLEFFECT_TELEPORT_INTERPLANAR,
  SPELLEFFECT_TRIGGER_ALARM,
  //
  RPG_MAGIC_SPELL_EFFECT_MAX,
  RPG_MAGIC_SPELL_EFFECT_INVALID
};

#include "ace/Global_Macros.h"

#include <map>
#include <string>

typedef std::map<RPG_Magic_Spell_Effect, std::string> RPG_Magic_Spell_EffectToStringTable_t;
typedef RPG_Magic_Spell_EffectToStringTable_t::const_iterator RPG_Magic_Spell_EffectToStringTableIterator_t;

class RPG_Magic_Spell_EffectHelper
{
 public:
  inline static void init()
  {
    myRPG_Magic_Spell_EffectToStringTable.clear();
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_ABILITY_LOSS_MAGICAL_ALL, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_ABILITY_LOSS_MAGICAL_ALL")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_ABILITY_LOSS_MAGICAL_EXTRAORDINARY, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_ABILITY_LOSS_MAGICAL_EXTRAORDINARY")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_ABILITY_LOSS_MAGICAL_SPELLLIKE, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_ABILITY_LOSS_MAGICAL_SPELLLIKE")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_ABILITY_LOSS_MAGICAL_SUPERNATURAL, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_ABILITY_LOSS_MAGICAL_SUPERNATURAL")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_ALIGN_WEAPON, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_ALIGN_WEAPON")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_ANIMATE_DEAD, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_ANIMATE_DEAD")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_ANIMATE_OBJECT, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_ANIMATE_OBJECT")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_ATONEMENT, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_ATONEMENT")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_AUGURY, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_AUGURY")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_BANISH_CREATURE, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_BANISH_CREATURE")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_BARRIER_CREATURE, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_BARRIER_CREATURE")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_BARRIER_MAGIC, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_BARRIER_MAGIC")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_BLESS_WATER, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_BLESS_WATER")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_BLESS_WEAPON, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_BLESS_WEAPON")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_BLIND, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_BLIND")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_CALM_CREATURE, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_CALM_CREATURE")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_CHARM_CREATURE, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_CHARM_CREATURE")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_COMMAND, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_COMMAND")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_COMPREHEND_LANGUAGES, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_COMPREHEND_LANGUAGES")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_CONFUSE, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_CONFUSE")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_CONSUME, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_CONSUME")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_CONTACT_DIETY, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_CONTACT_DIETY")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_CONTROL_CREATURE, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_CONTROL_CREATURE")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_CONTROL_WATER, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_CONTROL_WATER")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_CONTROL_WEATHER, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_CONTROL_WEATHER")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_CONTROL_WINDS, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_CONTROL_WINDS")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_CONTINGENCY, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_CONTINGENCY")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_CREATE_FOOD_AND_WATER, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_CREATE_FOOD_AND_WATER")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_CREATE_WATER, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_CREATE_WATER")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_CURSE_WATER, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_CURSE_WATER")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_DAMAGE, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_DAMAGE")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_DARKNESS, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_DARKNESS")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_DARKVISION, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_DARKVISION")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_DAZE, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_DAZE")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_DEAFEN, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_DEAFEN")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_DETECT_ANIMALS_PLANTS, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_DETECT_ANIMALS_PLANTS")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_DETECT_MAGIC, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_DETECT_MAGIC")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_DETECT_POISON, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_DETECT_POISON")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_DETECT_SCRYING, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_DETECT_SCRYING")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_DETECT_SECRET_DOORS, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_DETECT_SECRET_DOORS")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_DETECT_SNARES_AND_PITS, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_DETECT_SNARES_AND_PITS")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_DETECT_THOUGHTS, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_DETECT_THOUGHTS")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_DETECT_UNDEAD, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_DETECT_UNDEAD")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_DIMENSIONAL_ANCHOR, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_DIMENSIONAL_ANCHOR")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_DIMINISH, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_DIMINISH")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_DISCERN_LIES, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_DISCERN_LIES")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_DISGUISE, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_DISGUISE")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_DISINTEGRATE, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_DISINTEGRATE")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_DISPEL_MAGIC, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_DISPEL_MAGIC")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_ENTANGLE, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_ENTANGLE")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_ETHEREAL, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_ETHEREAL")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_FASCINATE, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_FASCINATE")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_FEAR, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_FEAR")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_GROW, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_GROW")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_HEAL, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_HEAL")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_HOLD_CREATURE, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_HOLD_CREATURE")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_IDENTIFY, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_IDENTIFY")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_IDENTIFY_CONDITION, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_IDENTIFY_CONDITION")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_IMMUNITY_MAGIC_DEATH, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_IMMUNITY_MAGIC_DEATH")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_IMMUNITY_MAGIC_MIND_AFFECTING, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_IMMUNITY_MAGIC_MIND_AFFECTING")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_IMMUNITY_POISON, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_IMMUNITY_POISON")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_IMMUNITY_POSSESSION, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_IMMUNITY_POSSESSION")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_INVISIBLE, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_INVISIBLE")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_KILL, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_KILL")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_KNOCKOUT, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_KNOCKOUT")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_LEVITATE, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_LEVITATE")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_LIGHT_MAGICAL, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_LIGHT_MAGICAL")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_LOCATE, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_LOCATE")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_LOCK, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_LOCK")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_MARK, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_MARK")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_MESSENGER, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_MESSENGER")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_MISS, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_MISS")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_MODIFIER_ARMOR_CLASS, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_MODIFIER_ARMOR_CLASS")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_MODIFIER_ATTACK_ROLL, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_MODIFIER_ATTACK_ROLL")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_MODIFIER_ATTRIBUTE, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_MODIFIER_ATTRIBUTE")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_MODIFIER_CHECK_ATTRIBUTE_ALL, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_MODIFIER_CHECK_ATTRIBUTE_ALL")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_MODIFIER_CHECK_ATTRIBUTE_CHARISMA, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_MODIFIER_CHECK_ATTRIBUTE_CHARISMA")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_MODIFIER_CHECK_SKILL_ALL, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_MODIFIER_CHECK_SKILL_ALL")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_MODIFIER_DAMAGE_ROLL, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_MODIFIER_DAMAGE_ROLL")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_MODIFIER_HP, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_MODIFIER_HP")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_MODIFIER_LEVEL_CASTER, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_MODIFIER_LEVEL_CASTER")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_MODIFIER_MORALE, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_MODIFIER_MORALE")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_MODIFIER_SAVE_ALL, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_MODIFIER_SAVE_ALL")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_MODIFIER_SAVE_FEAR, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_MODIFIER_SAVE_FEAR")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_MODIFIER_SAVE_REFLEX, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_MODIFIER_SAVE_REFLEX")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_MODIFIER_WEAPON_DAMAGE_ROLL, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_MODIFIER_WEAPON_DAMAGE_ROLL")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_PARALYSIS, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_PARALYSIS")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_POLYMORPH, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_POLYMORPH")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_RAISE_DEAD, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_RAISE_DEAD")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_RESISTANCE_SPELL_LAWFUL, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_RESISTANCE_SPELL_LAWFUL")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_SENSOR_AUDIAL, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_SENSOR_AUDIAL")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_SENSOR_VISUAL, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_SENSOR_VISUAL")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_SLEEP, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_SLEEP")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_SLOW, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_SLOW")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_STUN, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_STUN")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_SUGGESTION, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_SUGGESTION")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_SUMMON_CREATURE, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_SUMMON_CREATURE")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_TELEPORT, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_TELEPORT")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_TELEPORT_INTERPLANAR, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_TELEPORT_INTERPLANAR")));
    myRPG_Magic_Spell_EffectToStringTable.insert(std::make_pair(SPELLEFFECT_TRIGGER_ALARM, ACE_TEXT_ALWAYS_CHAR("SPELLEFFECT_TRIGGER_ALARM")));
  };

  inline static std::string RPG_Magic_Spell_EffectToString(const RPG_Magic_Spell_Effect& element_in)
  {
    std::string result;
    RPG_Magic_Spell_EffectToStringTableIterator_t iterator = myRPG_Magic_Spell_EffectToStringTable.find(element_in);
    if (iterator != myRPG_Magic_Spell_EffectToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_MAGIC_SPELL_EFFECT_INVALID");

    return result;
  };

  inline static RPG_Magic_Spell_Effect stringToRPG_Magic_Spell_Effect(const std::string& string_in)
  {
    RPG_Magic_Spell_EffectToStringTableIterator_t iterator = myRPG_Magic_Spell_EffectToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Magic_Spell_EffectToStringTable.end());

    return RPG_MAGIC_SPELL_EFFECT_INVALID;
  };

  static RPG_Magic_Spell_EffectToStringTable_t myRPG_Magic_Spell_EffectToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_Spell_EffectHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_Spell_EffectHelper(const RPG_Magic_Spell_EffectHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_Spell_EffectHelper& operator=(const RPG_Magic_Spell_EffectHelper&));
};

#endif
