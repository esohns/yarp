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
#include "rpg_magic_common_tools.h"

#include <rpg_dice_incl.h>
#include <rpg_common_incl.h>
#include "rpg_magic_incl.h"

#include <rpg_dice_common_tools.h>

#include <ace/Log_Msg.h>

#include <string>
#include <sstream>

// init statics
RPG_Magic_SchoolToStringTable_t RPG_Magic_SchoolHelper::myRPG_Magic_SchoolToStringTable;
RPG_Magic_SubSchoolToStringTable_t RPG_Magic_SubSchoolHelper::myRPG_Magic_SubSchoolToStringTable;
RPG_Magic_DescriptorToStringTable_t RPG_Magic_DescriptorHelper::myRPG_Magic_DescriptorToStringTable;
RPG_Magic_DomainToStringTable_t RPG_Magic_DomainHelper::myRPG_Magic_DomainToStringTable;
RPG_Magic_SpellTypeToStringTable_t RPG_Magic_SpellTypeHelper::myRPG_Magic_SpellTypeToStringTable;
RPG_Magic_AbilityClassToStringTable_t RPG_Magic_AbilityClassHelper::myRPG_Magic_AbilityClassToStringTable;
RPG_Magic_AbilityTypeToStringTable_t RPG_Magic_AbilityTypeHelper::myRPG_Magic_AbilityTypeToStringTable;
RPG_Magic_Spell_RangeEffectToStringTable_t RPG_Magic_Spell_RangeEffectHelper::myRPG_Magic_Spell_RangeEffectToStringTable;
RPG_Magic_Spell_TargetToStringTable_t RPG_Magic_Spell_TargetHelper::myRPG_Magic_Spell_TargetToStringTable;
RPG_Magic_Spell_DurationToStringTable_t RPG_Magic_Spell_DurationHelper::myRPG_Magic_Spell_DurationToStringTable;
RPG_Magic_Spell_PreconditionToStringTable_t RPG_Magic_Spell_PreconditionHelper::myRPG_Magic_Spell_PreconditionToStringTable;
RPG_Magic_Spell_EffectToStringTable_t RPG_Magic_Spell_EffectHelper::myRPG_Magic_Spell_EffectToStringTable;

void RPG_Magic_Common_Tools::initStringConversionTables()
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Common_Tools::initStringConversionTables"));

  RPG_Magic_SchoolHelper::init();
  RPG_Magic_SubSchoolHelper::init();
  RPG_Magic_DescriptorHelper::init();
  RPG_Magic_DomainHelper::init();
  RPG_Magic_SpellTypeHelper::init();
  RPG_Magic_AbilityClassHelper::init();
  RPG_Magic_AbilityTypeHelper::init();
  RPG_Magic_Spell_RangeEffectHelper::init();
  RPG_Magic_Spell_TargetHelper::init();
  RPG_Magic_Spell_DurationHelper::init();
  RPG_Magic_Spell_PreconditionHelper::init();
  RPG_Magic_Spell_EffectHelper::init();

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Magic_Common_Tools: initialized string conversion tables...\n")));
}

const std::string RPG_Magic_Common_Tools::spellTypeToString(const RPG_Magic_Spell_Type& type_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Common_Tools::spellTypeToString"));

  std::string result = RPG_Magic_SpellTypeHelper::RPG_Magic_SpellTypeToString(type_in.type);
  result += ACE_TEXT_ALWAYS_CHAR(" (");
  result += RPG_Magic_SchoolHelper::RPG_Magic_SchoolToString(type_in.school);
  if (type_in.subSchool != RPG_MAGIC_SUBSCHOOL_INVALID)
  {
    result += ACE_TEXT_ALWAYS_CHAR(" / ");
    result += RPG_Magic_SubSchoolHelper::RPG_Magic_SubSchoolToString(type_in.subSchool);
  } // end IF
  result += ACE_TEXT_ALWAYS_CHAR(")");
  if (!type_in.descriptors.empty())
  {
    result += ACE_TEXT_ALWAYS_CHAR(": ");
    for (std::vector<RPG_Magic_Descriptor>::const_iterator iterator = type_in.descriptors.begin();
         iterator != type_in.descriptors.end();
         iterator++)
    {
      result += RPG_Magic_DescriptorHelper::RPG_Magic_DescriptorToString(*iterator);
      result += ACE_TEXT_ALWAYS_CHAR(",");
    } // end FOR
    result.erase(--(result.end()));
  } // end IF

  return result;
}

