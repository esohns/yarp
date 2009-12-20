// Not copyrighted - public domain.
//
// This sample parser implementation was generated by CodeSynthesis XSD,
// an XML Schema to C++ data binding compiler. You may use it in your
// programs without any restrictions.
//

#ifndef CXX___RPG_COMBAT_XML_PARSER_BASE_H
#define CXX___RPG_COMBAT_XML_PARSER_BASE_H

#include <rpg_combat_XML_types.h>

#include <rpg_dice_XML_parser_base.h>

#include <rpg_item_XML_parser_base.h>

class RPG_Combat_AttackForm_Type_pimpl: public virtual RPG_Combat_AttackForm_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
  virtual void
  pre ();

  virtual RPG_Combat_AttackForm
  post_RPG_Combat_AttackForm_Type ();
};

class RPG_Combat_AttackSituation_Type_pimpl: public virtual RPG_Combat_AttackSituation_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
  virtual void
  pre ();

  virtual RPG_Combat_AttackSituation
  post_RPG_Combat_AttackSituation_Type ();
};

class RPG_Combat_DefenseSituation_Type_pimpl: public virtual RPG_Combat_DefenseSituation_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
  virtual void
  pre ();

  virtual RPG_Combat_DefenseSituation
  post_RPG_Combat_DefenseSituation_Type ();
};

class RPG_Combat_SpecialAttack_Type_pimpl: public virtual RPG_Combat_SpecialAttack_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
  virtual void
  pre ();

  virtual RPG_Combat_SpecialAttack
  post_RPG_Combat_SpecialAttack_Type ();
};

class RPG_Combat_SpecialDamageType_Type_pimpl: public virtual RPG_Combat_SpecialDamageType_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
  virtual void
  pre ();

  virtual RPG_Combat_SpecialDamageType
  post_RPG_Combat_SpecialDamageType_Type ();
};

class RPG_Combat_DamageEffectType_Type_pimpl: public virtual RPG_Combat_DamageEffectType_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
  virtual void
  pre ();

  virtual RPG_Combat_DamageEffectType
  post_RPG_Combat_DamageEffectType_Type ();
};

class RPG_Combat_DamageTypeUnion_Type_pimpl: public virtual RPG_Combat_DamageTypeUnion_Type_pskel
{
  public:
  virtual void
  pre ();

  virtual void
  _characters (const ::xml_schema::ro_string&);

  virtual RPG_Combat_DamageTypeUnion
  post_RPG_Combat_DamageTypeUnion_Type ();
};

class RPG_Combat_DamageElement_Type_pimpl: public virtual RPG_Combat_DamageElement_Type_pskel
{
  public:
  virtual void
  pre ();

  virtual void
  type (const RPG_Combat_DamageTypeUnion&);

  virtual void
  damage (const RPG_Dice_Roll&);

  virtual void
  duration (unsigned short);

  virtual void
  effect (const RPG_Combat_DamageEffectType&);

  virtual RPG_Combat_DamageElement
  post_RPG_Combat_DamageElement_Type ();
};

class RPG_Combat_Damage_Type_pimpl: public virtual RPG_Combat_Damage_Type_pskel
{
  public:
  virtual void
  pre ();

  virtual void
  element (const RPG_Combat_DamageElement&);

  virtual RPG_Combat_Damage
  post_RPG_Combat_Damage_Type ();
};

#endif // CXX___RPG_COMBAT_XML_PARSER_BASE_H