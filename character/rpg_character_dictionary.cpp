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

#include "rpg_character_XML_parser.h"
#include "rpg_character_common_tools.h"
#include "rpg_character_skills_common_tools.h"
#include "rpg_character_monster_common_tools.h"

#include <rpg_chance_dice.h>
#include <rpg_chance_dice_common_tools.h>
#include <rpg_chance_dice_XML_parser.h>

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
  ::xml_schema::string_pimpl                string_p;
  RPG_Character_Size_Type                   size_p;

  RPG_Character_MonsterMetaType_Type        monsterMetaType_p;
  RPG_Character_MonsterSubType_Type         monsterSubType_p;
  RPG_Character_MonsterType_Type            monsterType_p;
  monsterType_p.parsers(monsterMetaType_p,
                        monsterSubType_p);

  ::xml_schema::unsigned_int_pimpl          unsigned_int_p;
  RPG_Chance_DiceType_Type                  chanceDiceType_p;
  ::xml_schema::integer_pimpl               int_p;
  RPG_Chance_DiceRoll_Type                  roll_p;
  roll_p.parsers(unsigned_int_p,
                 chanceDiceType_p,
                 int_p);
//   int_pimpl                               initiative_p;
//   unsigned_int_pimpl                      speed_p;
  ::xml_schema::unsigned_byte_pimpl         unsigned_byte_p;
  RPG_Character_MonsterArmorClass_Type      monsterArmorClass_p;
  monsterArmorClass_p.parsers(unsigned_byte_p,
                              unsigned_byte_p,
                              unsigned_byte_p);
  RPG_Character_MonsterWeapon_Type          monsterWeapon_p;
  ::xml_schema::byte_pimpl                  byte_p;
  RPG_Character_AttackForm_Type             attackForm_p;
  RPG_Character_MonsterAttackAction_Type    monsterAttackAction_p;
  monsterAttackAction_p.parsers(monsterWeapon_p,
                                byte_p,
                                attackForm_p,
                                roll_p,
                                unsigned_byte_p);
  RPG_Character_MonsterAttack_Type          monsterAttack_p;
  monsterAttack_p.parsers(byte_p,
                          byte_p,
                          monsterAttackAction_p);
//   unsigned_int_pimpl                      space_p;
//   unsigned_int_pimpl                      reach_p;
  RPG_Character_SavingThrowModifiers_Type   savingThrowModifiers_p;
  savingThrowModifiers_p.parsers(byte_p,
                                 byte_p,
                                 byte_p);
  RPG_Character_Attributes_Type             attributes_p;
  attributes_p.parsers(unsigned_byte_p,
                       unsigned_byte_p,
                       unsigned_byte_p,
                       unsigned_byte_p,
                       unsigned_byte_p,
                       unsigned_byte_p);
  RPG_Character_Skill_Type                  skill_p;
  RPG_Character_SkillValue_Type             skillvalue_p;
  skillvalue_p.parsers(skill_p,
                       byte_p);
  RPG_Character_Skills_Type                 skills_p;
  skills_p.parsers(skillvalue_p);
  RPG_Character_Feat_Type                   feat_p;
  RPG_Character_Feats_Type                  feats_p;
  feats_p.parsers(feat_p);
  RPG_Character_Environment_Type            environment_p;
  RPG_Character_Organization_Type           organization_p;
//   unsigned_int_pimpl                      challengeRating_p;
//   unsigned_int_pimpl                      treasureModifier_p;
  RPG_Character_AlignmentCivic_Type         alignmentCivic_p;
  RPG_Character_AlignmentEthic_Type         alignmentEthic_p;
  RPG_Character_Alignment_Type              alignment_p;
  alignment_p.parsers(alignmentCivic_p,
                      alignmentEthic_p);
  RPG_Chance_ValueRange_Type                range_p;
  range_p.parsers(int_p,
                  int_p);
  RPG_Character_MonsterAdvancementStep_Type advancementStep_p;
  advancementStep_p.parsers(size_p,
                            range_p);
  RPG_Character_MonsterAdvancement_Type     advancement_p;
  advancement_p.parsers(advancementStep_p);