const std::string RPG_Magic_Common_Tools::spellLevelsToString(const RPG_Magic_SpellLevelList_t& levels_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Common_Tools::spellLevelsToString"));

  std::string result;
  std::stringstream converter;
  for (RPG_Magic_SpellLevelListIterator_t iterator = levels_in.begin();
       iterator != levels_in.end();
       iterator++)
  {
    switch ((*iterator).casterClass.discriminator)
    {
      case RPG_Magic_CasterClassUnion::SUBCLASS:
      {
        result += RPG_Common_SubClassHelper::RPG_Common_SubClassToString((*iterator).casterClass.subclass);

        break;
      }
      case RPG_Magic_CasterClassUnion::DOMAIN:
      {
        result += RPG_Magic_DomainHelper::RPG_Magic_DomainToString((*iterator).casterClass.domain);

        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid casterClass \"%d\" --> check implementation !, continuing\n"),
                   (*iterator).casterClass.discriminator));

        break;
      }
    } // end SWITCH

    result += ACE_TEXT_ALWAYS_CHAR(": ");
    converter.str(ACE_TEXT_ALWAYS_CHAR(""));
    converter << ACE_static_cast(unsigned int,(*iterator).level);
    result += converter.str();
    result += ACE_TEXT_ALWAYS_CHAR("\n");
  } // end FOR

  return result;
}

const std::string RPG_Magic_Common_Tools::spellRangeToString(const RPG_Magic_Spell_RangeProperties& range_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Common_Tools::spellRangeToString"));

  std::string result;
  std::stringstream converter;

  RPG_Magic_Spell_RangeProperties temp = range_in;
  if ((range_in.max == 0) || (range_in.increment == 0))
    updateSpellRange(temp);

  switch (temp.effect)
  {
    case RANGEEFFECT_PERSONAL:
    case RANGEEFFECT_TOUCH:
    {
      break;
    }
    case RANGEEFFECT_CLOSE:
    case RANGEEFFECT_MEDIUM:
    case RANGEEFFECT_LONG:
    case RANGEEFFECT_UNLIMITED:
    case RANGEEFFECT_RANGED:
    {
      result += ACE_TEXT_ALWAYS_CHAR("max: ");
      converter << temp.max;
      result += converter.str();
      result += ACE_TEXT_ALWAYS_CHAR(" ft\n");
      result += ACE_TEXT_ALWAYS_CHAR("increment: ");
      converter.str(ACE_TEXT_ALWAYS_CHAR(""));
      converter << temp.increment;
      result += converter.str();
      result += ACE_TEXT_ALWAYS_CHAR(" ft\n");

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid effect \"%s\" --> check implementation !, returning\n"),
                 RPG_Magic_Spell_RangeEffectHelper::RPG_Magic_Spell_RangeEffectToString(temp.effect).c_str()));

      break;
    }
  } // end SWITCH
  result += ACE_TEXT_ALWAYS_CHAR("effect: ");
  result += RPG_Magic_Spell_RangeEffectHelper::RPG_Magic_Spell_RangeEffectToString(temp.effect);
  result += ACE_TEXT_ALWAYS_CHAR("\n");

  return result;
}

