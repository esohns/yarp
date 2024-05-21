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
#include "stdafx.h"

#include "rpg_player.h"

#include <fstream>
#include <numeric>

#include "ace/Log_Msg.h"

#include "common_defines.h"
#include "common_file_tools.h"

#include "common_error_tools.h"

#include "rpg_common_defines.h"
#include "rpg_common_macros.h"
#include "rpg_common_tools.h"
#include "rpg_common_xsderrorhandler.h"

#include "rpg_dice.h"
#include "rpg_dice_common.h"

#include "rpg_magic_common_tools.h"
#include "rpg_magic_defines.h"
#include "rpg_magic_dictionary.h"

#include "rpg_item_armor.h"
#include "rpg_item_base.h"
#include "rpg_item_common_tools.h"
#include "rpg_item_dictionary.h"
#include "rpg_item_instance_common.h"
#include "rpg_item_instance_manager.h"
#include "rpg_item_weapon.h"

#include "rpg_character_common_tools.h"
#include "rpg_character_defines.h"
#include "rpg_character_class_common_tools.h"
#include "rpg_character_race_common_tools.h"
#include "rpg_character_skills_common_tools.h"

#include "rpg_player_common_tools.h"
#include "rpg_player_defines.h"
#include "rpg_player_XML_tree.h"

RPG_Player::RPG_Player (// base attributes
                        const std::string& name_in,
                        enum RPG_Character_Gender gender_in,
                        const RPG_Character_Race_t& race_in,
                        const struct RPG_Character_Class& class_in,
                        const struct RPG_Character_Alignment& alignment_in,
                        const struct RPG_Character_Attributes& attributes_in,
                        const RPG_Character_Skills_t& skills_in,
                        const RPG_Character_Feats_t& feats_in,
                        const RPG_Character_Abilities_t& abilities_in,
                        enum RPG_Character_OffHand offhand_in,
                        ACE_UINT16 maxHitPoints_in,
                        const RPG_Magic_SpellTypes_t& knownSpells_in,
                        // extended data
                        ACE_UINT64 experience_in,
                        ACE_UINT64 wealth_in,
                        const RPG_Item_List_t& inventory_in,
                        // current status
                        const RPG_Character_Conditions_t& condition_in,
                        ACE_INT16 hitpoints_in,
                        const RPG_Magic_Spells_t& spells_in)
 : inherited (// base attributes
              name_in,
              gender_in,
              race_in,
              class_in,
              alignment_in,
              attributes_in,
              skills_in,
              feats_in,
              abilities_in,
              offhand_in,
              maxHitPoints_in,
              knownSpells_in,
              // current status
              condition_in,
              hitpoints_in,
              experience_in,
              wealth_in,
              spells_in,
              inventory_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Player::RPG_Player"));

  // wear default items...
  defaultEquip ();
//   myEquipment.dump();
}

RPG_Player::RPG_Player (const RPG_Player& player_in)
 : inherited (player_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Player::RPG_Player"));

}

struct RPG_Dice_Roll
RPG_Player::getHitDicePerLevel (enum RPG_Common_SubClass subClass_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Player::getHitDicePerLevel"));

  struct RPG_Dice_Roll result;

  // retrieve hit die/level for requested subclass
  result.typeDice = RPG_Character_Common_Tools::getHitDie (subClass_in);
  result.numDice = 1;
  result.modifier =
      RPG_Character_Common_Tools::getAttributeAbilityModifier (myAttributes.constitution);

  return result;
}

ACE_UINT8
RPG_Player::getFeatsPerLevel (enum RPG_Common_SubClass subClass_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Player::getFeatsPerLevel"));

  ACE_UINT8 result = RPG_CHARACTER_FEAT_LEVELUP_POINTS;

  ACE_UINT8 class_level_i = inherited::getLevel (subClass_in);

  if (class_level_i == 1)
  {
    if (myRace.test (RACE_HUMAN - 1))
      ++result; // at first level, humans receive an extra feat
    else if (myRace.test (RACE_ELF - 1))
      ++result; // at first level, elves receive Martial Weapon Proficiency (longsword,
                // rapier, longbow (including composite longbow), and shortbow (including
                // composite shortbow)
  } // end IF

  switch (subClass_in)
  {
    case SUBCLASS_FIGHTER:
    { // *NOTE*: these must be combat-oriented- "fighter bonus feats"
      if ((class_level_i == 1) ||     // at first level
          ((class_level_i % 2) == 0)) // on even levels
        ++result;
      break;
    }
    case SUBCLASS_RANGER:
    {
      if (class_level_i == 1) // at first level
        ++result; // Track
      else if (class_level_i == 2) // at second level
        ++result; // Rapid Shot || Two-Weapon Fighting
      else if (class_level_i == 3) // at third level
        ++result; // Endurance
      else if (class_level_i == 6) // at sixth level
        ++result; // Manyshot || Improved Two-Weapon Fighting
      else if (class_level_i == 11) // at eleventh level
        ++result; // Improved Precise Shot || Greater Two-Weapon Fighting
      break;
    }
    case SUBCLASS_CLERIC:
    {
      // *TODO*: "...A cleric who chooses the War domain receives the Weapon
      //         Focus feat related to his deity’s weapon as a bonus feat. He
      //         also receives the appropriate Martial Weapon Proficiency feat
      //         as a bonus feat, if the weapon falls into that category. ..."
      if (class_level_i == 1) // at first level
        result += 2;
      break;
    }
//    case SUBCLASS_MONK:
//    {
//      if (class_level_i == 1)
//        result += 2; // Improved Unarmed Strike && (Improved Grapple || Stunning Fist)
//      else if (class_level_i == 2)
//        ++result; // Combat Reflexes || Deflect Arrows
//      else if (class_level_i == 6)
//        ++result; // Improved Disarm || Improved Trip
//      break;
//    }
    case SUBCLASS_THIEF:
    {
      // *TODO*: only iff feat (instead of "special ability") is chosen
      if ((class_level_i == 10) || // at tenth level
          ((class_level_i > 10) &&
           ((class_level_i - 10) % 3) == 0)) // ...and every three levels thereafter
        ++result;
      break;
    }
    case SUBCLASS_WIZARD:
    {
      if (class_level_i == 1) // at first level
        ++result; // Scribe Scroll
      else if ((class_level_i % 5) == 0) // every five levels thereafter
        ++result; // metamagic-, item creation-, or Spell Mastery
      break;
    }
    case SUBCLASS_BARD:
    case SUBCLASS_DRUID:
    case SUBCLASS_PALADIN:
    case SUBCLASS_SORCERER:
//     case SUBCLASS_WARLOCK:
//     case SUBCLASS_AVENGER:
//     case SUBCLASS_INVOKER:
//     case SUBCLASS_SHAMAN:
//     case SUBCLASS_WARLORD:
    {
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                 ACE_TEXT ("invalid subclass: \"%s\", aborting\n"),
                 ACE_TEXT (RPG_Common_SubClassHelper::RPG_Common_SubClassToString (subClass_in).c_str ())));
      break;
    }
  } // end SWITCH

  return result;
}

