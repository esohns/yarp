// Not copyrighted - public domain.
//
// This sample parser implementation was generated by CodeSynthesis XSD,
// an XML Schema to C++ data binding compiler. You may use it in your
// programs without any restrictions.
//

#ifndef CXX___RPG_CHARACTER_DICTIONARY_PARSER_BASE_H
#define CXX___RPG_CHARACTER_DICTIONARY_PARSER_BASE_H

#include "rpg_character_dictionary_types.h"

class RPG_Character_Dictionary_Type_pimpl: public virtual RPG_Character_Dictionary_Type_pskel
{
  public:
  virtual void
  pre ();

  virtual void
  RPG_Character_MonsterDictionary ();

  virtual void
  post_RPG_Character_Dictionary_Type ();
};

class RPG_Character_MonsterDictionary_Type_pimpl: public virtual RPG_Character_MonsterDictionary_Type_pskel
{
  public:
  virtual void
  pre ();

  virtual void
  monster (const RPG_Character_MonsterProperties_XML&);

  virtual void
  post_RPG_Character_MonsterDictionary_Type ();
};

class RPG_Character_MonsterMetaType_Type_pimpl: public virtual RPG_Character_MonsterMetaType_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
  virtual void
  pre ();

  virtual RPG_Character_MonsterMetaType
  post_RPG_Character_MonsterMetaType_Type ();
};

class RPG_Character_MonsterSubType_Type_pimpl: public virtual RPG_Character_MonsterSubType_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
  virtual void
  pre ();

  virtual RPG_Character_MonsterSubType
  post_RPG_Character_MonsterSubType_Type ();
};

class RPG_Character_MonsterType_Type_pimpl: public virtual RPG_Character_MonsterType_Type_pskel
{
  public:
  virtual void
  pre ();

  virtual void
  metaType (const RPG_Character_MonsterMetaType&);

  virtual void
  subType (const RPG_Character_MonsterSubType&);

  virtual RPG_Character_MonsterType
  post_RPG_Character_MonsterType_Type ();
};

class RPG_Character_MonsterArmorClass_Type_pimpl: public virtual RPG_Character_MonsterArmorClass_Type_pskel
{
  public:
  virtual void
  pre ();

  virtual void
  normal (unsigned int);

  virtual void
  touch (unsigned int);

  virtual void
  flatFooted (unsigned int);

  virtual RPG_Character_MonsterArmorClass
  post_RPG_Character_MonsterArmorClass_Type ();
};

class RPG_Character_NaturalWeapon_Type_pimpl: public virtual RPG_Character_NaturalWeapon_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
  virtual void
  pre ();

  virtual RPG_Character_NaturalWeapon
  post_RPG_Character_NaturalWeapon_Type ();
};

class RPG_Character_MonsterAttackForm_Type_pimpl: public virtual RPG_Character_MonsterAttackForm_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
  virtual void
  pre ();

  virtual RPG_Character_MonsterAttackForm
  post_RPG_Character_MonsterAttackForm_Type ();
};

class RPG_Chance_DiceType_Type_pimpl: public virtual RPG_Chance_DiceType_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
  virtual void
  pre ();

  virtual RPG_Chance_DiceType
  post_RPG_Chance_DiceType_Type ();
};

class RPG_Chance_Roll_Type_pimpl: public virtual RPG_Chance_Roll_Type_pskel
{
  public:
  virtual void
  pre ();

  virtual void
  numDice (unsigned int);

  virtual void
  typeDice (const RPG_Chance_DiceType&);

  virtual void
  modifier (long long);

  virtual RPG_Chance_Roll
  post_RPG_Chance_Roll_Type ();
};

class RPG_Character_MonsterAttackAction_Type_pimpl: public virtual RPG_Character_MonsterAttackAction_Type_pskel
{
  public:
  virtual void
  pre ();

  virtual void
  naturalWeapon (const RPG_Character_NaturalWeapon&);

  virtual void
  attackBonus (unsigned int);

  virtual void
  attackForm (const RPG_Character_MonsterAttackForm&);

  virtual void
  damage (const RPG_Chance_Roll&);

  virtual void
  numAttacksPerRound (unsigned int);

  virtual RPG_Character_MonsterAttackAction
  post_RPG_Character_MonsterAttackAction_Type ();
};

class RPG_Character_MonsterAttack_Type_pimpl: public virtual RPG_Character_MonsterAttack_Type_pskel
{
  public:
  virtual void
  pre ();

  virtual void
  baseAttackBonus (unsigned int);

  virtual void
  grappleBonus (unsigned int);

  virtual void
  attackAction (const RPG_Character_MonsterAttackAction&);

  virtual RPG_Character_MonsterAttack
  post_RPG_Character_MonsterAttack_Type ();
};

class RPG_Character_Size_Type_pimpl: public virtual RPG_Character_Size_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
  virtual void
  pre ();

  virtual RPG_Character_Size
  post_RPG_Character_Size_Type ();
};

class RPG_Character_SavingThrowModifiers_Type_pimpl: public virtual RPG_Character_SavingThrowModifiers_Type_pskel
{
  public:
  virtual void
  pre ();

  virtual void
  fortitude (unsigned int);

  virtual void
  reflex (unsigned int);

  virtual void
  will (unsigned int);

  virtual RPG_Character_SavingThrowModifiers
  post_RPG_Character_SavingThrowModifiers_Type ();
};

class RPG_Character_Attributes_Type_pimpl: public virtual RPG_Character_Attributes_Type_pskel
{
  public:
  virtual void
  pre ();

  virtual void
  strength (unsigned int);