const std::string RPG_Magic_Common_Tools::spellTargetToString(const RPG_Magic_Spell_TargetProperties& target_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Common_Tools::spellTargetToString"));

  std::string result;
  std::stringstream converter;

  result += ACE_TEXT_ALWAYS_CHAR("type: ");
  result += RPG_Magic_Spell_TargetHelper::RPG_Magic_Spell_TargetToString(target_in.type);
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  switch (target_in.type)
  {
    case TARGET_SELF:
    case TARGET_LOCATION:
    {
      break;
    }
    case TARGET_FIX:
    case TARGET_VARIABLE:
    {
      result += ACE_TEXT_ALWAYS_CHAR("#: ");
      if (target_in.range.typeDice == RPG_DICE_DIETYPE_INVALID)
      {
        converter << target_in.value;
        result += converter.str();
      } // end IF
      else
        result += RPG_Dice_Common_Tools::rollToString(target_in.range).c_str();
      if (target_in.rangeIsInHD)
        result += ACE_TEXT_ALWAYS_CHAR(" HD");
      if (target_in.levelIncrement)
      {
        result += ACE_TEXT_ALWAYS_CHAR(" + ");
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << ACE_static_cast(unsigned int, target_in.levelIncrement);
        result += converter.str();
        result += ACE_TEXT_ALWAYS_CHAR(" / [casterLevel]");
      } // end IF
      result += ACE_TEXT_ALWAYS_CHAR("\n");

      break;
    }
    case TARGET_AREA:
    {
      result += ACE_TEXT_ALWAYS_CHAR("area: ");
      result += RPG_Common_AreaOfEffectHelper::RPG_Common_AreaOfEffectToString(target_in.area);
      result += ACE_TEXT_ALWAYS_CHAR("\n");
      if (target_in.area == AREA_SPHERE)
      {
        result += ACE_TEXT_ALWAYS_CHAR("radius: ");
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << ACE_static_cast(unsigned int, target_in.radius);
        result += converter.str();
        result += ACE_TEXT_ALWAYS_CHAR(" ft\n");
        if (target_in.height)
        {
          result += ACE_TEXT_ALWAYS_CHAR("height: ");
          converter.str(ACE_TEXT_ALWAYS_CHAR(""));
          converter << ACE_static_cast(unsigned int, target_in.height);
          result += converter.str();
          result += ACE_TEXT_ALWAYS_CHAR(" ft\n");
        } // end IF
      } // end IF

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid target \"%s\" --> check implementation !, returning\n"),
                 RPG_Magic_Spell_TargetHelper::RPG_Magic_Spell_TargetToString(target_in.type).c_str()));

      break;
    }
  } // end SWITCH

  return result;
}

const std::string RPG_Magic_Common_Tools::spellDurationToString(const RPG_Magic_Spell_DurationProperties& duration_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Common_Tools::spellDurationToString"));

  std::string result = RPG_Magic_Spell_DurationHelper::RPG_Magic_Spell_DurationToString(duration_in.type);
  if ((duration_in.period.typeDice != RPG_DICE_DIETYPE_INVALID) ||
      ((duration_in.duration != 0) || (duration_in.levelIncrement != 0)))
  {
    result += ACE_TEXT_ALWAYS_CHAR(", ");
    if (duration_in.period.typeDice != RPG_DICE_DIETYPE_INVALID)
    {
      result += RPG_Dice_Common_Tools::rollToString(duration_in.period);
    } // end IF
    else
    {
      std::stringstream converter;
      if (duration_in.base)
      {
        converter << duration_in.base;
        result += converter.str();
        result += ACE_TEXT_ALWAYS_CHAR(" rd(s)");
      } // end IF
      if (duration_in.duration)
      {
        result += ACE_TEXT_ALWAYS_CHAR(" + ");
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << duration_in.duration;
        result += ACE_TEXT_ALWAYS_CHAR(" / [casterLevel");
        result += converter.str();
        if (duration_in.levelIncrement)
        {
          converter.str(ACE_TEXT_ALWAYS_CHAR(""));
          converter << ACE_static_cast(unsigned int, duration_in.levelIncrement);
          result += ACE_TEXT_ALWAYS_CHAR("/");
          result += converter.str();
          if (duration_in.levelIncrementMax)
          {
            converter.str(ACE_TEXT_ALWAYS_CHAR(""));
            converter << ACE_static_cast(unsigned int, duration_in.levelIncrementMax);
            result += ACE_TEXT_ALWAYS_CHAR(" (max: ");
            result += converter.str();
            result += ACE_TEXT_ALWAYS_CHAR("th)");
          }
        } // end IF
        result += ACE_TEXT_ALWAYS_CHAR("]");
      } // end IF
    } // end ELSE

    result += ACE_TEXT_ALWAYS_CHAR(" rd(s)");
  } // end IF
  if (duration_in.dismissible)
    result += ACE_TEXT_ALWAYS_CHAR(", dismissible");

  return result;
}

