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

#include "rpg_common_tools.h"

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"
#include "rpg_common_environment_incl.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "Security.h"
#endif

#include <ace/OS.h>
#include <ace/Log_Msg.h>

#include <sstream>
#include <algorithm>
#include <locale>

// init statics
RPG_Common_CreatureMetaTypeToStringTable_t RPG_Common_CreatureMetaTypeHelper::myRPG_Common_CreatureMetaTypeToStringTable;
RPG_Common_CreatureSubTypeToStringTable_t RPG_Common_CreatureSubTypeHelper::myRPG_Common_CreatureSubTypeToStringTable;
RPG_Common_SubClassToStringTable_t RPG_Common_SubClassHelper::myRPG_Common_SubClassToStringTable;
RPG_Common_AttributeToStringTable_t RPG_Common_AttributeHelper::myRPG_Common_AttributeToStringTable;
RPG_Common_ConditionToStringTable_t RPG_Common_ConditionHelper::myRPG_Common_ConditionToStringTable;
RPG_Common_SizeToStringTable_t RPG_Common_SizeHelper::myRPG_Common_SizeToStringTable;
RPG_Common_SkillToStringTable_t RPG_Common_SkillHelper::myRPG_Common_SkillToStringTable;
RPG_Common_PhysicalDamageTypeToStringTable_t RPG_Common_PhysicalDamageTypeHelper::myRPG_Common_PhysicalDamageTypeToStringTable;
RPG_Common_ActionTypeToStringTable_t RPG_Common_ActionTypeHelper::myRPG_Common_ActionTypeToStringTable;
RPG_Common_AreaOfEffectToStringTable_t RPG_Common_AreaOfEffectHelper::myRPG_Common_AreaOfEffectToStringTable;
RPG_Common_EffectTypeToStringTable_t RPG_Common_EffectTypeHelper::myRPG_Common_EffectTypeToStringTable;
RPG_Common_CounterMeasureToStringTable_t RPG_Common_CounterMeasureHelper::myRPG_Common_CounterMeasureToStringTable;
RPG_Common_CheckTypeToStringTable_t RPG_Common_CheckTypeHelper::myRPG_Common_CheckTypeToStringTable;
RPG_Common_SavingThrowToStringTable_t RPG_Common_SavingThrowHelper::myRPG_Common_SavingThrowToStringTable;
RPG_Common_SaveReductionTypeToStringTable_t RPG_Common_SaveReductionTypeHelper::myRPG_Common_SaveReductionTypeToStringTable;
RPG_Common_CampToStringTable_t RPG_Common_CampHelper::myRPG_Common_CampToStringTable;

RPG_Common_PlaneToStringTable_t RPG_Common_PlaneHelper::myRPG_Common_PlaneToStringTable;
RPG_Common_TerrainToStringTable_t RPG_Common_TerrainHelper::myRPG_Common_TerrainToStringTable;
RPG_Common_ClimateToStringTable_t RPG_Common_ClimateHelper::myRPG_Common_ClimateToStringTable;
RPG_Common_TimeOfDayToStringTable_t RPG_Common_TimeOfDayHelper::myRPG_Common_TimeOfDayToStringTable;
RPG_Common_AmbientLightingToStringTable_t RPG_Common_AmbientLightingHelper::myRPG_Common_AmbientLightingToStringTable;

void
RPG_Common_Tools::initStringConversionTables()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Tools::initStringConversionTables"));

  RPG_Common_CreatureMetaTypeHelper::init();
  RPG_Common_CreatureSubTypeHelper::init();
  RPG_Common_SubClassHelper::init();
  RPG_Common_AttributeHelper::init();
  RPG_Common_ConditionHelper::init();
  RPG_Common_SizeHelper::init();
  RPG_Common_SkillHelper::init();
  RPG_Common_PhysicalDamageTypeHelper::init();
  RPG_Common_ActionTypeHelper::init();
  RPG_Common_AreaOfEffectHelper::init();
  RPG_Common_EffectTypeHelper::init();
  RPG_Common_CounterMeasureHelper::init();
  RPG_Common_CheckTypeHelper::init();
  RPG_Common_SavingThrowHelper::init();
  RPG_Common_SaveReductionTypeHelper::init();
  RPG_Common_CampHelper::init();

  RPG_Common_PlaneHelper::init();
  RPG_Common_TerrainHelper::init();
  RPG_Common_ClimateHelper::init();
  RPG_Common_TimeOfDayHelper::init();
  RPG_Common_AmbientLightingHelper::init();

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Common_Tools: initialized string conversion tables...\n")));
}