ACE_UINT8
RPG_Player::getSkillsPerLevel (enum RPG_Common_SubClass subClass_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Player::getSkillsPerLevel"));

  ACE_UINT8 result = 0;

  switch (subClass_in)
  {
    case SUBCLASS_CLERIC:
    case SUBCLASS_FIGHTER:
    case SUBCLASS_PALADIN:
    case SUBCLASS_SORCERER:
    case SUBCLASS_WIZARD:
    {
      result = 2;
      break;
    }
//     case SUBCLASS_BARBARIAN:
//     {
//       result = 4;
//       result +=
//     }
//     case SUBCLASS_WARLORD:
//    {
//      break;
//    }
    case SUBCLASS_DRUID:
//     case SUBCLASS_MONK:
    {
      result = 4;
      break;
    }
    case SUBCLASS_BARD:
    case SUBCLASS_RANGER:
    {
      result = 6;
      break;
    }
    case SUBCLASS_THIEF:
    {
      result = 8;
      break;
    }
//     case SUBCLASS_AVENGER:
//     case SUBCLASS_INVOKER:
//     case SUBCLASS_SHAMAN:
//    {
//      break;
//    }
//     case SUBCLASS_WARLOCK:
//    {
//      break;
//    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                 ACE_TEXT ("invalid subclass: \"%s\", aborting\n"),
                 ACE_TEXT (RPG_Common_SubClassHelper::RPG_Common_SubClassToString (subClass_in).c_str ())));
      break;
    }
  } // end SWITCH
  result +=
      RPG_Character_Common_Tools::getAttributeAbilityModifier (myAttributes.intelligence);

  ACE_UINT8 class_level_i = inherited::getLevel (subClass_in);
  if (myRace.test (RACE_HUMAN - 1))
  {
   if (class_level_i == 1)
     result += 4; // at first level, humans receive four extra skill points
   else
     ++result; // humans receive an extra skill point every level
  } // end IF

  return result;
}

ACE_UINT16
RPG_Player::getKnownSpellsPerLevel (enum RPG_Common_SubClass subClass_in,
                                    ACE_UINT8 spellLevel_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Player::getKnownSpellsPerLevel"));

  // sanity check(s)
  if (!RPG_Magic_Common_Tools::isCasterClass (subClass_in))
    return 0;

  ACE_UINT16 result = 0;

  ACE_UINT8 class_level_i = inherited::getLevel (subClass_in);

  switch (subClass_in)
  {
    case SUBCLASS_BARD:
    case SUBCLASS_SORCERER:
    {
      result = RPG_Magic_Common_Tools::getNumKnownSpells (subClass_in,
                                                          class_level_i,
                                                          spellLevel_in);
      break;
    }
    case SUBCLASS_WIZARD:
    default:
      return std::numeric_limits<ACE_UINT16>::max (); // ALL
  } // end SWITCH

  return result;
}

