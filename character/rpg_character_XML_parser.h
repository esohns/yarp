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

#include "rpg_character_XML_parser_base.h"

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Character_Gender_Type
 : public RPG_Character_Gender_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Gender post_RPG_Character_Gender_Type();
};

class RPG_Character_Race_Type
 : public RPG_Character_Race_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Race post_RPG_Character_Race_Type();
};

class RPG_Character_MetaClass_Type
 : public RPG_Character_MetaClass_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_MetaClass post_RPG_Character_MetaClass_Type();
};

class RPG_Character_SubClass_Type
 : public RPG_Character_SubClass_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_SubClass post_RPG_Character_SubClass_Type();
};

class RPG_Character_Class_Type
 : public RPG_Character_Class_Type_pimpl
{
 public:
  RPG_Character_Class_Type();

//   virtual void pre();
  virtual void metaClass(const RPG_Character_MetaClass&);
  virtual void subClass(const RPG_Character_SubClass&);
  virtual RPG_Character_Class post_RPG_Character_Class_Type();

 private:
  RPG_Character_Class myCurrentClass;
};

class RPG_Character_Condition_Type
 : public RPG_Character_Condition_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Condition post_RPG_Character_Condition_Type();
};

class RPG_Character_Ability_Type
 : public RPG_Character_Ability_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Ability post_RPG_Character_Ability_Type();
};

class RPG_Character_Size_Type
 : public RPG_Character_Size_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Size post_RPG_Character_Size_Type();
};

class RPG_Character_Attributes_Type
 : public RPG_Character_Attributes_Type_pimpl
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

class RPG_Character_Skill_Type
 : public RPG_Character_Skill_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Skill post_RPG_Character_Skill_Type();
};

class RPG_Character_SkillValue_Type
 : public RPG_Character_SkillValue_Type_pimpl
{
 public:
  RPG_Character_SkillValue_Type();

//   virtual void pre();
  virtual void skill(const RPG_Character_Skill&);
  virtual void rank(signed char);
  virtual RPG_Character_SkillValue post_RPG_Character_SkillValue_Type();

 private:
  RPG_Character_SkillValue myCurrentSkill;
};

class RPG_Character_Skills_Type
 : public RPG_Character_Skills_Type_pimpl
{
 public:
  RPG_Character_Skills_Type();

//   virtual void pre();
  virtual void skill(const RPG_Character_SkillValue&);
  virtual RPG_Character_Skills post_RPG_Character_Skills_Type();

 private:
  RPG_Character_Skills myCurrentSkills;
};

class RPG_Character_Feat_Type
 : public RPG_Character_Feat_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Feat post_RPG_Character_Feat_Type();
};

class RPG_Character_Feats_Type
 : public RPG_Character_Feats_Type_pimpl
{
 public:
  RPG_Character_Feats_Type();

//   virtual void pre();
  virtual void feat(const RPG_Character_Feat&);
  virtual RPG_Character_Feats post_RPG_Character_Feats_Type();

 private:
  RPG_Character_Feats myCurrentFeats;
};

class RPG_Character_Terrain_Type
  : public RPG_Character_Terrain_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Terrain post_RPG_Character_Terrain_Type();
};

class RPG_Character_Climate_Type
  : public RPG_Character_Climate_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_Climate post_RPG_Character_Climate_Type();
};

class RPG_Character_Environment_Type
 : public RPG_Character_Environment_Type_pimpl
{
 public:
  RPG_Character_Environment_Type();

//   virtual void pre();
  virtual void terrain(const RPG_Character_Terrain&);
  virtual void climate(const RPG_Character_Climate&);
  virtual RPG_Character_Environment post_RPG_Character_Environment_Type();

 private:
  RPG_Character_Environment myCurrentEnvironment;
};

class RPG_Character_AlignmentCivic_Type
 : public RPG_Character_AlignmentCivic_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_AlignmentCivic post_RPG_Character_AlignmentCivic_Type();
};

class RPG_Character_AlignmentEthic_Type
 : public RPG_Character_AlignmentEthic_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_AlignmentEthic post_RPG_Character_AlignmentEthic_Type();
};

class RPG_Character_Alignment_Type
 : public RPG_Character_Alignment_Type_pimpl
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
 : public RPG_Character_EquipmentSlot_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Character_EquipmentSlot post_RPG_Character_EquipmentSlot_Type();
};

#endif