//   unsigned_int_pimpl                      levelAdjustment_p;
  RPG_Character_MonsterProperties_Type      monsterProperties_p;
  monsterProperties_p.parsers(string_p,
                              size_p,
                              monsterType_p,
                              roll_p,
                              byte_p,
                              unsigned_byte_p,
                              monsterArmorClass_p,
                              monsterAttack_p,
                              unsigned_byte_p,
                              unsigned_byte_p,
                              savingThrowModifiers_p,
                              attributes_p,
                              skills_p,
                              feats_p,
                              environment_p,
                              organization_p,
                              unsigned_byte_p,
                              unsigned_byte_p,
                              alignment_p,
                              advancement_p,
                              unsigned_byte_p);
  RPG_Character_MonsterDictionary_Type    monsterDictionary_p(&myMonsterDictionary);
  monsterDictionary_p.parsers(monsterProperties_p);
  RPG_Character_Dictionary_Type           characterDictionary_p(&myMonsterDictionary);
  characterDictionary_p.parsers(monsterDictionary_p);

  // Parse the document to obtain the object model.
  //
  ::xml_schema::document doc_p(characterDictionary_p,
                               ACE_TEXT_ALWAYS_CHAR("urn:rpg"),
                               ACE_TEXT_ALWAYS_CHAR("characterDictionary"));

  characterDictionary_p.pre();

  // OK: parse the file...
  try
  {
    doc_p.parse(filename_in,
                myXSDErrorHandler);
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

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("finished parsing character dictionary file \"%s\"...\n"),
//              filename_in.c_str()));
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

void RPG_Character_Dictionary::generateRandomEncounter(const unsigned int& numDifferentMonsterTypes,
                                                       const RPG_Character_Environment& environment_in,
                                                       const RPG_Character_Organizations_t& organizations_in,
                                                       RPG_Character_Encounter_t& encounter_out) const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Dictionary::generateRandomEncounter"));

  // init result
  encounter_out.clear();

  RPG_Chance_DiceRollResult_t result;
  // step1: depending on the environment, find appropriate foes
  // *IMPORTANT NOTE*: if the input environment is ENVIRONMENT_ANY, we choose one at random
  RPG_Character_Environment choiceEnvironment = environment_in;
  if (environment_in == ENVIRONMENT_ANY)
  {
    do
    {
      result.clear();
      RPG_Chance_Dice::generateRandomNumbers(ENVIRONMENT_MAX,
                                             1,
                                             result);
      choiceEnvironment = ACE_static_cast(RPG_Character_Environment,
                                          (result.front() - 1));
    } while (choiceEnvironment == ENVIRONMENT_ANY);
  } // end IF

  RPG_Character_MonsterList_t list;
  for (RPG_Character_MonsterDictionaryIterator_t iterator = myMonsterDictionary.begin();
       iterator != myMonsterDictionary.end();
       iterator++)
  {
    // *TODO*: need to refine this for different planes...
    if ((iterator->second.environment == choiceEnvironment) ||
        (iterator->second.environment == ENVIRONMENT_ANY))
    {
      list.push_back(iterator->first);
    } // end IF
  }; // end FOR

  if (list.empty())
  {
    // nothing found in database...
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("found no appropriate monster types for environment \"%s\" in dictionary (%d items) --> check implementation !, returning\n"),
               RPG_Character_Monster_Common_Tools::environmentToString(choiceEnvironment).c_str(),
               myMonsterDictionary.size()));

    return;
  } // end IF

  for (int i = 0;
       i < numDifferentMonsterTypes;
       i++)
  {
    // step2a: choose random foe
    RPG_Character_EncounterIterator_t iterator;
    int choiceType = 0;
    do
    {
      result.clear();
      RPG_Chance_Dice::generateRandomNumbers(list.size(),
                                             1,
                                             result);
      choiceType = result.front() - 1; // list index
      // already used this type ?
      iterator = encounter_out.find(list[choiceType]);
    } while (iterator != encounter_out.end());

    // step2b: compute number of foes
    // step2ba: ...simply choose a random organization from the list
    result.clear();
    RPG_Chance_Dice::generateRandomNumbers(organizations_in.size(),
                                           1,
                                           result);
    int choiceOrganization = result.front() - 1; // list index
    RPG_Character_OrganizationsIterator_t iterator2 = organizations_in.begin();
    std::advance(iterator2, choiceOrganization);
    RPG_Chance_DiceRoll roll;
    organizationToRoll(*iterator2,
                       roll);
    result.clear();
    RPG_Chance_Dice::simulateDiceRoll(roll,
                                      1,
                                      result);

    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("generated encounter of %d %s...\n"),
               result.front(),
               list[choiceType].c_str()));

    encounter_out.insert(std::make_pair(list[choiceType], result.front()));
  } // end FOR
}

