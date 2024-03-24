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

#include "rpg_combat_common_tools.h"

#include <sstream>

#include "ace/Log_Msg.h"

#include "rpg_common_macros.h"
#include "rpg_common_tools.h"

#include "rpg_dice_common_tools.h"

// initialize statics
RPG_Combat_AttackFormToStringTable_t RPG_Combat_AttackFormHelper::myRPG_Combat_AttackFormToStringTable;
RPG_Combat_AttackSituationToStringTable_t RPG_Combat_AttackSituationHelper::myRPG_Combat_AttackSituationToStringTable;
RPG_Combat_DefenseSituationToStringTable_t RPG_Combat_DefenseSituationHelper::myRPG_Combat_DefenseSituationToStringTable;
RPG_Combat_SpecialAttackToStringTable_t RPG_Combat_SpecialAttackHelper::myRPG_Combat_SpecialAttackToStringTable;
RPG_Combat_SpecialDamageTypeToStringTable_t RPG_Combat_SpecialDamageTypeHelper::myRPG_Combat_SpecialDamageTypeToStringTable;
RPG_Combat_OtherDamageTypeToStringTable_t RPG_Combat_OtherDamageTypeHelper::myRPG_Combat_OtherDamageTypeToStringTable;
RPG_Combat_DamageReductionTypeToStringTable_t RPG_Combat_DamageReductionTypeHelper::myRPG_Combat_DamageReductionTypeToStringTable;

void
RPG_Combat_Common_Tools::initializeStringConversionTables ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Combat_Common_Tools::initStringConversionTables"));

  RPG_Combat_AttackFormHelper::init ();
  RPG_Combat_AttackSituationHelper::init ();
  RPG_Combat_DefenseSituationHelper::init ();
  RPG_Combat_SpecialAttackHelper::init ();
  RPG_Combat_SpecialDamageTypeHelper::init ();
  RPG_Combat_OtherDamageTypeHelper::init ();
  RPG_Combat_DamageReductionTypeHelper::init ();

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("RPG_Combat_Common_Tools: initialized string conversion tables...\n")));
}

std::string
RPG_Combat_Common_Tools::toString (const RPG_Combat_AttackForms_t& attackForms_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Combat_Common_Tools::attackFormsToString"));

  std::string result;

  for (RPG_Combat_AttackFormsIterator_t iterator = attackForms_in.begin ();
       iterator != attackForms_in.end ();
       iterator++)
  {
    result += RPG_Combat_AttackFormHelper::RPG_Combat_AttackFormToString (*iterator);
    result += ACE_TEXT_ALWAYS_CHAR ("|");
  } // end FOR

  if (!result.empty ())
    result.erase (--(result.end ()));

  return result;
}

