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

#include "rpg_magic_dictionary.h"

#include <iostream>
#include <algorithm>
#include <sstream>

#include "ace/Log_Msg.h"

#include "common_macros.h"

#include "rpg_dice_XML_parser.h"
#include "rpg_dice_common_tools.h"

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"
#include "rpg_common_XML_tools.h"
#include "rpg_common_XML_parser.h"
#include "rpg_common_tools.h"

#include "rpg_character_XML_parser.h"

#include "rpg_magic_XML_parser.h"
#include "rpg_magic_common_tools.h"

RPG_Magic_Dictionary::RPG_Magic_Dictionary()
 : dictionary_ ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Dictionary::RPG_Magic_Dictionary"));

}

bool
RPG_Magic_Dictionary::initialize (const std::string& filename_in,
                                  bool validateXML_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Dictionary::initialize"));

  // Construct the parser.
  //
  ::xml_schema::string_pimpl                  string_p;

  RPG_Magic_SpellType_Type                    spellType_p;
  RPG_Magic_School_Type                       school_p;
  RPG_Magic_SubSchool_Type                    subSchool_p;
  RPG_Magic_Descriptor_Type                   descriptor_p;
  RPG_Magic_Spell_Type_Type                   type_p;
  type_p.parsers (spellType_p,
                  school_p,
                  subSchool_p,
                  descriptor_p,
                  spellType_p);

  RPG_Magic_CasterClassUnion_Type             casterClass_p;
  ::xml_schema::unsigned_byte_pimpl           unsigned_byte_p;
  RPG_Magic_Spell_Level_Type                  level_p;
  level_p.parsers (casterClass_p,
                   unsigned_byte_p);

  ::xml_schema::unsigned_int_pimpl            unsigned_int_p;

  RPG_Common_ActionType_Type                  actionType_p;
  RPG_Magic_Spell_CastingTime_Type            castingTime_p;
  castingTime_p.parsers (unsigned_int_p,
                         actionType_p);

  RPG_Magic_Spell_RangeEffect_Type            rangeEffect_p;
  RPG_Magic_Spell_RangeProperties_Type        range_p;
  range_p.parsers (unsigned_int_p,
                   unsigned_int_p,
                   rangeEffect_p);

  RPG_Magic_Spell_Target_Type                 target_p;
  RPG_Common_Amount_Type                      amount_p;
  ::xml_schema::short_pimpl                   short_p;
  RPG_Dice_DieType_Type                       dieType_p;
  ::xml_schema::int_pimpl                     int_p;
  RPG_Dice_Roll_Type                          roll_p;
  roll_p.parsers (unsigned_int_p,
                  dieType_p,
                  int_p);
  amount_p.parsers (short_p,
                    roll_p);
  ::xml_schema::boolean_pimpl                 bool_p;
  RPG_Magic_Spell_AreaOfEffect_Type           area_of_effect_p;
  RPG_Common_AreaOfEffect_Type                shape_p;
  RPG_Magic_Spell_TargetProperties_Type       targetProperties_p;
  targetProperties_p.parsers (amount_p,
                              unsigned_byte_p,
                              unsigned_byte_p,
                              area_of_effect_p,
                              shape_p,
                              unsigned_int_p,
                              unsigned_int_p,
                              target_p,
                              bool_p,
                              bool_p,
                              bool_p);

  ::xml_schema::byte_pimpl                    byte_p;
  RPG_Common_EffectDuration_Type              effectDuration_p;
  effectDuration_p.parsers (amount_p,
                            byte_p,
                            unsigned_byte_p,
                            unsigned_byte_p,
                            bool_p,
                            bool_p);
  RPG_Magic_Spell_Duration_Type               durationType_p;
  RPG_Magic_Spell_DurationProperties_Type     duration_p;
  duration_p.parsers (amount_p,
                      byte_p,
                      unsigned_byte_p,
                      unsigned_byte_p,
                      bool_p,
                      bool_p,
                      durationType_p,
                      bool_p);

  RPG_Magic_Spell_Precondition_Type           precondition_p;
  RPG_Character_AlignmentEthic_Type           ethic_p;
  RPG_Character_AlignmentCivic_Type           civic_p;
  RPG_Character_Alignment_Type                alignment_p;
  alignment_p.parsers (civic_p,
                       ethic_p);
  RPG_Common_Attribute_Type                   attribute_p;
  RPG_Common_Condition_Type                   condition_p;
  RPG_Common_CreatureMetaType_Type            creatureMetaType_p;
  RPG_Common_CreatureSubType_Type             creatureSubType_p;
  RPG_Common_CreatureType_Type                creatureType_p;
  creatureType_p.parsers (creatureMetaType_p,
                          creatureSubType_p);
  RPG_Common_Size_Type                        size_p;
  RPG_Common_Plane_Type                       plane_p;
  RPG_Common_Terrain_Type                     terrain_p;
  RPG_Common_Climate_Type                     climate_p;
  RPG_Common_TimeOfDay_Type                   time_p;
  RPG_Common_AmbientLighting_Type             lighting_p;
  RPG_Common_Environment_Type                 environment_p;
  environment_p.parsers (plane_p,
                         terrain_p,
                         climate_p,
                         time_p,
                         lighting_p,
                         bool_p);
  RPG_Magic_Spell_PreconditionProperties_Type preconditionProperties_p;
  preconditionProperties_p.parsers (precondition_p,
                                    int_p,
                                    unsigned_byte_p,
                                    unsigned_byte_p,
                                    alignment_p,
                                    attribute_p,
                                    condition_p,
                                    creatureType_p,
                                    size_p,
                                    environment_p,
                                    bool_p,
                                    bool_p);

  RPG_Magic_Spell_Effect_Type                 effect_p;
  RPG_Magic_Spell_DamageTypeUnion_Type        damageType_p;
  RPG_Magic_Domain_Type                       domain_p;
  RPG_Common_CounterMeasure_Type              counterType_p;
  RPG_Magic_CheckTypeUnion_Type               checkType_p;
  RPG_Magic_Check_Type                        check_p;
  check_p.parsers (checkType_p,
                   unsigned_byte_p,
                   byte_p,
                   unsigned_byte_p,
                   bool_p);
  RPG_Common_SaveReductionType_Type           reduction_p;
  RPG_Magic_CounterMeasure_Type               counterMeasure_p;
  counterMeasure_p.parsers (counterType_p,
                            check_p,
                            spellType_p,
                            reduction_p);
  RPG_Magic_Spell_EffectProperties_Type       effectProperties_p;
  effectProperties_p.parsers (effect_p,
                              damageType_p,
                              amount_p,
                              unsigned_int_p,
                              amount_p,
                              unsigned_byte_p,
                              attribute_p,
                              domain_p,
                              creatureType_p,
                              effectDuration_p,
                              preconditionProperties_p,
                              unsigned_byte_p,
                              counterMeasure_p,
                              bool_p,
                              bool_p);

  RPG_Common_SavingThrow_Type                 saveType_p;

  RPG_Magic_Spell_PropertiesXML_Type          propertiesXML_p;
  propertiesXML_p.parsers (string_p,
                           type_p,
                           level_p,
                           unsigned_int_p,
                           castingTime_p,
                           range_p,
                           targetProperties_p,
                           duration_p,
                           preconditionProperties_p,
                           effectProperties_p,
                           counterMeasure_p,
                           saveType_p,
                           bool_p,
                           bool_p);

  RPG_Magic_Dictionary_Type                   dictionary_p (&dictionary_);
  dictionary_p.parsers (propertiesXML_p);

  // Parse the document to obtain the object model.
  //
  ::xml_schema::document doc_p (dictionary_p,                                          // parser
                                ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_XML_TARGET_NAMESPACE), // namespace
                                ACE_TEXT_ALWAYS_CHAR ("spellDictionary"),              // root element name
                                false);                                                // polymorphic ?

  dictionary_p.pre ();

  // OK: parse the file...
  ::xml_schema::flags flags;
  if (!validateXML_in)
    flags = flags | ::xml_schema::flags::dont_validate;
	::xml_schema::properties properties;
  try {
    //doc_p.parse(filename_in,
    //            RPG_XSDErrorHandler,
    //            flags,
    //            properties);
	  doc_p.parse (filename_in,
                 *RPG_Common_XML_Tools::parser (),
                 flags,
                 properties);
  } catch (const ::xml_schema::parsing& exception) {
    std::ostringstream converter;
    converter << exception;
    std::string text = converter.str ();
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("RPG_Magic_Dictionary::initialize(): exception occurred: \"%s\", aborting\n"),
                ACE_TEXT (text.c_str ())));
    return false;
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("RPG_Magic_Dictionary::initialize(): exception occurred, aborting\n")));
    return false;
  }

  dictionary_p.post_RPG_Magic_Dictionary_Type ();

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished parsing magic dictionary file \"%s\"...\n"),
              ACE_TEXT (filename_in.c_str ())));

	return true;
}