std::string
RPG_Common_Tools::creatureTypeToString(const RPG_Common_CreatureType& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Tools::creatureTypeToString"));

  std::string result = RPG_Common_CreatureMetaTypeHelper::RPG_Common_CreatureMetaTypeToString(type_in.metaType);
  if (!type_in.subTypes.empty())
  {
    result += ACE_TEXT_ALWAYS_CHAR(" / (");
    for (std::vector<RPG_Common_CreatureSubType>::const_iterator iterator = type_in.subTypes.begin();
         iterator != type_in.subTypes.end();
         iterator++)
    {
      result += RPG_Common_CreatureSubTypeHelper::RPG_Common_CreatureSubTypeToString(*iterator);
      result += ACE_TEXT_ALWAYS_CHAR("|");
    } // end FOR
    result.erase(--(result.end()));
    result += ACE_TEXT_ALWAYS_CHAR(")");
  } // end IF

  return result;
}

RPG_Common_Attribute
RPG_Common_Tools::savingThrowToAttribute(const RPG_Common_SavingThrow& save_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Tools::savingThrowToAttribute"));

  switch (save_in)
  {
    case SAVE_FORTITUDE:
    {
      return ATTRIBUTE_CONSTITUTION;
    }
    case SAVE_REFLEX:
    {
      return ATTRIBUTE_DEXTERITY;
    }
    case SAVE_WILL:
    {
      return ATTRIBUTE_WISDOM;
    }
    default:
    {
      // debug info
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("invalid saving throw type: \"%s\", aborting\n"),
                 RPG_Common_SavingThrowHelper::RPG_Common_SavingThrowToString(save_in).c_str()));

      break;
    }
  } // end SWITCH

  return RPG_COMMON_ATTRIBUTE_INVALID;
}

std::string
RPG_Common_Tools::savingThrowToString(const RPG_Common_SavingThrowCheck& save_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Tools::savingThrowToString"));

  std::string result;

  result += ACE_TEXT_ALWAYS_CHAR("type: ");
  result += RPG_Common_SavingThrowHelper::RPG_Common_SavingThrowToString(save_in.type);
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT_ALWAYS_CHAR("attribute: ");
  result += RPG_Common_AttributeHelper::RPG_Common_AttributeToString(save_in.attribute);
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT_ALWAYS_CHAR("DC: ");
  std::stringstream converter;
  converter << static_cast<unsigned int>(save_in.difficultyClass);
  result += converter.str();
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT_ALWAYS_CHAR("reduction: ");
  result += RPG_Common_SaveReductionTypeHelper::RPG_Common_SaveReductionTypeToString(save_in.reduction);
  result += ACE_TEXT_ALWAYS_CHAR("\n");

  return result;
}

std::string
RPG_Common_Tools::environmentToString(const RPG_Common_Environment& environment_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Tools::environmentToString"));

  std::string result;

  result += RPG_Common_TerrainHelper::RPG_Common_TerrainToString(environment_in.terrain);
  if (environment_in.climate != RPG_COMMON_CLIMATE_INVALID)
  {
    result += ACE_TEXT_ALWAYS_CHAR("|");
    result += RPG_Common_ClimateHelper::RPG_Common_ClimateToString(environment_in.climate);
  } // end IF

  return result;
}