std::string
RPG_Combat_Common_Tools::toString (const struct RPG_Combat_Damage& damage_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Combat_Common_Tools::damageToString"));

  std::string result;

  std::ostringstream converter;
  for (std::vector<RPG_Combat_DamageElement>::const_iterator iterator = damage_in.elements.begin ();
       iterator != damage_in.elements.end ();
       iterator++)
  {
    result += ACE_TEXT_ALWAYS_CHAR ("damage type: ");
    for (std::vector<RPG_Combat_DamageTypeUnion>::const_iterator iterator2 = (*iterator).types.begin ();
         iterator2 != (*iterator).types.end ();
         iterator2++)
    {
      result += RPG_Combat_Common_Tools::toString (*iterator2);
      result += ACE_TEXT_ALWAYS_CHAR ("|");
    } // end FOR
    if (!(*iterator).types.empty ())
      result.erase (--result.end ());
    result += ACE_TEXT_ALWAYS_CHAR ("\namount: ");
    result += RPG_Dice_Common_Tools::toString ((*iterator).amount);
    if ((*iterator).duration.interval ||
        (*iterator).duration.totalDuration)
    {
      result += ACE_TEXT_ALWAYS_CHAR ("\nduration (incubation / interval / total): ");
      result += RPG_Dice_Common_Tools::toString ((*iterator).duration.incubationPeriod);
      result += ACE_TEXT_ALWAYS_CHAR (" / ");
      converter.clear ();
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter << (*iterator).duration.interval;
      result += converter.str ();
      result += ACE_TEXT_ALWAYS_CHAR (" / ");
      converter.clear ();
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter << (*iterator).duration.totalDuration;
      result += converter.str ();
    } // end IF
    for (std::vector<RPG_Combat_OtherDamage>::const_iterator iterator2 = (*iterator).others.begin ();
         iterator2 != (*iterator).others.end ();
         iterator2++)
    {
      result += ACE_TEXT_ALWAYS_CHAR ("\nother (bonus / modifier): ");
      result += RPG_Combat_OtherDamageTypeHelper::RPG_Combat_OtherDamageTypeToString ((*iterator2).type);
      result += ACE_TEXT_ALWAYS_CHAR (" / ");
      converter.clear ();
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter << static_cast<int> ((*iterator2).modifier);
      result += converter.str ();
    } // end FOR
    if ((*iterator).attribute != RPG_COMMON_ATTRIBUTE_INVALID)
    {
      result += ACE_TEXT_ALWAYS_CHAR ("\nattribute: ");
      result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString ((*iterator).attribute);
    } // end IF
    for (std::vector<RPG_Combat_DamageCounterMeasure>::const_iterator iterator2 = (*iterator).counterMeasures.begin ();
         iterator2 != (*iterator).counterMeasures.end ();
         iterator2++)
    {
      result +=
        ACE_TEXT_ALWAYS_CHAR ("\ncounter-measure ([reduction] type (check|spell) [(attribute) DC]: ");
      if ((*iterator2).reduction != REDUCTION_FULL)
      {
        result +=
          RPG_Combat_DamageReductionTypeHelper::RPG_Combat_DamageReductionTypeToString ((*iterator2).reduction);
        result += ACE_TEXT_ALWAYS_CHAR (" ");
      } // end IF
//       result += RPG_Combat_DamageCounterMeasureTypeHelper::RPG_Combat_DamageCounterMeasureTypeToString((*iterator).type);
//       result += ACE_TEXT_ALWAYS_CHAR(" ");
      switch ((*iterator2).type)
      {
        case COUNTERMEASURE_CHECK:
        {
          switch ((*iterator2).check.type.discriminator)
          {
            case RPG_Combat_CheckTypeUnion::SKILL:
            {
              result += RPG_Common_SkillHelper::RPG_Common_SkillToString ((*iterator2).check.type.skill);
              result += ACE_TEXT_ALWAYS_CHAR (" ");
              break;
            }
            case RPG_Combat_CheckTypeUnion::ATTRIBUTE:
            {
              result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString ((*iterator2).check.type.attribute);
              result += ACE_TEXT_ALWAYS_CHAR (" ");
              break;
            }
            case RPG_Combat_CheckTypeUnion::BASECHECKTYPEUNION:
            {
              switch ((*iterator2).check.type.basechecktypeunion.discriminator)
              {
                case RPG_Common_BaseCheckTypeUnion::CHECKTYPE:
                {
                  result += RPG_Common_CheckTypeHelper::RPG_Common_CheckTypeToString ((*iterator2).check.type.basechecktypeunion.checktype);
                  result += ACE_TEXT_ALWAYS_CHAR (" ");
                  break;
                }
                case RPG_Common_BaseCheckTypeUnion::SAVINGTHROW:
                {
                  result += RPG_Common_SavingThrowHelper::RPG_Common_SavingThrowToString ((*iterator2).check.type.basechecktypeunion.savingthrow);
                  result += ACE_TEXT_ALWAYS_CHAR (" ");

                  if ((*iterator2).check.attribute != RPG_COMMON_ATTRIBUTE_INVALID)
                  {
                    result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString ((*iterator2).check.attribute);
                    result += ACE_TEXT_ALWAYS_CHAR (" ");
                  } // end IF
                  else
                  {
                    result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString (RPG_Common_Tools::savingThrowToAttribute ((*iterator2).check.type.basechecktypeunion.savingthrow));
                    result += ACE_TEXT_ALWAYS_CHAR (" ");
                  } // end ELSE

                  break;
                }
                default:
                {
                  ACE_DEBUG ((LM_ERROR,
                              ACE_TEXT ("invalid RPG_Common_BaseCheckTypeUnion type: %d, continuing\n"),
                              (*iterator2).check.type.basechecktypeunion.discriminator));
                  break;
                }
              } // end SWITCH

              break;
            }
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid RPG_Combat_CheckTypeUnion type: %d, continuing\n"),
                          (*iterator2).check.type.discriminator));
              break;
            }
          } // end SWITCH

          converter.clear ();
          converter.str (ACE_TEXT_ALWAYS_CHAR (""));
          converter << static_cast<int> ((*iterator2).check.difficultyClass);
          result += converter.str ();

          break;
        }
        case COUNTERMEASURE_SPELL:
        {
          for (std::vector<RPG_Magic_SpellType>::const_iterator iterator3 = (*iterator2).spells.begin ();
               iterator3 != (*iterator2).spells.end ();
               iterator3++)
          {
            result += RPG_Magic_SpellTypeHelper::RPG_Magic_SpellTypeToString (*iterator3);
            result += ACE_TEXT_ALWAYS_CHAR ("|");
          } // end FOR
          if (!(*iterator2).spells.empty ())
            result.erase (--(result.end ()));

          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid counterMeasure: \"%s\", continuing\n"),
                      ACE_TEXT (RPG_Common_CounterMeasureHelper::RPG_Common_CounterMeasureToString ((*iterator2).type).c_str ())));
          break;
        }
      } // end SWITCH
    } // end FOR
    if ((*iterator).effect != EFFECT_IMMEDIATE)
    {
      result += ACE_TEXT_ALWAYS_CHAR ("\neffect: ");
      result += RPG_Common_EffectTypeHelper::RPG_Common_EffectTypeToString ((*iterator).effect);
      result += ACE_TEXT_ALWAYS_CHAR ("\n");
    } // end IF
  } // end FOR

  return result;
}

std::string
RPG_Combat_Common_Tools::toString (const struct RPG_Combat_DamageTypeUnion& type_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Combat_Common_Tools::toString"));

  std::string result;

  switch (type_in.discriminator)
  {
    case RPG_Combat_DamageTypeUnion::PHYSICALDAMAGETYPE:
      return RPG_Common_PhysicalDamageTypeHelper::RPG_Common_PhysicalDamageTypeToString (type_in.physicaldamagetype);
    case RPG_Combat_DamageTypeUnion::SPECIALDAMAGETYPE:
      return RPG_Combat_SpecialDamageTypeHelper::RPG_Combat_SpecialDamageTypeToString (type_in.specialdamagetype);
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid damage type (was: %d), aborting\n"),
                  type_in.discriminator));
      break;
    }
  } // end SWITCH

  return result;
}