struct RPG_Magic_Spell_Properties
RPG_Magic_Dictionary::getSpellProperties (const std::string& spellName_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Dictionary::getSpellProperties"));

  struct RPG_Magic_Spell_Properties result;

  RPG_Magic_DictionaryIterator_t iterator = dictionary_.find (spellName_in);
  if (unlikely (iterator == dictionary_.end ()))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid/unknown spell name (was: \"%s\"), aborting\n"),
                ACE_TEXT (spellName_in.c_str ())));
  else
    result = iterator->second;

  return result;
}

struct RPG_Magic_Spell_Properties
RPG_Magic_Dictionary::getSpellProperties (enum RPG_Magic_SpellType spellType_in,
                                          std::string& spellName_out) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Dictionary::getSpellProperties"));

  // reset return value(s)
  struct RPG_Magic_Spell_Properties result;
  spellName_out.resize (0);

  RPG_Magic_DictionaryIterator_t iterator = dictionary_.begin ();
  for (;
       iterator != dictionary_.end();
       iterator++)
  {
    if ((*iterator).second.type.type == spellType_in)
    {
      spellName_out = iterator->first;
      result = iterator->second;
      break;
    } // end IF
  } // end IF
  if (unlikely (iterator == dictionary_.end ()))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT("invalid/unknown spell type (was: \"%s\"), aborting\n"),
                ACE_TEXT (RPG_Magic_SpellTypeHelper::RPG_Magic_SpellTypeToString (spellType_in).c_str ())));

  return result;
}