RPG_Common_Plane
RPG_Common_Tools::terrainToPlane(const RPG_Common_Terrain& terrain_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Tools::terrainToPlane"));

  switch (terrain_in)
  {
    case TERRAIN_DESERTS:
    case TERRAIN_FORESTS:
    case TERRAIN_HILLS:
    case TERRAIN_MOUNTAINS:
    case TERRAIN_PLAINS:
    case TERRAIN_PLANE_MATERIAL_ANY:
    case TERRAIN_UNDER_GROUND:
    case TERRAIN_UNDER_WATER:
    {
      return PLANE_MATERIAL;
    }
    case TERRAIN_PLANE_TRANSITIVE_ASTRAL:
    case TERRAIN_PLANE_TRANSITIVE_ETHERAL:
    case TERRAIN_PLANE_TRANSITIVE_SHADOW:
    case TERRAIN_PLANE_TRANSITIVE_ANY:
    {
      return PLANE_TRANSITIVE;
    }
    case TERRAIN_PLANE_INNER_AIR:
    case TERRAIN_PLANE_INNER_EARTH:
    case TERRAIN_PLANE_INNER_FIRE:
    case TERRAIN_PLANE_INNER_WATER:
    case TERRAIN_PLANE_INNER_POSITIVE:
    case TERRAIN_PLANE_INNER_NEGATIVE:
    case TERRAIN_PLANE_INNER_ANY:
    {
      return PLANE_INNER;
    }
    case TERRAIN_PLANE_OUTER_LAWFUL_ANY:
    case TERRAIN_PLANE_OUTER_CHAOTIC_ANY:
    case TERRAIN_PLANE_OUTER_GOOD_ANY:
    case TERRAIN_PLANE_OUTER_EVIL_ANY:
    case TERRAIN_PLANE_OUTER_LAWFUL_GOOD:
    case TERRAIN_PLANE_OUTER_LAWFUL_EVIL:
    case TERRAIN_PLANE_OUTER_CHAOTIC_GOOD:
    case TERRAIN_PLANE_OUTER_CHAOTIC_EVIL:
    case TERRAIN_PLANE_OUTER_NEUTRAL:
    case TERRAIN_PLANE_OUTER_MILD_ANY:
    case TERRAIN_PLANE_OUTER_STRONG_ANY:
    case TERRAIN_PLANE_OUTER_ANY:
    {
      return PLANE_OUTER;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid terrain: \"%s\", aborting\n"),
                 RPG_Common_TerrainHelper::RPG_Common_TerrainToString(terrain_in).c_str()));

      break;
    }
  } // end SWITCH

  return RPG_COMMON_PLANE_INVALID;
}