const std::string RPG_Magic_Common_Tools::preconditionsToString(const RPG_Magic_Spell_PreconditionList_t& preconditions_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Common_Tools::preconditionsToString"));

  std::string result;
  std::stringstream converter;
  for (RPG_Magic_Spell_PreconditionListIterator_t iterator = preconditions_in.begin();
       iterator != preconditions_in.end();
       iterator++)
  {
    result += RPG_Magic_Spell_PreconditionHelper::RPG_Magic_Spell_PreconditionToString((*iterator).type);
    switch ((*iterator).type)
    {
      case PRECONDITION_ANIMAL:
      case PRECONDITION_MANUFACTURED:
      case PRECONDITION_OBJECT:
      case PRECONDITION_RANGED_TOUCH_ATTACK:
      {
        break;
      }
      case PRECONDITION_ATTRIBUTE_MAX:
      case PRECONDITION_CONDITION:
      case PRECONDITION_HD_MAX:
      case PRECONDITION_SIZE_MAX:
      case PRECONDITION_SIZE_RELATIVE:
      {
        result += ACE_TEXT_ALWAYS_CHAR(": ");

        if ((*iterator).type == PRECONDITION_ATTRIBUTE_MAX)
        {
          result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString((*iterator).attribute);
          result += ACE_TEXT_ALWAYS_CHAR(" <= ");
          converter.str(ACE_TEXT_ALWAYS_CHAR(""));
          converter << (*iterator).value;
          result += converter.str();
        } // end IF
        else if ((*iterator).type == PRECONDITION_CONDITION)
          result += RPG_Character_ConditionHelper::RPG_Character_ConditionToString((*iterator).condition);
        else if ((*iterator).type == PRECONDITION_SIZE_MAX)
          result += RPG_Character_SizeHelper::RPG_Character_SizeToString((*iterator).size);
        else
        {
          result += ACE_TEXT_ALWAYS_CHAR(" ");
          converter.str(ACE_TEXT_ALWAYS_CHAR(""));
          converter << (*iterator).value;
          result += converter.str();

          if ((*iterator).type == PRECONDITION_HD_MAX)
            result += ACE_TEXT_ALWAYS_CHAR(" HD ");
        } // end ELSE

        if ((*iterator).levelIncrement)
        {
          result += ACE_TEXT_ALWAYS_CHAR(" / [casterLevel");
          if ((*iterator).levelIncrement != 1)
          {
            result += ACE_TEXT_ALWAYS_CHAR("/");
            converter.str(ACE_TEXT_ALWAYS_CHAR(""));
            converter << ACE_static_cast(unsigned int, (*iterator).levelIncrement);
            result += converter.str();
          } // end IF
          result += ACE_TEXT_ALWAYS_CHAR("]");

          if ((*iterator).levelIncrementMax)
          {
            result += ACE_TEXT_ALWAYS_CHAR(" (max: ");
            converter.str(ACE_TEXT_ALWAYS_CHAR(""));
            converter << ACE_static_cast(unsigned int, (*iterator).levelIncrementMax);
            result += converter.str();
            result += ACE_TEXT_ALWAYS_CHAR("th)");
          } // end IF
        } // end IF
        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid precondition \"%s\" --> check implementation !, continuing\n"),
                   RPG_Magic_Spell_PreconditionHelper::RPG_Magic_Spell_PreconditionToString((*iterator).type).c_str()));

        break;
      }
    } // end SWITCH
    result += ACE_TEXT_ALWAYS_CHAR("\n");
  } // end FOR

  return result;
}