RPG_Magic_SpellTypes_t
RPG_Magic_Dictionary::getSpells (const RPG_Magic_CasterClassUnion& casterClass_in,
                                 ACE_UINT8 spellLevel_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Dictionary::getSpells"));

  // sanity check(s)
  ACE_ASSERT (casterClass_in.discriminator != RPG_Magic_CasterClassUnion::INVALID);

  RPG_Magic_SpellTypes_t result;

  bool match = false;
  for (RPG_Magic_DictionaryIterator_t iterator = dictionary_.begin();
       iterator != dictionary_.end();
       iterator++)
    for (RPG_Magic_SpellLevelsIterator_t iterator2 = (*iterator).second.levels.begin();
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
        // *PORTABILITY*: "DOMAIN" seems to be a constant (see math.h)
        // --> provide a (temporary) workaround here...
#if defined (__GNUC__) || defined (_MSC_VER)
#pragma message("applying quirk code for this compiler...")
        case RPG_Magic_CasterClassUnion::__QUIRK__DOMAIN:
#else
#pragma message("re-check code for this compiler")
        case RPG_Magic_CasterClassUnion::DOMAIN:
#endif // __GNUC__ || _MSC_VER
        {
          if ((*iterator2).casterClass.domain == casterClass_in.domain)
            match = true;
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid caster class type (was: %d) --> check implementation, continuing\n"),
                      (*iterator2).casterClass.discriminator));
          break;
        }
      } // end SWITCH
      if (!match)
        continue;

      if ((spellLevel_in == 0xFF)              ||
          (spellLevel_in == (*iterator2).level))
        result.insert ((*iterator).second.type.type);
      break;
    } // end FOR

  return result;
}