bool
RPG_Common_Tools::match(const RPG_Common_Environment& environmentA_in,
                        const RPG_Common_Environment& environmentB_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Tools::match"));

  if ((environmentA_in.terrain == TERRAIN_ANY) ||
      (environmentB_in.terrain == TERRAIN_ANY))
    return true;

  // different planes don't match: determine planes
  RPG_Common_Plane planeA = RPG_Common_Tools::terrainToPlane(environmentA_in.terrain);
  RPG_Common_Plane planeB = RPG_Common_Tools::terrainToPlane(environmentB_in.terrain);
  if (planeA != planeB)
    return false;

  switch (planeA)
  {
    case PLANE_MATERIAL:
    {
      if ((environmentA_in.terrain == TERRAIN_PLANE_MATERIAL_ANY) ||
          (environmentB_in.terrain == TERRAIN_PLANE_MATERIAL_ANY))
        return true;

      // handle climate
      if ((environmentA_in.climate == CLIMATE_ANY) ||
          (environmentB_in.climate == CLIMATE_ANY))
        return true;

      return ((environmentA_in.terrain == environmentB_in.terrain) &&
              (environmentA_in.climate == environmentB_in.climate));
    }
    case PLANE_TRANSITIVE:
    {
      if ((environmentA_in.terrain == TERRAIN_PLANE_TRANSITIVE_ANY) ||
          (environmentB_in.terrain == TERRAIN_PLANE_TRANSITIVE_ANY))
        return true;

      return (environmentA_in.terrain == environmentB_in.terrain);
    }
    case PLANE_INNER:
    {
      if ((environmentA_in.terrain == TERRAIN_PLANE_INNER_ANY) ||
          (environmentB_in.terrain == TERRAIN_PLANE_INNER_ANY))
        return true;

      return (environmentA_in.terrain == environmentB_in.terrain);
    }
    case PLANE_OUTER:
    {
      if ((environmentA_in.terrain == TERRAIN_PLANE_OUTER_ANY) ||
          (environmentB_in.terrain == TERRAIN_PLANE_OUTER_ANY))
        return true;

      switch (environmentA_in.terrain)
      {
        case TERRAIN_PLANE_OUTER_LAWFUL_ANY:
        {
          return ((environmentB_in.terrain == TERRAIN_PLANE_OUTER_LAWFUL_GOOD) ||
                  (environmentB_in.terrain == TERRAIN_PLANE_OUTER_LAWFUL_EVIL));
        }
        case TERRAIN_PLANE_OUTER_CHAOTIC_ANY:
        {
          return ((environmentB_in.terrain == TERRAIN_PLANE_OUTER_CHAOTIC_GOOD) ||
                  (environmentB_in.terrain == TERRAIN_PLANE_OUTER_CHAOTIC_EVIL));
        }
        case TERRAIN_PLANE_OUTER_GOOD_ANY:
        {
          return ((environmentB_in.terrain == TERRAIN_PLANE_OUTER_LAWFUL_GOOD) ||
                  (environmentB_in.terrain == TERRAIN_PLANE_OUTER_CHAOTIC_GOOD));
        }
        case TERRAIN_PLANE_OUTER_EVIL_ANY:
        {
          return ((environmentB_in.terrain == TERRAIN_PLANE_OUTER_LAWFUL_EVIL) ||
                  (environmentB_in.terrain == TERRAIN_PLANE_OUTER_CHAOTIC_EVIL));
        }
        case TERRAIN_PLANE_OUTER_NEUTRAL:
        {
          return ((environmentB_in.terrain == TERRAIN_PLANE_OUTER_NEUTRAL) ||
                  (environmentB_in.terrain == TERRAIN_PLANE_OUTER_MILD_ANY) ||
                  (environmentB_in.terrain == TERRAIN_PLANE_OUTER_STRONG_ANY));
        }
        case TERRAIN_PLANE_OUTER_MILD_ANY:
        case TERRAIN_PLANE_OUTER_STRONG_ANY:
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid terrain: \"%s\", aborting\n"),
                     RPG_Common_TerrainHelper::RPG_Common_TerrainToString(environmentA_in.terrain).c_str()));

          break;
        }
      } // end SWITCH

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid plane: \"%s\", aborting\n"),
                 RPG_Common_PlaneHelper::RPG_Common_PlaneToString(planeA).c_str()));

      break;
    }
  } // end SWITCH

  return false;
}

signed char
RPG_Common_Tools::getSizeModifier(const RPG_Common_Size& size_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Tools::getSizeModifier"));

  // SIZE_FINE:       8
  // SIZE_DIMINUTIVE: 4
  // SIZE_TINY:       2
  // SIZE_SMALL:      1
  // SIZE_MEDIUM:     0
  // SIZE_LARGE:      -1
  // SIZE_HUGE:       -2
  // SIZE_GARGANTUAN: -4
  // SIZE_COLOSSAL:   -8
  // --> +/-2**(distance to medium - 1);
  if (size_in == SIZE_MEDIUM)
    return 0;

  signed char result = 1;
  result <<= ::abs(SIZE_MEDIUM - size_in - 1);

  //ACE_DEBUG((LM_DEBUG,
  //           ACE_TEXT("size (\"%s\") --> modifier: %d...\n"),
  //           RPG_Common_SizeHelper::RPG_Common_SizeToString(size_in).c_str(),
  //           static_cast<int>(((size_in > SIZE_MEDIUM) ? -result : result))));

  return ((size_in > SIZE_MEDIUM) ? -result : result);
}