RPG_Player*
RPG_Player::random ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Player::random"));

  // step1: name
  std::string name;
  // generate a string of (random ASCII alphabet, printable) characters
  int length = 0;
  RPG_Dice_RollResult_t result;
  RPG_Dice::generateRandomNumbers (10, // maximum length
                                   1,
                                   result);
  length = result.front ();
  result.clear ();
  RPG_Dice::generateRandomNumbers (26, // characters in (ASCII) alphabet
                                   (2 * length), // first half are characters, last half interpreted as boolean (upper/lower)
                                   result);
  bool lowercase = false;
  for (int i = 0;
       i < length;
       i++)
  {
    // upper/lower ?
    if (result[length + i] > 13)
      lowercase = false;
    else
      lowercase = true;

    name += static_cast<char> ((lowercase ? 96 : 64) + result[i]); // 97 == 'a', 65 == 'A'
  } // end FOR

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("generated name: \"%s\"\n"),
//              ACE_TEXT(name.c_str())));

  // step2: gender
  RPG_Character_Gender gender = RPG_CHARACTER_GENDER_INVALID;
  result.clear();
  RPG_Dice::generateRandomNumbers ((RPG_CHARACTER_GENDER_MAX - 2),
                                   1,
                                   result);
  gender = static_cast<RPG_Character_Gender> (result.front ());

  // step3: race
  RPG_Character_Race_t player_race (0);
  RPG_Character_Race race = RPG_CHARACTER_RACE_INVALID;
  result.clear ();
  RPG_Dice::generateRandomNumbers ((RPG_CHARACTER_RACE_MAX - 1),
                                   1,
                                   result);
  race = static_cast<RPG_Character_Race> (result.front ());
  player_race.flip (race - 1);
  if (RPG_Dice::probability (RPG_PLAYER_MULTIRACE_PROBABILITY))
  {
    RPG_Character_Race race_2 = RPG_CHARACTER_RACE_INVALID;
    do
    {
      result.clear ();
      RPG_Dice::generateRandomNumbers ((RPG_CHARACTER_RACE_MAX - 1),
                                       1,
                                       result);
      race_2 = static_cast<RPG_Character_Race> (result.front ());
    } while ((race == race_2) ||
             !RPG_Character_Race_Common_Tools::isCompatible (race,
                                                             race_2));
    player_race.flip (race_2 - 1);
  } // end IF

  // step4: class
  RPG_Character_Class player_class;
  player_class.metaClass = RPG_CHARACTER_METACLASS_INVALID;
  RPG_Common_SubClass player_subclass = RPG_COMMON_SUBCLASS_INVALID;
  result.clear ();
  RPG_Dice::generateRandomNumbers ((RPG_COMMON_SUBCLASS_MAX - 1),
                                   1,
                                   result);
  player_subclass = static_cast<RPG_Common_SubClass> (result.front ());
  player_class.metaClass =
      RPG_Character_Class_Common_Tools::subClassToMetaClass (player_subclass);
  player_class.subClasses.insert (player_subclass);
  if (RPG_Dice::probability (RPG_PLAYER_MULTICLASS_PROBABILITY))
  {
    // step1: determine number of subclasses (3 max)
    unsigned int num_subclasses = 1;
    result.clear ();
    RPG_Dice::generateRandomNumbers (2,
                                     1,
                                     result);
    num_subclasses = result.front ();

    // step2: determine subclass type(s)
    RPG_Common_SubClass player_subclass_temp = RPG_COMMON_SUBCLASS_INVALID;
    for (unsigned int i = 0;
         i < num_subclasses;
         i++)
    {
      player_subclass_temp = RPG_COMMON_SUBCLASS_INVALID;
      do
      {
        result.clear ();
        RPG_Dice::generateRandomNumbers ((RPG_COMMON_SUBCLASS_MAX - 1),
                                         1,
                                         result);
        player_subclass_temp = static_cast<RPG_Common_SubClass> (result.front ());
      } while (player_class.subClasses.find (player_subclass_temp) != player_class.subClasses.end ());
      player_class.subClasses.insert (player_subclass_temp);
    } // end FOR
  } // end IF

  // step5: alignment
  RPG_Character_Alignment alignment;
  alignment.civic = RPG_CHARACTER_ALIGNMENTCIVIC_INVALID;
  alignment.ethic = RPG_CHARACTER_ALIGNMENTETHIC_INVALID;
  result.clear ();
  RPG_Dice::generateRandomNumbers ((RPG_CHARACTER_ALIGNMENTCIVIC_MAX - 2),
                                   2,
                                   result);
  alignment.civic = static_cast<RPG_Character_AlignmentCivic> (result.front ());
  alignment.ethic = static_cast<RPG_Character_AlignmentEthic> (result.back ());

  // step6: attributes
  struct RPG_Character_Attributes attributes;
  ACE_OS::memset (&attributes, 0, sizeof (struct RPG_Character_Attributes));
  unsigned char* p = &(attributes.strength);
  struct RPG_Dice_Roll roll;
  roll.numDice = 2;
  roll.typeDice = D_10;
  roll.modifier = -2; // interval: 0-18
  // make sure the result is somewhat balanced (average == 6 * 9 ?)...
  // *NOTE*: INT must be > 2 (smaller values are reserved for animals...)
  int sum = 0;
  do
  {
    sum = 0;
    result.clear ();
    RPG_Dice::simulateRoll (roll,
                            6,
                            result);
    sum = std::accumulate (result.begin (),
                           result.end (),
                           0);
  } while ((sum <= RPG_PLAYER_ATTRIBUTE_MINIMUM_SUM) ||
           (*(std::min_element (result.begin (),
                                result.end ())) <= 9) ||
           (result[3] < 3)); // Note: this is already covered by the last case...
  for (int i = 0;
       i < 6;
       i++, p++)
  {
    // add +1 if result is 0 --> stats interval 1-18
    *p = (result[i] == 0 ? 1 : result[i]);
  } // end FOR

  // step7: (initial) skills
  RPG_Character_Skills_t skills;
  unsigned int initialSkillPoints = 0;
  RPG_Character_Skills_Common_Tools::getSkillPoints (player_subclass,
                                                     RPG_Character_Common_Tools::getAttributeAbilityModifier (attributes.intelligence),
                                                     initialSkillPoints);
  RPG_Character_SkillsIterator_t iterator;
  enum RPG_Common_Skill skill = RPG_COMMON_SKILL_INVALID;
  for (unsigned int i = 0;
       i < initialSkillPoints;
       i++)
  {
    result.clear ();
    RPG_Dice::generateRandomNumbers (RPG_COMMON_SKILL_MAX,
                                     1,
                                     result);
    skill = static_cast<RPG_Common_Skill> ((result.front () - 1));
    iterator = skills.find (skill);
    if (iterator != skills.end ())
      (iterator->second)++;
    else
      skills.insert (std::make_pair (skill,
                                     static_cast<ACE_INT8> (1)));
  } // end FOR

  // step8: (initial) feats & abilities
  RPG_Character_Feats_t feats;
  unsigned int initial_feats_i = 0;
  RPG_Character_Abilities_t abilities;
  RPG_Character_Skills_Common_Tools::getNumFeatsAbilities (race,
                                                           player_subclass,
                                                           1,
                                                           feats,
                                                           initial_feats_i,
                                                           abilities);
  RPG_Character_FeatsIterator_t iterator2;
  RPG_Character_Feat feat = RPG_CHARACTER_FEAT_INVALID;
  do
  {
    result.clear ();
    RPG_Dice::generateRandomNumbers (RPG_CHARACTER_FEAT_MAX,
                                     1,
                                     result);
    feat = static_cast<RPG_Character_Feat> ((result.front () - 1));

    // check prerequisites
    if (!RPG_Character_Skills_Common_Tools::meetsFeatPrerequisites (feat,
                                                                    player_subclass,
                                                                    1,
                                                                    attributes,
                                                                    skills,
                                                                    feats,
                                                                    abilities))
    {
      // try again
      continue;
    } // end IF

    iterator2 = feats.find (feat);
    if (iterator2 != feats.end ())
    {
      // try again
      continue;
    } // end IF

    feats.insert (feat);
  } while (feats.size () < initial_feats_i);

  // step9: off-hand
  enum RPG_Character_OffHand offHand = OFFHAND_LEFT;
  roll.numDice = 1;
  roll.typeDice = D_100;
  roll.modifier = 0;
  // *TODO*: 10% (?) of people are "lefties"...
  result.clear ();
  RPG_Dice::simulateRoll (roll,
                          1,
                          result);
  if (result.front () <= 10)
    offHand = OFFHAND_RIGHT;

  // step10: (initial) Hit Points
  ACE_INT16 hitpoints = 0;