const std::string RPG_Magic_Common_Tools::effectsToString(const RPG_Magic_Spell_EffectList_t& effects_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Common_Tools::effectsToString"));

  std::string result;
  std::stringstream converter;
  for (RPG_Magic_Spell_EffectListIterator_t iterator = effects_in.begin();
       iterator != effects_in.end();
       iterator++)
  {
    result += RPG_Magic_Spell_EffectHelper::RPG_Magic_Spell_EffectToString((*iterator).type);
    if ((*iterator).base.value ||
        ((*iterator).base.range.typeDice != RPG_DICE_DIETYPE_INVALID))
    {
      result += ACE_TEXT_ALWAYS_CHAR(": ");
      if ((*iterator).base.range.typeDice == RPG_DICE_DIETYPE_INVALID)
      {
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << ACE_static_cast(int, (*iterator).base.value);
        result += converter.str();
      } // end IF
      else
      {
        result += RPG_Dice_Common_Tools::rollToString((*iterator).base.range);
      } // end ELSE
      if (((*iterator).levelIncrement.value) ||
          ((*iterator).levelIncrement.range.typeDice != RPG_DICE_DIETYPE_INVALID))
      {
        result += ACE_TEXT_ALWAYS_CHAR(" [+ ");
        if ((*iterator).levelIncrement.range.typeDice == RPG_DICE_DIETYPE_INVALID)
        {
          converter.str(ACE_TEXT_ALWAYS_CHAR(""));
          converter << ACE_static_cast(int, (*iterator).levelIncrement.value);
          result += converter.str();
        } // end IF
        else
        {
          result += RPG_Dice_Common_Tools::rollToString((*iterator).levelIncrement.range);
        } // end ELSE

        if ((*iterator).levelIncrementMax)
        {
          converter.str(ACE_TEXT_ALWAYS_CHAR(""));
          converter << ACE_static_cast(unsigned int, (*iterator).levelIncrementMax);
          result += ACE_TEXT_ALWAYS_CHAR(" / casterLevel (max: ");
          result += converter.str();
          result += ACE_TEXT_ALWAYS_CHAR("th)]");
        } // end IF
        else
          result += ACE_TEXT_ALWAYS_CHAR(" / casterLevel]");
      } // end IF
    } // end IF
    result += ACE_TEXT_ALWAYS_CHAR("\n");
    for (std::vector<RPG_Magic_CounterMeasure>::const_iterator iterator2 = (*iterator).counterMeasures.begin();
         iterator2 != (*iterator).counterMeasures.end();
         iterator2++)
    {
      result += ACE_TEXT_ALWAYS_CHAR("counterMeasure: ");
      result += RPG_Common_CounterMeasureHelper::RPG_Common_CounterMeasureToString((*iterator2).type);
      switch ((*iterator2).type)
      {
        case COUNTERMEASURE_CHECK:
        {
          result += ACE_TEXT_ALWAYS_CHAR(": ");
          switch ((*iterator2).check.type.discriminator)
          {
            case RPG_Magic_CheckTypeUnion::SKILL:
            {
              result += RPG_Character_SkillHelper::RPG_Character_SkillToString((*iterator2).check.type.skill);

              break;
            }
            case RPG_Magic_CheckTypeUnion::ATTRIBUTE:
            {
              result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString((*iterator2).check.type.attribute);

              break;
            }
            case RPG_Magic_CheckTypeUnion::BASECHECKTYPEUNION:
            {
              switch ((*iterator2).check.type.basechecktypeunion.discriminator)
              {
                case RPG_Common_BaseCheckTypeUnion::CHECKTYPE:
                {
                  result += RPG_Common_CheckTypeHelper::RPG_Common_CheckTypeToString((*iterator2).check.type.basechecktypeunion.checktype);

                  break;
                }
                case RPG_Common_BaseCheckTypeUnion::SAVINGTHROW:
                {
                  result += RPG_Common_SavingThrowHelper::RPG_Common_SavingThrowToString((*iterator2).check.type.basechecktypeunion.savingthrow);

                  break;
                }
                default:
                {
                  // debug info
                  ACE_DEBUG((LM_ERROR,
                             ACE_TEXT("invalid RPG_Common_BaseCheckTypeUnion type: %d, continuing\n"),
                             (*iterator2).check.type.basechecktypeunion.discriminator));

                  break;
                }
              } // end SWITCH

              break;
            }
            default:
            {
              // debug info
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("invalid RPG_Magic_CheckTypeUnion type: %d, continuing\n"),
                         (*iterator2).check.type.discriminator));

              break;
            }
          } // end SWITCH
          result += ACE_TEXT_ALWAYS_CHAR(" (DC: ");
          converter.str(ACE_TEXT_ALWAYS_CHAR(""));
          converter << ACE_static_cast(unsigned int, (*iterator2).check.difficultyClass);
          result += converter.str();
          result += ACE_TEXT_ALWAYS_CHAR(")");

          break;
        }
        case COUNTERMEASURE_SPELL:
        {
          for (std::vector<RPG_Magic_SpellType>::const_iterator iterator3 = (*iterator2).spells.begin();
               iterator3 != (*iterator2).spells.end();
               iterator3++)
          {
            result += RPG_Magic_SpellTypeHelper::RPG_Magic_SpellTypeToString(*iterator3);
            result += ACE_TEXT_ALWAYS_CHAR("|");
          } // end FOR
          if (!(*iterator2).spells.empty())
          {
            result.erase(--(result.end()));
          } // end IF

          break;
        }
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid counterMeasure \"%s\" --> check implementation !, continuing\n"),
                     RPG_Common_CounterMeasureHelper::RPG_Common_CounterMeasureToString((*iterator2).type).c_str()));

          break;
        }
      } // end SWITCH
      result += ACE_TEXT_ALWAYS_CHAR("\n");
    } // end FOR
  } // end FOR

  return result;
}