float
RPG_Common_Tools::getSizeModifierLoad(const RPG_Common_Size& size_in,
                                      const bool& isBiped_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Tools::getSizeModifierLoad"));

  // SIZE_FINE:       1/8 | 1/4
  // SIZE_DIMINUTIVE: 1/4 | 1/2
  // SIZE_TINY:       1/2 | 3/4
  // SIZE_SMALL:      3/4 | 1
  // SIZE_MEDIUM:     1   | 3/2
  // SIZE_LARGE:      2   | 3
  // SIZE_HUGE:       4   | 6
  // SIZE_GARGANTUAN: 8   | 12
  // SIZE_COLOSSAL:   16  | 24
  switch (size_in)
  {
    case SIZE_FINE:
      return (isBiped_in ? 0.125F : 0.25F);
    case SIZE_DIMINUTIVE:
      return (isBiped_in ? 0.25F  : 0.5F);
    case SIZE_TINY:
      return (isBiped_in ? 0.5F   : 0.75F);
    case SIZE_SMALL:
      return (isBiped_in ? 0.75F  : 1.0F);
    case SIZE_MEDIUM:
      return (isBiped_in ? 1.0F   : 1.5F);
    case SIZE_LARGE:
      return (isBiped_in ? 2.0F   : 3.0F);
    case SIZE_HUGE:
      return (isBiped_in ? 4.0F   : 6.0F);
    case SIZE_GARGANTUAN:
      return (isBiped_in ? 8.0F   : 12.0F);
    case SIZE_COLOSSAL:
      return (isBiped_in ? 16.0F  : 24.0F);
    default:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("invalid size (was: \"%s\"), aborting\n"),
                 RPG_Common_SizeHelper::RPG_Common_SizeToString(size_in).c_str()));

      break;
    }
  } // end SWITCH

  return 0.0F;
}

unsigned char
RPG_Common_Tools::sizeToReach(const RPG_Common_Size& size_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Tools::sizeToReach"));

  switch (size_in)
  {
    case SIZE_FINE:
    case SIZE_DIMINUTIVE:
    case SIZE_TINY:
    case SIZE_SMALL:
    case SIZE_MEDIUM:
    {
      return 5;
    }
    case SIZE_LARGE:
    case SIZE_HUGE:
    case SIZE_GARGANTUAN:
    case SIZE_COLOSSAL:
    {
      return 10;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid size: \"%s\", aborting\n"),
                 RPG_Common_SizeHelper::RPG_Common_SizeToString(size_in).c_str()));

      break;
    }
  } // end SWITCH

  return 0;
}

unsigned char
RPG_Common_Tools::environment2Radius(const RPG_Common_Environment& environment_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Tools::environment2Radius"));

  if (environment_in.outdoors)
  {
    // consider time of day...
    if (environment_in.time == TIMEOFDAY_DAYTIME)
      return 120;
  } // end IF

  // indoors &&/|| nighttime
  // consider ambient lighting...
  switch (environment_in.lighting)
  {
    case AMBIENCE_BRIGHT:
      return 120;
    case AMBIENCE_SHADOWY:
    case AMBIENCE_DARKNESS:
      break; // --> depends on other factors...
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid (ambient) lighting: \"%s\", aborting\n"),
                 RPG_Common_AmbientLightingHelper::RPG_Common_AmbientLightingToString(environment_in.lighting).c_str()));

      break;
    }
  } // end SWITCH

  return 0;
}

