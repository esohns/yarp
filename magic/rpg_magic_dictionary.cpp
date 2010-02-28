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
#include "rpg_magic_dictionary.h"

#include "rpg_magic_XML_parser.h"
#include "rpg_magic_common_tools.h"

#include <rpg_character_XML_parser.h>

#include <rpg_common_XML_parser.h>
#include <rpg_common_tools.h>

#include <rpg_dice_XML_parser.h>
#include <rpg_dice_common_tools.h>

#include <ace/Log_Msg.h>

#include <iostream>
#include <algorithm>
#include <sstream>

RPG_Magic_Dictionary::RPG_Magic_Dictionary()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Dictionary::RPG_Magic_Dictionary"));

}

RPG_Magic_Dictionary::~RPG_Magic_Dictionary()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Dictionary::~RPG_Magic_Dictionary"));

}

void RPG_Magic_Dictionary::init(const std::string& filename_in,
                                const bool& validateXML_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Dictionary::init"));

  // Construct the parser.
  //
  ::xml_schema::string_pimpl                  string_p;

  RPG_Magic_SpellType_Type                    spellType_p;
  RPG_Magic_School_Type                       school_p;
  RPG_Magic_SubSchool_Type                    subSchool_p;
  RPG_Magic_Descriptor_Type                   descriptor_p;
  RPG_Magic_Spell_Type_Type                   type_p;
  type_p.parsers(spellType_p,
                 school_p,
                 subSchool_p,
                 descriptor_p);

  RPG_Magic_CasterClassUnion_Type             casterClass_p;
  ::xml_schema::unsigned_byte_pimpl           unsigned_byte_p;
  RPG_Magic_Spell_Level_Type                  level_p;
  level_p.parsers(casterClass_p,
                  unsigned_byte_p);

  ::xml_schema::unsigned_int_pimpl            unsigned_int_p;

  RPG_Common_ActionType_Type                  actionType_p;
  RPG_Magic_Spell_CastingTime_Type            castingTime_p;
  castingTime_p.parsers(unsigned_int_p,
                        actionType_p);

  RPG_Magic_Spell_RangeEffect_Type            rangeEffect_p;
  RPG_Magic_Spell_RangeProperties_Type        range_p;
  range_p.parsers(unsigned_int_p,
                  unsigned_int_p,
                  rangeEffect_p);

  RPG_Magic_Spell_Target_Type                 target_p;
  RPG_Dice_DieType_Type                       dieType_p;
  ::xml_schema::integer_pimpl                 int_p;
  RPG_Dice_Roll_Type                          roll_p;
  roll_p.parsers(unsigned_int_p,
                 dieType_p,
                 int_p);
  ::xml_schema::boolean_pimpl                 bool_p;
  RPG_Common_AreaOfEffect_Type                area_p;
  RPG_Magic_Spell_TargetProperties_Type       targetProperties_p;
  targetProperties_p.parsers(target_p,
                             unsigned_int_p,
                             roll_p,
                             unsigned_byte_p,
                             area_p,
                             unsigned_byte_p,
                             unsigned_byte_p,
                             bool_p);

  RPG_Magic_Spell_Duration_Type               durationType_p;
  RPG_Magic_Spell_DurationProperties_Type     duration_p;
  duration_p.parsers(durationType_p,
                     unsigned_int_p,
                     unsigned_int_p,
                     unsigned_byte_p,
                     unsigned_byte_p,
                     roll_p,
                     bool_p);

  RPG_Magic_Spell_Precondition_Type           precondition_p;
  RPG_Common_Attribute_Type                   attribute_p;
  RPG_Character_Condition_Type                condition_p;
  RPG_Common_CreatureMetaType_Type            creatureMetaType_p;
  RPG_Common_CreatureSubType_Type             creatureSubType_p;
  RPG_Common_CreatureType_Type                creatureType_p;
  creatureType_p.parsers(creatureMetaType_p,
                         creatureSubType_p);
  RPG_Character_Size_Type                     size_p;
  RPG_Magic_Spell_PreconditionProperties_Type preconditionProperties_p;
  preconditionProperties_p.parsers(precondition_p,
                                   int_p,
                                   unsigned_byte_p,
                                   unsigned_byte_p,
                                   attribute_p,
                                   condition_p,
                                   creatureType_p,
                                   size_p,
                                   bool_p);

  RPG_Magic_Spell_Effect_Type                 effect_p;
  RPG_Common_Amount_Type                      amount_p;
  ::xml_schema::byte_pimpl                    byte_p;
  amount_p.parsers(byte_p,
                   roll_p);
  RPG_Common_CounterMeasure_Type              counterType_p;
  RPG_Magic_CheckTypeUnion_Type               checkType_p;
  RPG_Magic_Check_Type                        check_p;
  check_p.parsers(checkType_p,
                  unsigned_byte_p);
  RPG_Common_SaveReductionType_Type           reduction_p;
  RPG_Magic_CounterMeasure_Type               counterMeasure_p;
  counterMeasure_p.parsers(counterType_p,
                           check_p,
                           spellType_p,
                           reduction_p);
  RPG_Magic_Spell_EffectProperties_Type       effectProperties_p;
  effectProperties_p.parsers(effect_p,
                             amount_p,
                             amount_p,
                             unsigned_byte_p,
                             attribute_p,
                             unsigned_byte_p,
                             counterMeasure_p);

  RPG_Common_SavingThrow_Type                 saveType_p;

  RPG_Magic_Spell_PropertiesXML_Type          propertiesXML_p;
  propertiesXML_p.parsers(string_p,
                          type_p,
                          level_p,
                          unsigned_int_p,
                          castingTime_p,
                          range_p,
                          targetProperties_p,
                          duration_p,
                          preconditionProperties_p,
                          effectProperties_p,
                          saveType_p,
                          bool_p);

  RPG_Magic_Dictionary_Type                   dictionary_p(&myDictionary);
  dictionary_p.parsers(propertiesXML_p);

  // Parse the document to obtain the object model.
  //
  ::xml_schema::document doc_p(dictionary_p,
                               ACE_TEXT_ALWAYS_CHAR("urn:rpg"),
                               ACE_TEXT_ALWAYS_CHAR("spellDictionary"));

  dictionary_p.pre();

  // OK: parse the file...
  ::xml_schema::flags flags;
  if (!validateXML_in)
    flags = flags | ::xml_schema::flags::dont_validate;
  try
  {
    doc_p.parse(filename_in,
                myXSDErrorHandler,
                flags);
  }
  catch (const ::xml_schema::parsing& exception)
  {
    std::ostringstream converter;
    converter << exception;
    std::string text = converter.str();
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Magic_Dictionary::init(): exception occurred: \"%s\", returning\n"),
               text.c_str()));

    throw(exception);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Magic_Dictionary::init(): exception occurred, returning\n")));

    throw;
  }

  dictionary_p.post_RPG_Magic_Dictionary_Type();

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("finished parsing character dictionary file \"%s\"...\n"),
//              filename_in.c_str()));
}