//   roll.numDice = 1;
//   roll.typeDice = RPG_Character_Common_Tools::getHitDie(player_class.subClass);
//   roll.modifier = 0;
//   result.clear();
//   RPG_Dice::simulateRoll(roll,
//                          1,
//                          result);
//   hitpoints = result.front();
  // *NOTE*: players start with maxed HP...
  hitpoints = RPG_Character_Common_Tools::getHitDie (player_subclass);
  hitpoints +=
      RPG_Character_Common_Tools::getAttributeAbilityModifier (attributes.constitution);

  // step11: (initial) set of spells
  unsigned int numKnownSpells = 0;
  unsigned int numSpells = 0;
  RPG_Magic_SpellTypes_t knownSpells;
  RPG_Magic_Spells_t spells;
  unsigned int numChosen = 0;
  RPG_Magic_SpellTypes_t available;
  RPG_Magic_SpellTypesIterator_t available_iterator;
  struct RPG_Magic_CasterClassUnion casterClass;
  casterClass.discriminator = RPG_Magic_CasterClassUnion::SUBCLASS;
  for (RPG_Character_SubClassesIterator_t iterator = player_class.subClasses.begin ();
       iterator != player_class.subClasses.end ();
       iterator++)
  {
    if (!RPG_Magic_Common_Tools::isCasterClass (*iterator))
      continue;

    casterClass.subclass = *iterator;

    switch (*iterator)
    {
      case SUBCLASS_BARD:
      case SUBCLASS_SORCERER:
      {
        for (unsigned char i = 0;
             i <= RPG_COMMON_MAX_SPELL_LEVEL;
             i++)
        {
          numKnownSpells = 0;

          // step1: get list of available spells
          available =
              RPG_MAGIC_DICTIONARY_SINGLETON::instance ()->getSpells (casterClass,
                                                                      i);

          // step2: compute # known spells
          numKnownSpells = RPG_Magic_Common_Tools::getNumKnownSpells (*iterator,
                                                                      1,
                                                                      i);

          //         ACE_DEBUG((LM_DEBUG,
          //                    ACE_TEXT("number of initial known spells (lvl %d) for subClass \"%s\" is: %d...\n"),
          //                    i,
          //                    ACE_TEXT(RPG_Common_SubClassHelper::RPG_Common_SubClassToString(*iterator).c_str()),
          //                    numKnownSpells));

          // make sure we have enough variety...
          ACE_ASSERT (numKnownSpells <= available.size ());

          // step3: choose known spells
          if (numKnownSpells == 0)
            break; // done

          numChosen = 0;
          while (numChosen < numKnownSpells)
          {
            result.clear ();
            RPG_Dice::generateRandomNumbers (static_cast<unsigned int> (available.size ()),
                                             (numKnownSpells - numChosen),
                                             result);

            for (RPG_Dice_RollResultIterator_t iterator2 = result.begin ();
                 iterator2 != result.end ();
                 iterator2++)
            {
              available_iterator = available.begin ();
              std::advance (available_iterator, *iterator2 - 1);
              if (knownSpells.find (*available_iterator) != knownSpells.end ())
                continue; // try again

              ACE_DEBUG ((LM_DEBUG,
                          ACE_TEXT ("chose known spell #%d: \"%s\"\n"),
                          numChosen + 1,
                          ACE_TEXT (RPG_Magic_Common_Tools::spellToName (*available_iterator).c_str ())));

              knownSpells.insert (*available_iterator);
              numChosen++;
            } // end FOR
          } // end WHILE
        } // end FOR

        // *WARNING*: falls through !
      }
      case SUBCLASS_CLERIC:
      case SUBCLASS_DRUID:
      case SUBCLASS_PALADIN:
      case SUBCLASS_RANGER:
      {
        for (unsigned char i = 0;
             i <= RPG_COMMON_MAX_SPELL_LEVEL;
             i++)
        {
          numSpells = 0;

          // step1: get list of available spells
          available =
              RPG_MAGIC_DICTIONARY_SINGLETON::instance ()->getSpells (casterClass,
                                                                      i);

          // step2: compute # prepared spells
          numSpells = RPG_Magic_Common_Tools::getNumSpells (*iterator,
                                                            1,
                                                            i);

    //       ACE_DEBUG((LM_DEBUG,
    //                  ACE_TEXT("number of initial memorized/prepared spells (lvl %d) for subClass \"%s\" is: %d...\n"),
    //                  i,
    //                  ACE_TEXT(RPG_Common_SubClassHelper::RPG_Common_SubClassToString(*iterator).c_str()),
    //                  numSpells));

          // step3: choose prepared spells
          if (numSpells == 0)
            break; // done

          result.clear ();
          RPG_Dice::generateRandomNumbers ((RPG_Magic_Common_Tools::isDivineCasterClass (*iterator) ? static_cast<unsigned int> (available.size ())
                                                                                                    : static_cast<unsigned int> (knownSpells.size ())),
                                           numSpells,
                                           result);
          int index = 0;
          for (RPG_Dice_RollResultIterator_t iterator2 = result.begin ();
               iterator2 != result.end ();
               iterator2++, index++)
          {
            available_iterator =
                (RPG_Magic_Common_Tools::isDivineCasterClass (*iterator) ? available.begin ()
                                                                         : knownSpells.begin ());
            std::advance (available_iterator, *iterator2 - 1);

            ACE_DEBUG ((LM_DEBUG,
                        ACE_TEXT ("chose prepared spell #%d: \"%s\"\n"),
                        index + 1,
                        ACE_TEXT (RPG_Magic_Common_Tools::spellToName (*available_iterator).c_str ())));

            spells.push_back (*available_iterator);
          } // end FOR
        } // end FOR

        break;
      }
      case SUBCLASS_WIZARD:
      {
        numKnownSpells = RPG_MAGIC_DEF_NUM_NEW_SPELLS_PER_LEVEL;

        // step1: collect list of available spells
        RPG_Magic_SpellTypes_t current;
        char i = 0;
        for (;
             i <= RPG_COMMON_MAX_SPELL_LEVEL;
             i++)
        {
          if (RPG_Magic_Common_Tools::getNumSpells (*iterator,
                                                    1,
                                                    i) == 0)
          {
            i--;
            break; // done
          } // end FOR

          current =
              RPG_MAGIC_DICTIONARY_SINGLETON::instance ()->getSpells (casterClass,
                                                                      i);
          available.insert (current.begin (), current.end ());
        } // end FOR

        // step2: chose # known spells
        numChosen = 0;
        while (numChosen < numKnownSpells)
        {
          result.clear ();
          RPG_Dice::generateRandomNumbers (static_cast<unsigned int> (available.size ()),
                                           (numKnownSpells - numChosen),
                                           result);

          for (RPG_Dice_RollResultIterator_t iterator2 = result.begin ();
               iterator2 != result.end ();
               iterator2++)
          {
            available_iterator = available.begin ();
            std::advance (available_iterator, *iterator2 - 1);
            if (knownSpells.find (*available_iterator) != knownSpells.end ())
              continue; // try again

            ACE_DEBUG ((LM_DEBUG,
                        ACE_TEXT ("chose known spell #%d: \"%s\"\n"),
                        numChosen + 1,
                        ACE_TEXT (RPG_Magic_Common_Tools::spellToName (*available_iterator).c_str ())));

            knownSpells.insert (*available_iterator);
            numChosen++;
          } // end FOR
        } // end WHILE

        // step3: chose # prepared spells
        for (char j = i;
             j >= 0;
             j--)
        {
          numSpells += RPG_Magic_Common_Tools::getNumSpells (*iterator,
                                                             1,
                                                             j);
          ACE_ASSERT (numSpells);

          current =
              RPG_MAGIC_DICTIONARY_SINGLETON::instance ()->getSpells (casterClass,
                                                                      j);
          available.clear ();
          for (RPG_Magic_SpellTypesIterator_t iterator2 = current.begin ();
               iterator2 != current.end ();
               iterator2++)
            if (knownSpells.find (*iterator2) != knownSpells.end ())
              available.insert (*iterator2);
//           std::set_intersection(current.begin(),
//                                 current.end(),
//                                 knownSpells.begin(),
//                                 knownSpells.end(),
//                                 available.begin());
          if (available.empty ())
            continue; // done

          result.clear ();
          RPG_Dice::generateRandomNumbers (static_cast<unsigned int> (available.size ()),
                                           numSpells,
                                           result);
          int index = 0;
          for (RPG_Dice_RollResultIterator_t iterator2 = result.begin ();
               iterator2 != result.end ();
               iterator2++, index++)
          {
            available_iterator = available.begin ();
            std::advance (available_iterator, *iterator2 - 1);

            ACE_DEBUG ((LM_DEBUG,
                        ACE_TEXT ("chose prepared spell #%d: \"%s\"\n"),
                        index + 1,
                        ACE_TEXT (RPG_Magic_Common_Tools::spellToName (*available_iterator).c_str ())));

            spells.push_back (*available_iterator);
          } // end FOR

          numSpells = 0;
        } // end FOR

        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid class (was: %s), aborting\n"),
                    ACE_TEXT (RPG_Common_SubClassHelper::RPG_Common_SubClassToString (*iterator).c_str ())));
        break;
      }
    } // end SWITCH
  } // end FOR

  // step12: initialize condition
  RPG_Character_Conditions_t condition;
  condition.insert (CONDITION_NORMAL); // start "normal"

  // step13: (initial) set of items
  // *TODO*: somehow generate these at random as well ?
  RPG_Item_List_t items;
  RPG_Item_Instance_Base* current = NULL;
  try
  {
    switch (player_subclass)
    {
      case SUBCLASS_FIGHTER:
      {
        current =
            RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->create (ITEM_WEAPON,
                                                                      ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
        ACE_ASSERT (current);
        items.insert (current->id ());

        current =
            RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->create (ITEM_ARMOR,
                                                                      ARMOR_MAIL_SPLINT);
        ACE_ASSERT (current);
        items.insert (current->id ());

        current =
            RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->create (ITEM_ARMOR,
                                                                      ARMOR_SHIELD_HEAVY_WOODEN);
        ACE_ASSERT (current);
        items.insert (current->id ());

        break;
      }
      case SUBCLASS_PALADIN:
//       case SUBCLASS_WARLORD:
      {
        current =
            RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->create (ITEM_WEAPON,
                                                                      ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
        ACE_ASSERT (current);
        items.insert (current->id ());

        current =
            RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->create (ITEM_ARMOR,
                                                                      ARMOR_PLATE_FULL);
        ACE_ASSERT (current);
        items.insert (current->id ());

        current =
            RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->create (ITEM_ARMOR,
                                                                      ARMOR_SHIELD_HEAVY_STEEL);
        ACE_ASSERT (current);
        items.insert (current->id ());

        break;
      }
      case SUBCLASS_RANGER:
      {
        current =
            RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->create (ITEM_WEAPON,
                                                                      ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
        ACE_ASSERT (current);
        items.insert (current->id ());

        current =
            RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->create (ITEM_WEAPON,
                                                                      RANGED_WEAPON_BOW_LONG);
        ACE_ASSERT (current);
        items.insert (current->id ());

        current =
            RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->create (ITEM_ARMOR,
                                                                      ARMOR_HIDE);
        ACE_ASSERT (current);
        items.insert (current->id ());

        // *TODO*: no arrows ?

        break;
      }
//       case SUBCLASS_BARBARIAN:
//       {
//         current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
//             ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
//         ACE_ASSERT(current);
//         items.insert(current->id());
//
//         current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
//             ARMOR_HIDE);
//         ACE_ASSERT(current);
//         items.insert(current->id());
//
//         break;
//       }
      case SUBCLASS_WIZARD:
      case SUBCLASS_SORCERER:
//       case SUBCLASS_WARLOCK:
      {
        current =
            RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->create (ITEM_WEAPON,
                                                                      TWO_HANDED_MELEE_WEAPON_QUARTERSTAFF);
        ACE_ASSERT (current);
        items.insert (current->id ());

        break;
      }
      case SUBCLASS_CLERIC:
//       case SUBCLASS_AVENGER:
//       case SUBCLASS_INVOKER:
      {
        current =
            RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create (ITEM_WEAPON,
                                                                     ONE_HANDED_MELEE_WEAPON_MACE_HEAVY);
        ACE_ASSERT (current);
        items.insert (current->id ());

        current =
            RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->create (ITEM_ARMOR,
                                                                      ARMOR_MAIL_CHAIN);
        ACE_ASSERT (current);
        items.insert (current->id ());

        current =
            RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->create (ITEM_ARMOR,
                                                                      ARMOR_SHIELD_HEAVY_WOODEN);
        ACE_ASSERT (current);
        items.insert (current->id ());

        break;
      }
      case SUBCLASS_DRUID:
//       case SUBCLASS_SHAMAN:
      {
        current =
            RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->create (ITEM_WEAPON,
                                                                      LIGHT_MELEE_WEAPON_SICKLE);
        ACE_ASSERT (current);
        items.insert (current->id ());

        current =
            RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->create (ITEM_ARMOR,
                                                                      ARMOR_HIDE);
        ACE_ASSERT (current);
        items.insert (current->id ());

        current =
            RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->create (ITEM_ARMOR,
                                                                      ARMOR_SHIELD_LIGHT_WOODEN);
        ACE_ASSERT (current);
        items.insert (current->id ());

        break;
      }
//       case SUBCLASS_MONK:
//       {
//         current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
//             TWO_HANDED_MELEE_WEAPON_QUARTERSTAFF);
//         ACE_ASSERT(current);
//         items.insert(current->id());
//
//         break;
//       }
      case SUBCLASS_THIEF:
      case SUBCLASS_BARD:
      {
        current =
            RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->create (ITEM_WEAPON,
                                                                      LIGHT_MELEE_WEAPON_SWORD_SHORT);
        ACE_ASSERT (current);
        items.insert (current->id ());

        current =
            RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->create (ITEM_ARMOR,
                                                                      ARMOR_LEATHER);
        ACE_ASSERT (current);
        items.insert (current->id ());

        current =
            RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->create (ITEM_ARMOR,
                                                                      ARMOR_SHIELD_LIGHT_STEEL);
        ACE_ASSERT (current);
        items.insert (current->id ());

        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid subclass \"%s\", aborting\n"),
                    ACE_TEXT (RPG_Common_SubClassHelper::RPG_Common_SubClassToString (player_subclass).c_str ())));
        break;
      }
    } // end SWITCH

    // ...and everybody gets a torch
    unsigned int subtype =
        (0 | (COMMODITY_LIGHT << (sizeof (unsigned int) * 4)));
    subtype |= COMMODITY_LIGHT_TORCH;
    current =
        RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->create (ITEM_COMMODITY,
                                                                  subtype);
    ACE_ASSERT (current);
    items.insert (current->id ());
  }
  catch (const std::bad_alloc& exception)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("RPG_Player::create(): caught exception: \"%s\", continuing\n"),
                ACE_TEXT (exception.what ())));
  }

  // step14: instantiate player character
  RPG_Player* player_p = NULL;
  ACE_NEW_NORETURN (player_p,
                    RPG_Player (// base attributes
                                name,
                                gender,
                                player_race,
                                player_class,
                                alignment,
                                attributes,
                                skills,
                                feats,
                                abilities,
                                offHand,
                                hitpoints,
                                knownSpells,
                                // extended data
                                0,
                                RPG_PLAYER_START_MONEY,
                                items,
                                // current status
                                condition,
                                hitpoints, // start healthy
                                spells));
  if (!player_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%u), aborting\n"),
                sizeof (RPG_Player)));
    return NULL;
  } // end IF

  return player_p;
}