void RPG_Magic_Common_Tools::updateSpellRange(RPG_Magic_Spell_RangeProperties& range_inout)
{
  ACE_TRACE(ACE_TEXT("RPG_Magic_Common_Tools::updateSpellRange"));

  switch (range_inout.effect)
  {
    case RANGEEFFECT_PERSONAL:
    case RANGEEFFECT_TOUCH:
    {
      range_inout.max = 0;
      range_inout.increment = 0;
//       range_inout.targets = 1;

      break;
    }
    case RANGEEFFECT_CLOSE:
    {
      range_inout.max = 25;
      range_inout.increment = 5; // per casterLevel/2

      break;
    }
    case RANGEEFFECT_MEDIUM:
    {
      range_inout.max = 100;
      range_inout.increment = 10; // per casterLevel

      break;
    }
    case RANGEEFFECT_LONG:
    {
      range_inout.max = 400;
      range_inout.increment = 40; // per casterLevel

      break;
    }
    case RANGEEFFECT_UNLIMITED:
    {
      range_inout.max = std::numeric_limits<unsigned int>::max();
      range_inout.increment = std::numeric_limits<unsigned int>::max();

      break;
    }
    case RANGEEFFECT_RANGED:
    {
      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid effect \"%s\" --> check implementation !, returning\n"),
                 RPG_Magic_Spell_RangeEffectHelper::RPG_Magic_Spell_RangeEffectToString(range_inout.effect).c_str()));

      break;
    }
  } // end SWITCH
}
