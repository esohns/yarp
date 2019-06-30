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

#ifndef RPG_CHARACTER_XML_PARSER_H
#define RPG_CHARACTER_XML_PARSER_H

#include "ace/Global_Macros.h"

#include "rpg_XMLSchema_XML_types.h"

//#include "rpg_character_exports.h"
#include "rpg_character_XML_types.h"

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Character_Gender_Type
 : public RPG_Character_Gender_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Gender post_RPG_Character_Gender_Type();
};

class RPG_Character_Race_Type
 : public RPG_Character_Race_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Race post_RPG_Character_Race_Type();
};

class RPG_Character_MetaClass_Type
 : public RPG_Character_MetaClass_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_MetaClass post_RPG_Character_MetaClass_Type();
};

class RPG_Character_ClassXML_Type
  : public RPG_Character_ClassXML_Type_pskel
{
 public:
   RPG_Character_ClassXML_Type();

//   virtual void pre();
  virtual void metaClass(const RPG_Character_MetaClass&);
  virtual void subClass(const RPG_Common_SubClass&);
  virtual RPG_Character_ClassXML post_RPG_Character_ClassXML_Type();

 private:
   RPG_Character_ClassXML myCurrentClass;
};

class RPG_Character_Ability_Type
 : public RPG_Character_Ability_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Ability post_RPG_Character_Ability_Type();
};

// class RPG_Character_Abilities_Type
//   : public RPG_Character_Abilities_Type_pskel
// {
//   public:
//     RPG_Character_Abilities_Type();
//
// //   virtual void pre();
//     virtual void ability(const RPG_Character_Ability&);
//     virtual RPG_Character_Abilities post_RPG_Character_Abilities_Type();
//
//   private:
//     RPG_Character_Abilities myCurrentAbilities;
// };

class RPG_Character_Attributes_Type
 : public RPG_Character_Attributes_Type_pskel
{
 public:
  RPG_Character_Attributes_Type();

//   virtual void pre();
  virtual void strength(unsigned char);
  virtual void dexterity(unsigned char);
  virtual void constitution(unsigned char);
  virtual void intelligence(unsigned char);
  virtual void wisdom(unsigned char);
  virtual void charisma(unsigned char);
  virtual RPG_Character_Attributes post_RPG_Character_Attributes_Type();

 private:
  RPG_Character_Attributes myCurrentAttributes;
};

class RPG_Character_SkillValue_Type
 : public RPG_Character_SkillValue_Type_pskel
{
 public:
  RPG_Character_SkillValue_Type();

//   virtual void pre();
  virtual void skill(const RPG_Common_Skill&);
  virtual void rank(signed char);
  virtual RPG_Character_SkillValue post_RPG_Character_SkillValue_Type();

 private:
  RPG_Character_SkillValue myCurrentSkill;
};

class RPG_Character_Skills_Type
 : public RPG_Character_Skills_Type_pskel
{
 public:
  RPG_Character_Skills_Type();

//   virtual void pre();
  virtual void skill(const RPG_Character_SkillValue&);
  virtual RPG_Character_Skills post_RPG_Character_Skills_Type();

 private:
  RPG_Character_Skills myCurrentSkills;
};

class RPG_Character_CheckTypeUnion_Type
 : public RPG_Character_CheckTypeUnion_Type_pskel
{
 public:
  RPG_Character_CheckTypeUnion_Type();

//   virtual void pre();
  virtual void _characters(const ::xml_schema::ro_string&);
  virtual RPG_Character_CheckTypeUnion post_RPG_Character_CheckTypeUnion_Type();

 private:
  RPG_Character_CheckTypeUnion myCurrentCheckType;
};

class RPG_Character_Check_Type
 : public RPG_Character_Check_Type_pskel
{
 public:
  RPG_Character_Check_Type();

//   virtual void pre();
  virtual void type(const RPG_Character_CheckTypeUnion&);
  virtual void difficultyClass(unsigned char);
  virtual RPG_Character_Check post_RPG_Character_Check_Type();

 private:
  RPG_Character_Check myCurrentCheck;
};

