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
#include "rpg_character_dictionary.h"

#include "rpg_character_dictionary_parser.h"

#include <ace/Log_Msg.h>

#include <iostream>

RPG_Character_Dictionary::RPG_Character_Dictionary()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Dictionary::RPG_Character_Dictionary"));

}

RPG_Character_Dictionary::~RPG_Character_Dictionary()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Dictionary::~RPG_Character_Dictionary"));

}

void RPG_Character_Dictionary::initCharacterDictionary(const std::string& filename_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Dictionary::initCharacterDictionary"));

  // Construct the parser.
  //
  ::xml_schema::string_pimpl              string_p;
  RPG_Character_Size_Type                 size_p;

  RPG_Character_MonsterMetaType_Type      monsterMetaType_p;
  RPG_Character_MonsterSubType_Type       monsterSubType_p;
  RPG_Character_MonsterType_Type          monsterType_p;
  monsterType_p.parsers(monsterMetaType_p,
                        monsterSubType_p);

  ::xml_schema::unsigned_int_pimpl        unsigned_int_p;
  RPG_Chance_DiceType_Type                chanceDiceType_p;
  ::xml_schema::integer_pimpl             int_p;
  RPG_Chance_Roll_Type                    roll_p;
  roll_p.parsers(unsigned_int_p,
                 chanceDiceType_p,
                 int_p);
//   int_pimpl                               initiative_p;
//   unsigned_int_pimpl                      speed_p;
  RPG_Character_MonsterArmorClass_Type    monsterArmorClass_p;
  monsterArmorClass_p.parsers(unsigned_int_p,
                              unsigned_int_p,
                              unsigned_int_p);
  RPG_Character_NaturalWeapon_Type        naturalWeapon_p;
  RPG_Character_MonsterAttackForm_Type    monsterAttackForm_p;
  RPG_Character_MonsterAttack_Type        monsterAttack_p;
  monsterAttack_p.parsers(unsigned_int_p,
                          unsigned_int_p,
                          naturalWeapon_p,
                          unsigned_int_p,
                          monsterAttackForm_p,
                          roll_p,
                          unsigned_int_p);
//   unsigned_int_pimpl                      space_p;
//   unsigned_int_pimpl                      reach_p;
  RPG_Character_SavingThrowModifiers_Type savingThrowModifiers_p;
  savingThrowModifiers_p.parsers(unsigned_int_p,
                                 unsigned_int_p,
                                 unsigned_int_p);
//   RPG_Character_Abilities_t          abilities;
//   RPG_Character_Skills_t             skills;
//   RPG_Character_Feats_t              feats;
  RPG_Character_Environment_Type          environment_p;
  RPG_Character_Organization_Type         organization_p;
//   unsigned_int_pimpl                      challengeRating_p;
//   unsigned_int_pimpl                      treasureModifier_p;
  RPG_Character_AlignmentCivic_Type       alignmentCivic_p;
  RPG_Character_AlignmentEthic_Type       alignmentEthic_p;
  RPG_Character_Alignment_Type            alignment_p;
  alignment_p.parsers(alignmentCivic_p,
                      alignmentEthic_p);
  RPG_Character_MonsterAdvancementStep_Type advancementStep_p;
  advancementStep_p.parsers(size_p,
                            roll_p);
  RPG_Character_MonsterAdvancement_Type   advancement_p;
  advancement_p.parsers(advancementStep_p);
//   unsigned_int_pimpl                      levelAdjustment_p;
  RPG_Character_MonsterProperties_Type    monsterProperties_p;
  monsterProperties_p.parsers(string_p,
                              size_p,
                              monsterType_p,
                              roll_p,
                              int_p,
                              unsigned_int_p,
                              monsterArmorClass_p,
                              monsterAttack_p,
                              unsigned_int_p,
                              unsigned_int_p,
                              savingThrowModifiers_p,
                              environment_p,
                              organization_p,
                              unsigned_int_p,
                              unsigned_int_p,
                              alignment_p,
                              advancement_p,
                              unsigned_int_p);
  RPG_Character_MonsterDictionary_Type    monsterDictionary_p(&myMonsterDictionary);
  monsterDictionary_p.parsers(monsterProperties_p);
  RPG_Character_Dictionary_Parser         characterDictionary_p(&myMonsterDictionary);
  characterDictionary_p.parsers(monsterDictionary_p);

  // Parse the document to obtain the object model.
  //
  ::xml_schema::document doc_p(characterDictionary_p,
                               ACE_TEXT_ALWAYS_CHAR("RPG_Character_Dictionary"));

  characterDictionary_p.pre();

  // OK: parse the file...
  try
  {
    doc_p.parse(filename_in);
  }
  catch(const ::xml_schema::parsing& exception)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Character_Dictionary::initCharacterDictionary(): exception occurred, returning\n")));

    std::cerr << exception << std::endl;

    return;
  }
  catch(...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Character_Dictionary::initCharacterDictionary(): exception occurred, returning\n")));

    return;
  }

  characterDictionary_p.post_RPG_Character_Dictionary_Type();

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished parsing character dictionary file \"%s\"...\n"),
             filename_in.c_str()));
}

const RPG_Character_MonsterProperties RPG_Character_Dictionary::getMonsterProperties(const std::string& monsterName_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Dictionary::getMonsterProperties"));

  RPG_Character_MonsterDictionaryIterator_t iterator = myMonsterDictionary.find(monsterName_in);
  if (iterator == myMonsterDictionary.end())
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("invalid monster name \"%s\" --> check implementation !, aborting\n"),
               monsterName_in.c_str()));

    ACE_ASSERT(false);
  } // end IF

  return iterator->second;
}