  virtual void
  dexterity (unsigned int);

  virtual void
  constitution (unsigned int);

  virtual void
  intelligence (unsigned int);

  virtual void
  wisdom (unsigned int);

  virtual void
  charisma (unsigned int);

  virtual RPG_Character_Attributes
  post_RPG_Character_Attributes_Type ();
};

class RPG_Character_Skill_Type_pimpl: public virtual RPG_Character_Skill_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
  virtual void
  pre ();

  virtual RPG_Character_Skill
  post_RPG_Character_Skill_Type ();
};

class RPG_Character_SkillValue_Type_pimpl: public virtual RPG_Character_SkillValue_Type_pskel
{
  public:
  virtual void
  pre ();

  virtual void
  skill (const RPG_Character_Skill&);

  virtual void
  rank (unsigned int);

  virtual RPG_Character_SkillsItem_t
  post_RPG_Character_SkillValue_Type ();
};

class RPG_Character_Skills_Type_pimpl: public virtual RPG_Character_Skills_Type_pskel
{
  public:
  virtual void
  pre ();

  virtual void
  skill (const RPG_Character_SkillsItem_t&);

  virtual RPG_Character_Skills_t
  post_RPG_Character_Skills_Type ();
};

class RPG_Character_Feat_Type_pimpl: public virtual RPG_Character_Feat_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
  virtual void
  pre ();

  virtual RPG_Character_Feat
  post_RPG_Character_Feat_Type ();
};

class RPG_Character_Feats_Type_pimpl: public virtual RPG_Character_Feats_Type_pskel
{
  public:
  virtual void
  pre ();

  virtual void
  feat (const RPG_Character_Feat&);

  virtual RPG_Character_Feats_t
  post_RPG_Character_Feats_Type ();
};

class RPG_Character_Environment_Type_pimpl: public virtual RPG_Character_Environment_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
  virtual void
  pre ();

  virtual RPG_Character_Environment
  post_RPG_Character_Environment_Type ();
};

class RPG_Character_Organization_Type_pimpl: public virtual RPG_Character_Organization_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
  virtual void
  pre ();

  virtual RPG_Character_Organization
  post_RPG_Character_Organization_Type ();
};

class RPG_Character_AlignmentCivic_Type_pimpl: public virtual RPG_Character_AlignmentCivic_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
  virtual void
  pre ();

  virtual RPG_Character_AlignmentCivic
  post_RPG_Character_AlignmentCivic_Type ();
};

class RPG_Character_AlignmentEthic_Type_pimpl: public virtual RPG_Character_AlignmentEthic_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
  virtual void
  pre ();

  virtual RPG_Character_AlignmentEthic
  post_RPG_Character_AlignmentEthic_Type ();
};

class RPG_Character_Alignment_Type_pimpl: public virtual RPG_Character_Alignment_Type_pskel
{
  public:
  virtual void
  pre ();

  virtual void
  civic (const RPG_Character_AlignmentCivic&);

  virtual void
  ethic (const RPG_Character_AlignmentEthic&);

  virtual RPG_Character_Alignment
  post_RPG_Character_Alignment_Type ();
};

class RPG_Character_MonsterAdvancementStep_Type_pimpl: public virtual RPG_Character_MonsterAdvancementStep_Type_pskel
{
  public:
  virtual void
  pre ();

  virtual void
  size (const RPG_Character_Size&);

  virtual void
  range (const RPG_Chance_Roll&);

  virtual RPG_Character_MonsterAdvancementStep_t
  post_RPG_Character_MonsterAdvancementStep_Type ();
};

class RPG_Character_MonsterAdvancement_Type_pimpl: public virtual RPG_Character_MonsterAdvancement_Type_pskel
{
  public:
  virtual void
  pre ();

  virtual void
  step (const RPG_Character_MonsterAdvancementStep_t&);

  virtual RPG_Character_MonsterAdvancement_t
  post_RPG_Character_MonsterAdvancement_Type ();
};

class RPG_Character_MonsterProperties_Type_pimpl: public virtual RPG_Character_MonsterProperties_Type_pskel
{
  public:
  virtual void
  pre ();

  virtual void
  name (const ::std::string&);

  virtual void
  size (const RPG_Character_Size&);

  virtual void
  type (const RPG_Character_MonsterType&);

  virtual void
  hitDice (const RPG_Chance_Roll&);

  virtual void
  initiative (long long);

  virtual void
  speed (unsigned int);

  virtual void
  armorClass (const RPG_Character_MonsterArmorClass&);

  virtual void
  attack (const RPG_Character_MonsterAttack&);

  virtual void
  space (unsigned int);

  virtual void
  reach (unsigned int);

  virtual void
  saves (const RPG_Character_SavingThrowModifiers&);

  virtual void
  attributes (const RPG_Character_Attributes&);

  virtual void
  skills (const RPG_Character_Skills_t&);

  virtual void
  feats (const RPG_Character_Feats_t&);

  virtual void
  environment (const RPG_Character_Environment&);

  virtual void
  organization (const RPG_Character_Organization&);

  virtual void
  challengeRating (unsigned int);

  virtual void
  treasureModifier (unsigned int);

  virtual void
  alignment (const RPG_Character_Alignment&);

  virtual void
  advancement (const RPG_Character_MonsterAdvancement_t&);

  virtual void
  levelAdjustment (unsigned int);

  virtual RPG_Character_MonsterProperties_XML
  post_RPG_Character_MonsterProperties_Type ();
};

#endif // CXX___RPG_CHARACTER_DICTIONARY_PARSER_BASE_H