class RPG_Character_Feat_Type
 : public RPG_Character_Feat_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Feat post_RPG_Character_Feat_Type();
};

class RPG_Character_Feats_Type
 : public RPG_Character_Feats_Type_pskel
{
 public:
  RPG_Character_Feats_Type();

//   virtual void pre();
  virtual void feat(const RPG_Character_Feat&);
  virtual RPG_Character_Feats post_RPG_Character_Feats_Type();

 private:
  RPG_Character_Feats myCurrentFeats;
};

class RPG_Character_AlignmentCivic_Type
 : public RPG_Character_AlignmentCivic_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_AlignmentCivic post_RPG_Character_AlignmentCivic_Type();
};

class RPG_Character_AlignmentEthic_Type
 : public RPG_Character_AlignmentEthic_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_AlignmentEthic post_RPG_Character_AlignmentEthic_Type();
};

class RPG_Character_Alignment_Type
 : public RPG_Character_Alignment_Type_pskel
{
 public:
  RPG_Character_Alignment_Type();

//   virtual void pre();
  virtual void civic(const RPG_Character_AlignmentCivic&);
  virtual void ethic(const RPG_Character_AlignmentEthic&);
  virtual RPG_Character_Alignment post_RPG_Character_Alignment_Type();

 private:
  RPG_Character_Alignment myCurrentAlignment;
};

class RPG_Character_EquipmentSlot_Type
 : public RPG_Character_EquipmentSlot_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_EquipmentSlot post_RPG_Character_EquipmentSlot_Type();
};

class RPG_Character_OffHand_Type
 : public RPG_Character_OffHand_Type_pskel,
   public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Character_OffHand post_RPG_Character_OffHand_Type();
};

class RPG_Character_Encumbrance_Type
 : public RPG_Character_Encumbrance_Type_pskel,
   public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Character_Encumbrance post_RPG_Character_Encumbrance_Type();
};

// class RPG_Character_BaseXML_Type
//  : public RPG_Character_BaseXML_Type_pskel
// {
//  public:
//   RPG_Character_BaseXML_Type();
//
// //   virtual void pre();
//   virtual void gender(const RPG_Character_Gender&);
//   virtual void alignment(const RPG_Character_Alignment&);
//   virtual void attributes(const RPG_Character_Attributes&);
//   virtual void skills(const RPG_Character_Skills&);
//   virtual void feats(const RPG_Character_Feats&);
//   virtual void abilities(const RPG_Character_Abilities&);
//   virtual void XP(unsigned int);
//   virtual void HP(long long);
//   virtual void maxHP(unsigned int);
//   virtual void gold(unsigned int);
//   virtual void condition(const RPG_Common_Condition&);
//   virtual RPG_Character_BaseXML post_RPG_Character_BaseXML_Type();
//
//  private:
//   RPG_Character_BaseXML myCurrentCharacterBaseXML;
// };

// class RPG_Character_PlayerXML_Type
//  : public RPG_Character_PlayerXML_Type_pskel
// {
//  public:
//   RPG_Character_PlayerXML_Type();
//
//   // virtual void pre();
//   virtual void gender(const RPG_Character_Gender&);
//   virtual void alignment(const RPG_Character_Alignment&);
//   virtual void attributes(const RPG_Character_Attributes&);
//   virtual void skills(const RPG_Character_Skills&);
//   virtual void feats(const RPG_Character_Feats&);
//   virtual void abilities(const RPG_Character_Abilities&);
//   virtual void XP(unsigned int);
//   virtual void HP(long long);
//   virtual void maxHP(unsigned int);
//   virtual void gold(unsigned int);
//   virtual void condition(const RPG_Common_Condition&);
//   // ------------------------------------------------------------
//   virtual void name(const ::std::string&);
//   virtual void race(const RPG_Character_Race&);
//   virtual void classXML(const RPG_Character_ClassXML&);
//   virtual void offhand(const RPG_Character_OffHand&);
//   virtual RPG_Character_PlayerXML post_RPG_Character_PlayerXML_Type();
//
//  private:
//   RPG_Character_PlayerXML myCurrentCharacterXML;
// };

#endif