RPG_Player*
RPG_Player::load (const std::string& filename_in,
                  const std::string& schemaRepository_in,
                  // current status
                  const RPG_Character_Conditions_t& condition_in,
                  ACE_INT16 HP_in,
                  const RPG_Magic_Spells_t& spells_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Player::load"));

  // sanity check(s)
  if (!Common_File_Tools::isReadable (filename_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Common_File_Tools::isReadable(\"%s\"), aborting\n"),
                ACE_TEXT (filename_in.c_str ())));
    return NULL;
  } // end IF

  std::ifstream ifs;
  ifs.exceptions (std::ifstream::badbit | std::ifstream::failbit);
  try {
    ifs.open (filename_in.c_str(),
              std::ios_base::in);
  } catch (std::ios_base::failure exception) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to std::ifstream::open(\"%s\"): caught exception: \"%s\", aborting\n"),
                ACE_TEXT (filename_in.c_str ()),
                ACE_TEXT (exception.what ())));
    return NULL;
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to std::ifstream::open(\"%s\"): caught exception, aborting\n"),
                ACE_TEXT (filename_in.c_str ())));
    return NULL;
  }

  //   ::xml_schema::flags = ::xml_schema::flags::dont_validate;
  ::xml_schema::flags flags = 0;
  ::xml_schema::properties props;
  std::string path;
  ACE_ASSERT (!schemaRepository_in.empty ());
  //if (Common_Error_Tools::inDebugSession ())
  //{
  //  path = schemaRepository_in;
  //  path += ACE_DIRECTORY_SEPARATOR_STR;
  //  path += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_SUB_DIRECTORY_STRING);
  //  path += ACE_DIRECTORY_SEPARATOR_STR;
  //  path += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY);
  //} // end IF
  //else
    path = schemaRepository_in;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_SCHEMA_FILE);
  // sanity check(s)
  if (!Common_File_Tools::isReadable (path))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::isReadable(\"%s\"), aborting\n"),
                ACE_TEXT (path.c_str ())));
    try {
      ifs.close ();
    } catch (std::ios_base::failure exception) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ifstream::close(\"%s\"): caught exception: \"%s\", aborting\n"),
                  ACE_TEXT (filename_in.c_str ()),
                  ACE_TEXT (exception.what ())));
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ifstream::close(\"%s\"): caught exception, aborting\n"),
                  ACE_TEXT (filename_in.c_str ())));
    }
    return NULL;
  } // end IF
  // *NOTE*: support paths with spaces
  path = RPG_Common_Tools::sanitizeURI (path);
  path.insert (0, ACE_TEXT_ALWAYS_CHAR ("file:///"));
  props.schema_location (ACE_TEXT_ALWAYS_CHAR (RPG_COMMON_XML_TARGET_NAMESPACE),
                         path);