void RPG_Character_Dictionary::organizationToRoll(const RPG_Character_Organization& organization_in,
                                                  RPG_Chance_DiceRoll& roll_out) const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Dictionary::organizationToRoll"));

  // init result
  roll_out.numDice = 0;
  roll_out.typeDice = D_0;
  roll_out.modifier = 0;

  switch (organization_in)
  {
    case ORGANIZATION_ANY:
    {
      // choose any (other) form of organization at random...
      RPG_Character_Organization organization = ORGANIZATION_ANY;
      RPG_Chance_DiceRollResult_t result;
      do
      {
        result.clear();
        RPG_Chance_Dice::generateRandomNumbers(ORGANIZATION_MAX,
                                               1,
                                               result);
        organization = ACE_static_cast(RPG_Character_Organization,
                                       (result.front() - 1));
      } while (organization == ORGANIZATION_ANY);
      // ...and run ourselves again
      organizationToRoll(organization,
                         roll_out);

      break;
    }
    case ORGANIZATION_SOLITARY:
    {
      roll_out.modifier = 1;

      break;
    }
    case ORGANIZATION_PAIR:
    {
      roll_out.modifier = 2;

      break;
    }
    case ORGANIZATION_BROOD:   // 2-4 --> 1d3+1
    case ORGANIZATION_CLUSTER:
    case ORGANIZATION_CLUTCH:
    case ORGANIZATION_PATCH:
    case ORGANIZATION_GANG:
    {
      roll_out.numDice = 1;
      roll_out.typeDice = D_3;
      roll_out.modifier = 1;

      break;
    }
    case ORGANIZATION_TEAM: // 3-4 --> 1d2+2
    {
      roll_out.numDice = 1;
      roll_out.typeDice = D_2;
      roll_out.modifier = 2;

      break;
    }
    case ORGANIZATION_SQUAD: // 3-5, 11-20 + leaders
    {
      roll_out.numDice = 1;
      roll_out.typeDice = D_3;
      roll_out.modifier = 2;

      break;
    }
    case ORGANIZATION_PACK: // 3-6 --> 1d4+2
    case ORGANIZATION_COLONY:
    {
      roll_out.numDice = 1;
      roll_out.typeDice = D_4;
      roll_out.modifier = 2;

      break;
    }
    case ORGANIZATION_FLOCK: // 5-8 --> 1d4+4
    {
      roll_out.numDice = 1;
      roll_out.typeDice = D_4;
      roll_out.modifier = 4;

      break;
    }
    case ORGANIZATION_TRIBE: // 7-12 --> 1d6+6
    case ORGANIZATION_SLAVER: // 7-12 slaves
    {
      roll_out.numDice = 1;
      roll_out.typeDice = D_6;
      roll_out.modifier = 6;

      break;
    }
    case ORGANIZATION_BAND: // 11-20 + 2 sergeants + 1 leader + 150% noncombatants
    {
      roll_out.numDice = 1;
      roll_out.typeDice = D_10;
      roll_out.modifier = 10;

      break;
    }
    case ORGANIZATION_CLAN: // 30-100 + leaders + 50% noncombatants
    {
      roll_out.numDice = 7;
      roll_out.typeDice = D_10;
      roll_out.modifier = 30;

      break;
    }
    default:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("invalid organization \"%s\" --> check implementation !, returning\n"),
                 RPG_Character_Monster_Common_Tools::organizationToString(organization_in).c_str()));

      break;
    }
  } // end SWITCH
}