const RPG_Magic_Spell_Properties RPG_Magic_Dictionary::getSpellProperties(const std::string& spellName_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Dictionary::getSpellProperties"));

  RPG_Magic_DictionaryIterator_t iterator = myDictionary.find(spellName_in);
  if (iterator == myDictionary.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid spell name \"%s\" --> check implementation !, aborting\n"),
               spellName_in.c_str()));

    ACE_ASSERT(false);
  } // end IF

  return iterator->second;
}

const RPG_Magic_Spells_t RPG_Magic_Dictionary::getSpells(const RPG_Magic_CasterClassUnion& casterClass_in,
                                                         const unsigned char& spellLevel_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Dictionary::getSpellProperties"));

  // sanity check
  ACE_ASSERT(casterClass_in.discriminator != RPG_Magic_CasterClassUnion::INVALID);

  RPG_Magic_Spells_t result;

  bool match = false;
  for (RPG_Magic_DictionaryIterator_t iterator = myDictionary.begin();
       iterator != myDictionary.end();
       iterator++)
  {
    for (RPG_Magic_SpellLevelListIterator_t iterator2 = (*iterator).second.levels.begin();
         iterator2 != (*iterator).second.levels.end();
         iterator2++)
    {
      if ((*iterator2).casterClass.discriminator != casterClass_in.discriminator)
        break;

      match = false;
      switch ((*iterator2).casterClass.discriminator)
      {
        case RPG_Magic_CasterClassUnion::SUBCLASS:
        {
          if ((*iterator2).casterClass.subclass == casterClass_in.subclass)
            match = true;

          break;
        }
        case RPG_Magic_CasterClassUnion::DOMAIN:
        {
          if ((*iterator2).casterClass.domain == casterClass_in.domain)
            match = true;

          break;
        }
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid caster class type: %d --> check implementation !, aborting\n"),
                     (*iterator2).casterClass.discriminator));

          break;
        }
      } // end SWITCH

      if (!match)
        continue;

      if ((spellLevel_in == 0xFF) ||
          (spellLevel_in == (*iterator2).level))
        result.insert((*iterator).second.type.type);

      break;
    } // end FOR
  } // end FOR

  return result;
}