//   props.no_namespace_schema_location(RPG_PLAYER_SCHEMA_FILE);
//   props.schema_location("http://www.w3.org/XML/1998/namespace", "xml.xsd");
  std::unique_ptr<RPG_Player_PlayerXML_XMLTree_Type> player_p;
  try {
    player_p = ::player_t (ifs,
                           RPG_XSDErrorHandler,
                           flags,
                           props);
  } catch (const ::xml_schema::parsing& exception) {
    std::ostringstream converter;
    converter << exception;
    std::string text = converter.str ();
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to player_t(\"%s\"): exception occurred: \"%s\", aborting\n"),
                ACE_TEXT (filename_in.c_str ()),
                ACE_TEXT (text.c_str ())));
    try {
      ifs.close ();
    } catch (std::ios_base::failure exception) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ifstream::close(\"%s\"): caught exception: \"%s\", aborting\n"),
                  ACE_TEXT (filename_in.c_str ()),
                  ACE_TEXT (exception.what ())));
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ifstream::close(\"%s\"): caught exception, aborting\n"),
                  ACE_TEXT (filename_in.c_str ())));
    }
    return NULL;
  } catch (const ::xml_schema::exception& exception) {
    std::ostringstream converter;
    converter << exception;
    std::string text = converter.str ();
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to player_t(\"%s\"): exception occurred: \"%s\", aborting\n"),
                ACE_TEXT (filename_in.c_str ()),
                ACE_TEXT (text.c_str ())));
    try {
      ifs.close ();
    } catch (std::ios_base::failure exception) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ifstream::close(\"%s\"): caught exception: \"%s\", aborting\n"),
                  ACE_TEXT (filename_in.c_str ()),
                  ACE_TEXT (exception.what ())));
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ifstream::close(\"%s\"): caught exception, aborting\n"),
                  ACE_TEXT (filename_in.c_str ())));
    }
    return NULL;
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to player_t(\"%s\"): exception occurred, aborting\n"),
                ACE_TEXT (filename_in.c_str ())));
    try {
      ifs.close ();
    } catch (std::ios_base::failure exception) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ifstream::close(\"%s\"): caught exception: \"%s\", aborting\n"),
                  ACE_TEXT (filename_in.c_str ()),
                  ACE_TEXT (exception.what ())));
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ifstream::close(\"%s\"): caught exception, aborting\n"),
                  ACE_TEXT (filename_in.c_str ())));
    }
    return NULL;
  }

  try {
    ifs.close ();
  } catch (std::ios_base::failure exception) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to std::ifstream::close(\"%s\"): caught exception: \"%s\", aborting\n"),
                ACE_TEXT (filename_in.c_str ()),
                ACE_TEXT (exception.what ())));
    return NULL;
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to std::ifstream::close(\"%s\"): caught exception, aborting\n"),
                ACE_TEXT (filename_in.c_str ())));
    return NULL;
  }

  return RPG_Player_Common_Tools::playerXMLToPlayer (*player_p,
                                                     condition_in,
                                                     HP_in,
                                                     spells_in);
}