std::string
RPG_Common_Tools::enumToString(const std::string& enumString_in,
                               const bool& chopPrefix_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Tools::enumToString"));

  std::string result = enumString_in;

  // *NOTE*: this converts "SUBCLASS_MONK" --> "Monk"

  // step1: chop off everything before (and including) the first underscore ('_')
  if (chopPrefix_in)
  {
    std::string::size_type underscore = result.find('_', 0);
// *TODO*: there is a linking problem using std::string::npos in MSVC 2010...
#if defined(_MSC_VER) && (_MSC_VER >= 1600) /* VS2010 or newer */
    if (underscore != -1)
#else
    if (underscore != std::string::npos)
#endif
    {
      std::string::iterator last = result.begin();
      std::advance(last, underscore + 1); // *NOTE*: move one past '_'
      result.erase(result.begin(),
                   last);
    } // end IF
  } // end IF

  // step2: convert everything past the first character to lower-case
  std::string::iterator first = result.begin();
  ACE_ASSERT(result.size() >= 1);
  std::advance(first, 1); // *NOTE*: skip first character
  std::transform(first,
                 result.end(),
                 first,
                 std::bind2nd(std::ptr_fun(&std::tolower<char>), std::locale("")));

  return result;
}

bool
RPG_Common_Tools::period2String(const ACE_Time_Value& period_in,
                                std::string& timeString_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Tools::period2String"));

  // init return value(s)
  timeString_out.resize(0);

  // extract hours and minutes...
  ACE_Time_Value temp = period_in;
  int hours = static_cast<int>(temp.sec()) / (60 * 60);
  temp.sec(temp.sec() % (60 * 60));

  int minutes = static_cast<int>(temp.sec()) / 60;
  temp.sec(temp.sec() % 60);

  char time_string[RPG_COMMON_MAX_TIMESTAMP_STRING_LENGTH];
  // *TODO*: rewrite this in C++...
  if (ACE_OS::snprintf(time_string,
                      sizeof(time_string),
                      ACE_TEXT_ALWAYS_CHAR("%d:%d:%d.%d"),
                      hours,
                      minutes,
                      static_cast<int>(temp.sec()),
                      static_cast<int>(temp.usec())) < 0)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::snprintf(): \"%m\", aborting\n")));

    return false;
  } // end IF

  timeString_out = time_string;

  return true;
}

bool
RPG_Common_Tools::isLinux()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Tools::isLinux"));

  // get system information
  ACE_utsname name;
  if (ACE_OS::uname(&name) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::uname(): \"%m\", aborting\n")));

    return false;
  } // end IF

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("local system info: %s (%s), %s %s, %s\n"),
//              name.nodename,
//              name.machine,
//              name.sysname,
//              name.release,
//              name.version));

  std::string kernel(name.sysname);
  return (kernel.find(ACE_TEXT_ALWAYS_CHAR("Linux"), 0) == 0);
}

bool
RPG_Common_Tools::getUserName(std::string& username_out,
                              std::string& realname_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Tools::getUserName"));

  // init return value(s)
  username_out.clear();
  realname_out.clear();

  char user_name[ACE_MAX_USERID];
  if (ACE_OS::cuserid(user_name, ACE_MAX_USERID) == NULL)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::cuserid(): \"%m\", aborting\n")));

    return false;
  } // end IF
  username_out = user_name;

#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  char pw_buf[BUFSIZ];
  struct passwd pw_struct;
  struct passwd* result = NULL;
  if (ACE_OS::getpwnam_r(user_name,
                         &pw_struct,
                         pw_buf,
                         sizeof(pw_buf),
						             &result) ||
	  (result == NULL))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::getpwnam_r(): \"%m\", aborting\n")));

    return false;
  } // end IF
  realname_out = pw_struct.pw_gecos;
#else
  TCHAR infoBuf[BUFSIZ];
  DWORD  bufCharCount = BUFSIZ;
  if (!GetUserNameEx(NameDisplay,
                     infoBuf,
                     &bufCharCount))
    if (GetLastError() == ERROR_NONE_MAPPED)
      realname_out = username_out;
    else
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to GetUserNameEx(): \"%m\", aborting\n")));

      return false;
    } // end IF
#endif

  return true;
}

std::string
RPG_Common_Tools::getHostName()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_Tools::getHostName"));

  std::string result;

  ACE_TCHAR host_name[MAXHOSTNAMELEN];
  if (ACE_OS::hostname(host_name, sizeof(host_name)) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::hostname(): \"%m\", aborting\n")));
  } // end IF
  else
    result = host_name;

  return result;
}