void
RPG_Magic_Dictionary::dump (bool groupLevels_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Dictionary::dump"));

  if (groupLevels_in)
    return dumpLevels ();

  std::ostringstream converter;
  for (RPG_Magic_DictionaryIterator_t iterator = dictionary_.begin ();
       iterator != dictionary_.end ();
       iterator++)
  {
    std::string castingTime;
    if ((iterator->second).time.rounds)
    {
      converter.clear ();
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter << static_cast<unsigned int> ((iterator->second).time.rounds);
      castingTime += converter.str ();
      castingTime += ACE_TEXT_ALWAYS_CHAR (" rd(s)");
    } // end IF
    else
      castingTime =
        RPG_Common_ActionTypeHelper::RPG_Common_ActionTypeToString ((iterator->second).time.action);

    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("Spell (\"%s\"):\nType: %s\nLevel(s):\n---------\n%sXP Cost: %u\nCasting Time: %s\nRange:\n------\n%sTarget(s):\n-------\n%sDuration:\n---------\n%s\nPrecondition(s):\n--------------\n%sSave: %s\nEffect(s):\n----------\n%sCounterMeasure(s):\n----------\n%sResistible: %s\n"),
                ACE_TEXT ((iterator->first).c_str ()),
                ACE_TEXT (RPG_Magic_Common_Tools::toString ((iterator->second).type).c_str ()),
                ACE_TEXT (RPG_Magic_Common_Tools::toString ((iterator->second).levels).c_str ()),
                (iterator->second).cost,
                ACE_TEXT (castingTime.c_str ()),
                ACE_TEXT (RPG_Magic_Common_Tools::toString ((iterator->second).range).c_str ()),
                ACE_TEXT (RPG_Magic_Common_Tools::toString ((iterator->second).targets).c_str ()),
                ACE_TEXT (RPG_Magic_Common_Tools::toString ((iterator->second).duration).c_str ()),
                ACE_TEXT (RPG_Magic_Common_Tools::toString ((iterator->second).preconditions).c_str ()),
                ACE_TEXT (RPG_Common_SavingThrowHelper::RPG_Common_SavingThrowToString ((iterator->second).saveable).c_str ()),
                ACE_TEXT (RPG_Magic_Common_Tools::toString ((iterator->second).effects).c_str ()),
                ACE_TEXT (RPG_Magic_Common_Tools::toString ((iterator->second).counterMeasures).c_str ()),
                ((iterator->second).resistible ? ACE_TEXT ("true") : ACE_TEXT ("false"))));
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("===========================\n")));
  } // end FOR
}

void
RPG_Magic_Dictionary::dumpLevels () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Magic_Dictionary::dumpLevels"));

  RPG_Magic_SpellTypes_t spell_types;
  RPG_Magic_DictionaryIterator_t iterator;
  RPG_Magic_SpellLevelsIterator_t iterator2;
  RPG_Magic_SpellTypesIterator_t iterator3;
  unsigned int index_i = 0;
  for (ACE_UINT8 i = 0;
       i <= RPG_COMMON_MAX_SPELL_LEVEL;
       ++i)
  {
    // step1: collect all known spells / level
    spell_types.clear ();
    for (iterator = dictionary_.begin ();
         iterator != dictionary_.end ();
         iterator++)
      for (iterator2 = (*iterator).second.levels.begin ();
           iterator2 != (*iterator).second.levels.end ();
           iterator2++)
        if ((*iterator2).level == i)
        {
          spell_types.insert ((*iterator).second.type.type);
          break;
        } // end IF

    // step2: list known spells for this level
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("============== level %u [%u] =============\n"),
                static_cast<unsigned int> (i),
                spell_types.size ()));
    index_i = 0;
    for (iterator3 = spell_types.begin();
         iterator3 != spell_types.end();
         ++iterator3, ++index_i)
      ACE_DEBUG ((LM_INFO,
                  ACE_TEXT ("[%u]: %s\n"),
                  index_i,
                  ACE_TEXT (RPG_Magic_SpellTypeHelper::RPG_Magic_SpellTypeToString (*iterator3).c_str ())));
  } // end FOR
}