bool
RPG_Player::save (const std::string& filename_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Player::save"));

  std::string filename = filename_in;
  if (filename.empty ())
  {
    filename = RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
    filename += ACE_DIRECTORY_SEPARATOR_CHAR;
    filename += RPG_Common_Tools::sanitize (inherited::getName ());
    filename += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);
  } // end IF

  std::ofstream ofs;
  ofs.exceptions (std::ofstream::badbit | std::ofstream::failbit);
  try {
    ofs.open (filename.c_str (),
              (std::ios_base::out | std::ios_base::trunc));
  } catch (std::ios_base::failure exception) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to std::ofstream::open(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (filename.c_str ()),
                ACE_TEXT (exception.what ())));
    return false;
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to std::ofstream::open(\"%s\"), aborting\n"),
                ACE_TEXT (filename.c_str ())));
    return false;
  }

  RPG_Player_PlayerXML_XMLTree_Type* player_model_p = NULL;
  player_model_p = RPG_Player_Common_Tools::playerToPlayerXML (*this);
  if (!player_model_p)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Player_Common_Tools::playerToPlayerXML(), aborting\n")));
    try {
      ofs.close ();
    } catch (std::ios_base::failure exception) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ofstream::close(): \"%s\", aborting\n"),
                  ACE_TEXT (exception.what ())));
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ofstream::close(), aborting\n")));
    }
    return false;
  } // end IF

  ::xml_schema::namespace_infomap map;
  map[""].name = ACE_TEXT_ALWAYS_CHAR (RPG_COMMON_XML_TARGET_NAMESPACE);
  map[""].schema = ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_SCHEMA_FILE);
  std::string character_set (ACE_TEXT_ALWAYS_CHAR (RPG_COMMON_XML_SCHEMA_CHARSET_UTF8));
  //   ::xml_schema::flags = ::xml_schema::flags::dont_validate;
  ::xml_schema::flags flags = 0;
  try {
    ::player_t (ofs,
                *player_model_p,
                map,
                character_set,
                flags);
  } catch (std::ios_base::failure exception) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("(\"%s\") write error: \"%s\", aborting\n"),
                ACE_TEXT (filename_in.c_str ()),
                ACE_TEXT (exception.what ())));
    try {
      ofs.close ();
    } catch (std::ios_base::failure exception) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ofstream::close(): \"%s\", aborting\n"),
                  ACE_TEXT (exception.what ())));
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ofstream::close(), aborting\n")));
    }
    delete player_model_p;
    return false;
  } catch (const ::xml_schema::serialization& exception) {
    std::ostringstream converter;
    converter << exception;
    std::string text = converter.str ();
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("(\"%s\") exception: \"%s\", aborting\n"),
                ACE_TEXT (filename_in.c_str ()),
                ACE_TEXT (text.c_str ())));
    try {
      ofs.close ();
    } catch (std::ios_base::failure exception) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ofstream::close(): \"%s\", aborting\n"),
                  ACE_TEXT (exception.what ())));
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ofstream::close(), aborting\n")));
    }
    delete player_model_p;
    return false;
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("(\"%s\") exception, aborting\n"),
                ACE_TEXT (filename_in.c_str ())));
    try {
      ofs.close ();
    } catch (std::ios_base::failure exception) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ofstream::close(): \"%s\", aborting\n"),
                  ACE_TEXT (exception.what ())));
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to std::ofstream::close(), aborting\n")));
    }
    delete player_model_p;
    return false;
  }

  delete player_model_p; player_model_p = NULL;

  try {
    ofs.close ();
  } catch (std::ios_base::failure exception) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to std::ofstream::close(): \"%s\", aborting\n"),
                ACE_TEXT (exception.what ())));
    return false;
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to std::ofstream::close(), aborting\n")));
    return false;
  }

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("saved player \"%s\" to file: \"%s\"\n"),
              ACE_TEXT (getName ().c_str ()),
              ACE_TEXT (filename_in.c_str ())));

  return true;
}