bool RPG_Character_Dictionary::XSD_Error_Handler::handle(const std::string& id_in,
                                                         unsigned long line_in,
                                                         unsigned long column_in,
                                                         ::xml_schema::error_handler::severity severity_in,
                                                         const std::string& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Dictionary::XSD_Error_Handler::handle"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("error occured (ID: \"%s\", location: %d, %d): \"%s\" --> check implementation !, continuing\n"),
             id_in.c_str(),
             line_in,
             column_in,
             message_in.c_str()));

  switch (severity_in)
  {
    case ::xml_schema::error_handler::severity::warning:
    {
      ACE_DEBUG((LM_WARNING,
                 ACE_TEXT("WARNING: error occured (ID: \"%s\", location: %d, %d): \"%s\" --> check implementation !, continuing\n"),
                 id_in.c_str(),
                 line_in,
                 column_in,
                 message_in.c_str()));

      break;
    }
    case ::xml_schema::error_handler::severity::error:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("ERROR: error occured (ID: \"%s\", location: %d, %d): \"%s\" --> check implementation !, continuing\n"),
                 id_in.c_str(),
                 line_in,
                 column_in,
                 message_in.c_str()));

      break;
    }
    case ::xml_schema::error_handler::severity::fatal:
    {
      ACE_DEBUG((LM_CRITICAL,
                 ACE_TEXT("FATAL: error occured (ID: \"%s\", location: %d, %d): \"%s\" --> check implementation !, continuing\n"),
                 id_in.c_str(),
                 line_in,
                 column_in,
                 message_in.c_str()));

      break;
    }
    default:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("unkown error occured (ID: \"%s\", location: %d, %d): \"%s\" --> check implementation !, continuing\n"),
                 id_in.c_str(),
                 line_in,
                 column_in,
                 message_in.c_str()));

      break;
    }
  } // end SWITCH

  // try to continue anyway...
  return true;
}

void RPG_Character_Dictionary::dump() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Dictionary::dump"));

  // simply dump the current content of our dictionary
  for (RPG_Character_MonsterDictionaryIterator_t iterator = myMonsterDictionary.begin();
       iterator != myMonsterDictionary.end();
       iterator++)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("Monster (\"%s\"):\nSize: \"%s\"\nType: \"%s\"\nHit Dice: \"%s\"\nInitiative: %d\nSpeed: %d ft/round\nArmor Class: %d / %d / %d\nAttack:\n-------\n%sSpace: %d ft\nReach: %d ft\nSaves: %d / %d / %d\nAttributes:\n-----------\n%sSkills:\n-------\n%sFeats:\n------\n%sEnvironment: \"%s\"\nOrganizations: \"%s\"\nChallenge Rating: %d\nTreasure Modifier: %d\nAlignment: \"%s\"\nAdvancement:\n------------\n%sLevel Adjustment: %d\n"),
               (iterator->first).c_str(),
               RPG_Character_Monster_Common_Tools::sizeToString((iterator->second).size).c_str(),
               RPG_Character_Monster_Common_Tools::monsterTypeToString((iterator->second).type).c_str(),
               RPG_Chance_Dice_Common_Tools::rollToString((iterator->second).hitDice).c_str(),
               (iterator->second).initiative,
               (iterator->second).speed,
               (iterator->second).armorClass.normal,
               (iterator->second).armorClass.touch,
               (iterator->second).armorClass.flatFooted,
               RPG_Character_Monster_Common_Tools::monsterAttackToString((iterator->second).attack).c_str(),
               (iterator->second).space,
               (iterator->second).reach,
               (iterator->second).saves.fortitude,
               (iterator->second).saves.reflex,
               (iterator->second).saves.will,
               RPG_Character_Common_Tools::attributesToString((iterator->second).attributes).c_str(),
               RPG_Character_Skills_Common_Tools::skillsToString((iterator->second).skills).c_str(),
               RPG_Character_Skills_Common_Tools::featsToString((iterator->second).feats).c_str(),
               RPG_Character_Monster_Common_Tools::environmentToString((iterator->second).environment).c_str(),
               RPG_Character_Monster_Common_Tools::organizationsToString((iterator->second).organizations).c_str(),
               (iterator->second).challengeRating,
               (iterator->second).treasureModifier,
               RPG_Character_Common_Tools::alignmentToString((iterator->second).alignment).c_str(),
               RPG_Character_Monster_Common_Tools::monsterAdvancementToString((iterator->second).advancement).c_str(),
               (iterator->second).levelAdjustment));
  } // end FOR
}