bool RPG_Magic_Dictionary::XSD_Error_Handler::handle(const std::string& id_in,
                                                     unsigned long line_in,
                                                     unsigned long column_in,
                                                     ::xml_schema::error_handler::severity severity_in,
                                                     const std::string& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Dictionary::XSD_Error_Handler::handle"));

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("error occured (ID: \"%s\", location: %d, %d): \"%s\" --> check implementation !, continuing\n"),
//              id_in.c_str(),
//              line_in,
//              column_in,
//              message_in.c_str()));

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

void RPG_Magic_Dictionary::dump() const
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Dictionary::dump"));

  // simply dump the current content of our dictionary
  std::stringstream converter;
  for (RPG_Magic_DictionaryIterator_t iterator = myDictionary.begin();
       iterator != myDictionary.end();
       iterator++)
  {
    std::string castingTime;
    if ((iterator->second).time.rounds)
    {
      converter.str(ACE_TEXT_ALWAYS_CHAR(""));
      converter << ACE_static_cast(unsigned int, (iterator->second).time.rounds);
      castingTime += converter.str();
      castingTime += ACE_TEXT_ALWAYS_CHAR(" rd(s)");
    } // end IF
    else
      castingTime = RPG_Common_ActionTypeHelper::RPG_Common_ActionTypeToString((iterator->second).time.action);

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("Spell (\"%s\"):\nType: %s\nLevel(s):\n---------\n%sXP Cost: %d\nCasting Time: %s\nRange:\n------\n%sTarget:\n-------\n%sDuration:\n---------\n%s\nPrecondition(s):\n--------------\n%sSave: %s\nEffect(s):\n----------\n%sResistible: %s\n"),
               (iterator->first).c_str(),
               RPG_Magic_Common_Tools::spellTypeToString((iterator->second).type).c_str(),
               RPG_Magic_Common_Tools::spellLevelsToString((iterator->second).levels).c_str(),
               (iterator->second).cost,
               castingTime.c_str(),
               RPG_Magic_Common_Tools::spellRangeToString((iterator->second).range).c_str(),
               RPG_Magic_Common_Tools::spellTargetToString((iterator->second).target).c_str(),
               RPG_Magic_Common_Tools::spellDurationToString((iterator->second).duration).c_str(),
               RPG_Magic_Common_Tools::preconditionsToString((iterator->second).preconditions).c_str(),
               RPG_Common_SavingThrowHelper::RPG_Common_SavingThrowToString((iterator->second).saveable).c_str(),
               RPG_Magic_Common_Tools::effectsToString((iterator->second).effects).c_str(),
               ((iterator->second).resistible ? ACE_TEXT_ALWAYS_CHAR("true") : ACE_TEXT_ALWAYS_CHAR("false"))));
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("===========================\n")));
  } // end FOR
}