RPG_Player*
RPG_Player::dummy()
{
  RPG_TRACE (ACE_TEXT ("RPG_Player::dummy"));

  // init result
  std::string name = ACE_TEXT("");
  RPG_Character_Class player_class;
  player_class.metaClass = METACLASS_NONE;
  RPG_Character_Alignment alignment;
  alignment.civic = ALIGNMENTCIVIC_NEUTRAL;
  alignment.ethic = ALIGNMENTETHIC_NEUTRAL;
  RPG_Character_Attributes attributes;
  attributes.strength = 9;
  attributes.dexterity = 9;
  attributes.constitution = 9;
  attributes.intelligence = 9;
  attributes.wisdom = 9;
  attributes.charisma = 9;
  RPG_Character_Skills_t skills;
  RPG_Character_Feats_t feats;
  RPG_Character_Abilities_t abilities;
  RPG_Magic_SpellTypes_t knownSpells;
  RPG_Character_Conditions_t condition;
  condition.insert(CONDITION_NORMAL);
  RPG_Magic_Spells_t preparedSpells;
  RPG_Item_List_t items;

  RPG_Player* player_p = NULL;
  ACE_NEW_NORETURN(player_p,
                   RPG_Player(// base attributes
                              name,
                              GENDER_NONE,
                              RACE_NONE,
                              player_class,
                              alignment,
                              attributes,
                              skills,
                              feats,
                              abilities,
                              OFFHAND_LEFT,
                              1,
                              knownSpells,
                              // extended data
                              0,
                              0,
                              items,
                              // current status
                              condition,
                              1,
                              preparedSpells));
  if (!player_p)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory(%u), aborting\n"),
               sizeof(RPG_Player)));

    return NULL;
  } // end IF

  return player_p;
}
