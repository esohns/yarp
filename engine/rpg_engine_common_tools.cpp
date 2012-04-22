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

#include "rpg_engine_common_tools.h"

#include "rpg_engine_defines.h"
#include "rpg_engine_incl.h"
#include "rpg_engine_XML_tree.h"

#include <rpg_graphics_defines.h>
#include <rpg_graphics_common_tools.h>

#include <rpg_map_parser_driver.h>
#include <rpg_map_common_tools.h>

#include <rpg_monster_common.h>
#include <rpg_monster_common_tools.h>
#include <rpg_monster_attackaction.h>
#include <rpg_monster_dictionary.h>

#include <rpg_player_defines.h>
#include <rpg_player_common_tools.h>

#include <rpg_combat_common_tools.h>

#include <rpg_character_common_tools.h>
#include <rpg_character_race_common_tools.h>
#include <rpg_character_class_common_tools.h>

#include <rpg_item_common.h>
#include <rpg_item_armor.h>
#include <rpg_item_weapon.h>
#include <rpg_item_instance_manager.h>
#include <rpg_item_dictionary.h>
#include <rpg_item_common_tools.h>

#include <rpg_magic_dictionary.h>
#include <rpg_magic_common_tools.h>

#include <rpg_common_macros.h>
#include <rpg_common_defines.h>
#include <rpg_common_attribute.h>
#include <rpg_common_tools.h>
#include <rpg_common_file_tools.h>
#include <rpg_common_xsderrorhandler.h>

#include <rpg_chance_common_tools.h>

#include <rpg_dice.h>
#include <rpg_dice_common_tools.h>

#include <ace/Log_Msg.h>

#include <fstream>
#include <string>
#include <math.h>

// init statics
RPG_Engine_CommandToStringTable_t RPG_Engine_CommandHelper::myRPG_Engine_CommandToStringTable;
RPG_Engine_EntityModeToStringTable_t RPG_Engine_EntityModeHelper::myRPG_Engine_EntityModeToStringTable;

void
RPG_Engine_Common_Tools::init(const std::string& magicDictionaryFile_in,
                              const std::string& itemDictionaryFile_in,
                              const std::string& monsterDictionaryFile_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Common_Tools::init"));

  // step1a: init randomization
  RPG_Dice::init();

  // step1b: init string conversion facilities
  RPG_Dice_Common_Tools::initStringConversionTables();
  RPG_Common_Tools::initStringConversionTables();
  RPG_Item_Common_Tools::initStringConversionTables();
  RPG_Combat_Common_Tools::initStringConversionTables();
  RPG_Monster_Common_Tools::initStringConversionTables();
  // step1c: ...and other static data
  RPG_Magic_Common_Tools::init();
  RPG_Character_Common_Tools::init();

  RPG_Engine_CommandHelper::init();
  RPG_Engine_EntityModeHelper::init();

  // step1c: init dictionaries
  // step1ca: init magic dictionary
  if (!magicDictionaryFile_in.empty())
  {
    try
    {
      RPG_MAGIC_DICTIONARY_SINGLETON::instance()->init(magicDictionaryFile_in
#ifdef _DEBUG
                                                       ,true
#else
                                                       ,false
#endif
                                                       );
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Magic_Dictionary::init, returning\n")));

      return;
    }
  } // end IF

  // step1cb: init item dictionary
  if (!itemDictionaryFile_in.empty())
  {
    try
    {
      RPG_ITEM_DICTIONARY_SINGLETON::instance()->init(itemDictionaryFile_in
#ifdef _DEBUG
                                                      ,true
#else
                                                      ,false
#endif
        );
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Item_Dictionary::init, returning\n")));

      return;
    }
  } // end IF

  // step1cc: init monster dictionary
  if (!monsterDictionaryFile_in.empty())
  {
    try
    {
      RPG_MONSTER_DICTIONARY_SINGLETON::instance()->init(monsterDictionaryFile_in
#ifdef _DEBUG
                                                         ,true
#else
                                                         ,false
#endif
        );
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Character_Monster_Dictionary::init, returning\n")));

      return;
    }
  } // end IF
}

RPG_Engine_Entity
RPG_Engine_Common_Tools::loadEntity(const std::string& filename_in,
                                    const std::string& schemaRepository_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Common_Tools::loadEntity"));

  RPG_Engine_Entity result;
  result.character = NULL;
  result.position = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                   std::numeric_limits<unsigned int>::max());
//  result.actions.clear();
//  result.sprite.clear();
  result.is_spawned = false;

  // sanity check(s)
  if (!RPG_Common_File_Tools::isReadable(filename_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_File_Tools::isReadable(\"%s\"), aborting\n"),
               filename_in.c_str()));

    return result;
  } // end IF

  // step1: load player character
  std::ifstream ifs;
  ifs.exceptions(std::ifstream::badbit | std::ifstream::failbit);
//   ::xml_schema::flags = ::xml_schema::flags::dont_validate;
  ::xml_schema::flags flags = 0;
  ::xml_schema::properties props;
  std::string base_path;
  // *NOTE*: use the working directory as a fallback...
  if (schemaRepository_in.empty())
    base_path = RPG_Common_File_Tools::getWorkingDirectory();
  else
  {
    // sanity check(s)
    if (!RPG_Common_File_Tools::isDirectory(schemaRepository_in))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Common_File_Tools::isDirectory(\"%s\"), aborting\n"),
                 schemaRepository_in.c_str()));

      return result;
    } // end IF

    base_path = schemaRepository_in;
  } // end ELSE
  std::string schemaFile = base_path;
  schemaFile += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schemaFile += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_SCHEMA_FILE);
  // sanity check(s)
  if (!RPG_Common_File_Tools::isReadable(schemaFile))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_File_Tools::isReadable(\"%s\"), aborting\n"),
               schemaFile.c_str()));

    return result;
  } // end IF

  props.schema_location(ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_XML_TARGET_NAMESPACE),
                        schemaFile);
//   props.no_namespace_schema_location(RPG_CHARACTER_PLAYER_SCHEMA_FILE);
//   props.schema_location("http://www.w3.org/XML/1998/namespace", "xml.xsd");

  std::auto_ptr<RPG_Player_PlayerXML_XMLTree_Type> character_player_p;
  std::auto_ptr<RPG_Engine_Player_XMLTree_Type> engine_player_p;
  try
  {
    ifs.open(filename_in.c_str(),
             std::ios_base::in);

    engine_player_p = ::engine_player_t(ifs,
                                        RPG_XSDErrorHandler,
                                        flags,
                                        props);

    ifs.close();
  }
  catch (std::ifstream::failure const& exception)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Engine_Common_Tools::loadEntity(\"%s\"): exception occurred: \"%s\", aborting\n"),
               filename_in.c_str(),
               exception.what()));

    return result;
  }
  catch (::xml_schema::parsing const& exception)
  {
    std::ostringstream converter;
    converter << exception;
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Engine_Common_Tools::loadEntity(\"%s\"): exception occurred: \"%s\", aborting\n"),
               filename_in.c_str(),
               converter.str().c_str()));

    return result;
  }
  catch (::xml_schema::exception const& exception)
  {
    ACE_UNUSED_ARG(exception);

    // *NOTE*: maybe this was a CHARACTER profile (expected ENTITY profile)
    // --> try parsing that instead...
    ::xml_schema::properties props_alt;
    schemaFile = base_path;
    schemaFile += ACE_DIRECTORY_SEPARATOR_CHAR_A;
    schemaFile += ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_SCHEMA_FILE);
    // sanity check(s)
    if (!RPG_Common_File_Tools::isReadable(schemaFile))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Common_File_Tools::isReadable(\"%s\"), aborting\n"),
                 schemaFile.c_str()));

      return result;
    } // end IF
    props_alt.schema_location(ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_XML_TARGET_NAMESPACE),
                              schemaFile);
    try
    {
      // reset read pointer
      ifs.clear();
      ifs.seekg(0, std::ios::beg);

      character_player_p = ::player_t(ifs,
                                      RPG_XSDErrorHandler,
                                      flags,
                                      props_alt);
    }
    catch (std::ifstream::failure const& exception)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("RPG_Engine_Common_Tools::loadEntity(\"%s\"): exception occurred: \"%s\", aborting\n"),
                 filename_in.c_str(),
                 exception.what()));

      return result;
    }
    catch (::xml_schema::parsing const& exception)
    {
      std::ostringstream converter;
      converter << exception;
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("RPG_Engine_Common_Tools::loadEntity(\"%s\"): exception occurred: \"%s\", aborting\n"),
                 filename_in.c_str(),
                 converter.str().c_str()));

      return result;
    }
    catch (::xml_schema::exception const& exception)
    {
      std::ostringstream converter;
      converter << exception;
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("RPG_Engine_Common_Tools::loadEntity(\"%s\"): exception occurred: \"%s\", aborting\n"),
                 filename_in.c_str(),
                 converter.str().c_str()));

      return result;
    }

    // OK ! --> proceed
    ifs.close();
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Engine_Common_Tools::loadEntity(\"%s\"): exception occurred, aborting\n"),
               filename_in.c_str()));

    return result;
  }
  ACE_ASSERT(character_player_p.get() || engine_player_p.get());
  RPG_Player_PlayerXML_XMLTree_Type* player_p = (engine_player_p.get() ? engine_player_p.get() : character_player_p.get());
  ACE_ASSERT(player_p);

  result.character = RPG_Player_Common_Tools::playerXMLToPlayer(*player_p);
  ACE_ASSERT(result.character);

  // step2: load player position/modes/sprite ?
  if (engine_player_p.get())
  {
    result.position = std::make_pair(engine_player_p->position().x(),
                                     engine_player_p->position().y());
    for (RPG_Engine_Player_XMLTree_Type::mode_const_iterator iterator = engine_player_p->mode().begin();
         iterator != engine_player_p->mode().end();
         iterator++)
      result.modes.insert(RPG_Engine_EntityModeHelper::stringToRPG_Engine_EntityMode(*iterator));
    result.sprite = RPG_Graphics_SpriteHelper::stringToRPG_Graphics_Sprite(engine_player_p->sprite());
  } // end IF

  return result;
}

bool
RPG_Engine_Common_Tools::saveEntity(const RPG_Engine_Entity& entity_in,
                                    const std::string& filename_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Common_Tools::saveEntity"));

  // sanity check(s)
  ACE_ASSERT(entity_in.character);
  if (RPG_Common_File_Tools::isReadable(filename_in) ||
      !entity_in.character->isPlayerCharacter())
  {
    // *TODO*: warn user ?
//     if (!RPG_Common_File_Tools::deleteFile(filename_in))
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to RPG_Common_File_Tools::deleteFile(\"%s\"), aborting\n"),
//                  filename_in.c_str()));
    //
//       return false;
//     } // end IF
  } // end IF

  RPG_Player* player_p = dynamic_cast<RPG_Player*>(entity_in.character);
  ACE_ASSERT(player_p);

  std::ofstream ofs;
  ofs.exceptions(std::ofstream::badbit | std::ofstream::failbit);
  ::xml_schema::namespace_infomap map;
  map[""].name = ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_XML_TARGET_NAMESPACE);
  map[""].schema = ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_SCHEMA_FILE);
  std::string character_set(ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_XML_SCHEMA_CHARSET));
  //   ::xml_schema::flags = ::xml_schema::flags::dont_validate;
  ::xml_schema::flags flags = 0;

  RPG_Player_PlayerXML_XMLTree_Type* player_xml_p = RPG_Player_Common_Tools::playerToPlayerXML(*player_p);
  ACE_ASSERT(player_xml_p);
  RPG_Engine_Player_XMLTree_Type* entity_model = RPG_Engine_Common_Tools::playerXMLToEntityXML(*player_xml_p);
  ACE_ASSERT(entity_model);

  RPG_Map_Position_XMLTree_Type position(entity_in.position.first,
                                         entity_in.position.second);
  entity_model->position(position);
  RPG_Engine_Player_XMLTree_Type::mode_sequence modes;
  for (RPG_Engine_EntityModeConstIterator_t iterator = entity_in.modes.begin();
       iterator != entity_in.modes.end();
       iterator++)
    modes.push_back(RPG_Engine_EntityMode_XMLTree_Type(static_cast<RPG_Engine_EntityMode_XMLTree_Type::value>(*iterator)));
  entity_model->mode(modes);
  entity_model->sprite(RPG_Graphics_SpriteHelper::RPG_Graphics_SpriteToString(entity_in.sprite));

  try
  {
    ofs.open(filename_in.c_str(),
             (std::ios_base::out | std::ios_base::trunc));

    ::engine_player_t(ofs,
                      *entity_model,
                      map,
                      character_set,
                      flags);

    ofs.close();
  }
  catch (const std::ofstream::failure& exception)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("\"%s\": unable to open or write error, aborting\n"),
               filename_in.c_str()));

    // clean up
    delete player_xml_p;
    delete entity_model;

    throw exception;
  }
  catch (const ::xml_schema::serialization& exception)
  {
    std::ostringstream converter;
    converter << exception;
    std::string text = converter.str();
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Engine_Common_Tools::saveEntity(\"%s\"): exception occurred: \"%s\", aborting\n"),
               filename_in.c_str(),
               text.c_str()));

    // clean up
    delete player_xml_p;
    delete entity_model;

    throw exception;
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Engine_Common_Tools::saveEntity(\"%s\"): exception occurred, aborting\n"),
               filename_in.c_str()));

    // clean up
    delete player_xml_p;
    delete entity_model;

    throw;
  }

  // clean up
  delete player_xml_p;
  delete entity_model;

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("saved entity \"%s\" to file: \"%s\"\n"),
             entity_in.character->getName().c_str(),
             filename_in.c_str()));

  return true;
}

RPG_Engine_Entity
RPG_Engine_Common_Tools::createEntity()
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Common_Tools::createEntity"));

  RPG_Engine_Entity result;
  result.character = NULL;
  result.position = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                   std::numeric_limits<unsigned int>::max());
  //result.modes();
  //result.actions();
  result.sprite = RPG_ENGINE_DEF_ENTITY_SPRITE;
  result.is_spawned = false;
  result.activation_timer = -1;

  // generate player
  RPG_Player* player_p = RPG_Player::create();
  ACE_ASSERT(player_p);
  result.character = player_p;

  return result;
}

RPG_Engine_Entity
RPG_Engine_Common_Tools::createEntity(const std::string& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Common_Tools::createEntity"));

  RPG_Engine_Entity result;
  result.character = NULL;
  result.position = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                   std::numeric_limits<unsigned int>::max());
  //result.modes();
  //result.actions();
  result.sprite = RPG_GRAPHICS_SPRITE_INVALID;
  result.is_spawned = false;
  result.activation_timer = -1;

  RPG_Monster_Properties properties = RPG_MONSTER_DICTIONARY_SINGLETON::instance()->getProperties(type_in);
  // compute individual hitpoints
  RPG_Dice_RollResult_t result_values;
  RPG_Dice::simulateRoll(properties.hitDice,
                         1,
                         result_values);
  RPG_Character_Conditions_t condition;
  condition.insert(CONDITION_NORMAL);
  // *TODO*: define monster abilities, spells, wealth, inventory (i.e. treasure)...
  RPG_Character_Abilities_t abilities;
  RPG_Magic_SpellTypes_t known_spells;
  unsigned int wealth = 0;
  RPG_Magic_Spells_t spells;
  RPG_Item_List_t items;

  result.character = new(std::nothrow) RPG_Monster(// base attributes
                                                   type_in,
                                                   properties.type,
                                                   properties.alignment,
                                                   properties.attributes,
                                                   properties.skills,
                                                   properties.feats,
                                                   abilities,
                                                   properties.size,
                                                   result_values.front(),
                                                   known_spells,
                                                   // current status
                                                   condition,
                                                   result_values.front(),
                                                   wealth,
                                                   spells,
                                                   items,
                                                   false);
  if (!result.character)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory, aborting\n")));

    return result;
  } // end IF

  return result;
}

std::string
RPG_Engine_Common_Tools::info(const RPG_Engine_Entity& entity_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Common_Tools::info"));

  // sanity check(s)
  ACE_ASSERT(entity_in.character);

  std::string result;

  result += ACE_TEXT("Entity: ");
  result += entity_in.character->getName();
  result += ACE_TEXT("\n===============\n");

  std::ostringstream converter;
  result += ACE_TEXT("position: [");
  converter.str(ACE_TEXT(""));
  converter << entity_in.position.first;
  result += converter.str();
  result += ACE_TEXT(",");
  converter.str(ACE_TEXT(""));
  converter << entity_in.position.second;
  result += converter.str();
  result += ACE_TEXT("]\n");

  result += ACE_TEXT("mode(s) [");
  converter.str(ACE_TEXT(""));
  converter << entity_in.modes.size();
  result += converter.str();
  result += ACE_TEXT("]:\n");
  unsigned int index = 0;
  for (RPG_Engine_EntityModeConstIterator_t iterator = entity_in.modes.begin();
       iterator != entity_in.modes.end();
       iterator++, index++)
  {
    result += ACE_TEXT("[");
    converter.str(ACE_TEXT(""));
    converter << index;
    result += converter.str();
    result += ACE_TEXT("]: ");
    result += RPG_Engine_EntityModeHelper::RPG_Engine_EntityModeToString(*iterator);
    result += ACE_TEXT("\n");
  } // end FOR
  result += ACE_TEXT("\\end mode(s)\n");

  result += ACE_TEXT("action(s) [");
  converter.str(ACE_TEXT(""));
  converter << entity_in.actions.size();
  result += converter.str();
  result += ACE_TEXT("]:\n");
  index = 0;
  for (RPG_Engine_ActionsConstIterator_t iterator = entity_in.actions.begin();
       iterator != entity_in.actions.end();
       iterator++, index++)
  {
    result += ACE_TEXT("[");
    converter.str(ACE_TEXT(""));
    converter << index;
    result += converter.str();
    result += ACE_TEXT("]: ");
    result += RPG_Engine_CommandHelper::RPG_Engine_CommandToString((*iterator).command);
    result += ACE_TEXT("\n");
  } // end FOR
  result += ACE_TEXT("\\end action(s)\n");

  result += ACE_TEXT("sprite: \"");
  result += entity_in.sprite;
  result += ACE_TEXT("\"\n");
  result += ACE_TEXT("\\end Entity\n");

  result += ACE_TEXT("spawned: ");
  result += (entity_in.is_spawned ? ACE_TEXT("yes") : ACE_TEXT("no"));
  result += ACE_TEXT("\n");
  result += ACE_TEXT("\\end Entity\n");

  return result;
}

RPG_Item_List_t
RPG_Engine_Common_Tools::generateStandardItems(const RPG_Common_SubClass& subClass_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Common_Tools::generateStandardItems"));

  RPG_Item_List_t result;

  RPG_Item_Instance_Base* current = NULL;
  try
  {
    switch (subClass_in)
    {
      case SUBCLASS_FIGHTER:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
            ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_MAIL_SPLINT);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_SHIELD_HEAVY_WOODEN);
        ACE_ASSERT(current);
        result.insert(current->getID());

        break;
      }
      case SUBCLASS_PALADIN:
//       case SUBCLASS_WARLORD:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
            ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_PLATE_FULL);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_SHIELD_HEAVY_STEEL);
        ACE_ASSERT(current);
        result.insert(current->getID());

        break;
      }
      case SUBCLASS_RANGER:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
            ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
            RANGED_WEAPON_BOW_LONG);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_HIDE);
        ACE_ASSERT(current);
        result.insert(current->getID());

        // *TODO*: no arrows ?

        break;
      }
//       case SUBCLASS_BARBARIAN:
//       {
//         current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
//             ONE_HANDED_MELEE_WEAPON_SWORD_LONG);
//         ACE_ASSERT(current);
//         result.insert(current->getID());
//         current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
//             ARMOR_HIDE);
//         ACE_ASSERT(current);
//         result.insert(current->getID());
//
//         break;
//       }
      case SUBCLASS_WIZARD:
      case SUBCLASS_SORCERER:
//       case SUBCLASS_WARLOCK:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
            TWO_HANDED_MELEE_WEAPON_QUARTERSTAFF);
        ACE_ASSERT(current);
        result.insert(current->getID());

        break;
      }
      case SUBCLASS_CLERIC:
//       case SUBCLASS_AVENGER:
//       case SUBCLASS_INVOKER:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
            ONE_HANDED_MELEE_WEAPON_MACE_HEAVY);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_MAIL_CHAIN);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_SHIELD_HEAVY_WOODEN);
        ACE_ASSERT(current);
        result.insert(current->getID());

        break;
      }
      case SUBCLASS_DRUID:
//       case SUBCLASS_SHAMAN:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
            LIGHT_MELEE_WEAPON_SICKLE);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_HIDE);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_SHIELD_LIGHT_WOODEN);
        ACE_ASSERT(current);
        result.insert(current->getID());

        break;
      }
//       case SUBCLASS_MONK:
//       {
//         current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
//             TWO_HANDED_MELEE_WEAPON_QUARTERSTAFF);
//         ACE_ASSERT(current);
//         result.insert(current->getID());
//
//         break;
//       }
      case SUBCLASS_THIEF:
      case SUBCLASS_BARD:
      {
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
            LIGHT_MELEE_WEAPON_SWORD_SHORT);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_LEATHER);
        ACE_ASSERT(current);
        result.insert(current->getID());
        current = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
            ARMOR_SHIELD_LIGHT_STEEL);
        ACE_ASSERT(current);
        result.insert(current->getID());

        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid subclass \"%s\", aborting\n"),
                   RPG_Common_SubClassHelper::RPG_Common_SubClassToString(subClass_in).c_str()));

        break;
      }
    } // end SWITCH
  }
  catch (const std::bad_alloc& exception)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("generate_standard_items(): caught exception: \"%s\", aborting\n"),
               exception.what()));
  }

  return result;
}

bool
RPG_Engine_Common_Tools::isPartyHelpless(const RPG_Player_Party_t& party_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Common_Tools::isPartyHelpless"));

  unsigned int numDeadOrHelpless = 0;
  for (RPG_Player_PartyConstIterator_t iterator = party_in.begin();
       iterator != party_in.end();
       iterator++)
  {
    if (isCharacterHelpless(*iterator))
    {
      numDeadOrHelpless++;
    } // end IF
  } // end FOR

  return (numDeadOrHelpless == party_in.size());
}

bool
RPG_Engine_Common_Tools::areMonstersHelpless(const RPG_Monster_Groups_t& monsters_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Common_Tools::areMonstersHelpless"));

  unsigned int numHelplessGroups = 0;
  for (RPG_Monster_GroupsIterator_t iterator = monsters_in.begin();
       iterator != monsters_in.end();
       iterator++)
  {
    if (RPG_Engine_Common_Tools::isMonsterGroupHelpless(*iterator))
    {
      numHelplessGroups++;
    } // end IF
  } // end FOR

  return (numHelplessGroups == monsters_in.size());
}

void
RPG_Engine_Common_Tools::getCombatantSequence(const RPG_Player_Party_t& party_in,
                                              const RPG_Monster_Groups_t& monsters_in,
                                              RPG_Engine_CombatantSequence_t& battleSequence_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Common_Tools::getCombatantSequence"));

  // init result
  battleSequence_out.clear();

  // step0: throw everybody into a list
  RPG_Player_List_t listOfCombatants;
  for (RPG_Player_PartyConstIterator_t iterator = party_in.begin();
       iterator != party_in.end();
       iterator++)
    listOfCombatants.push_back(*iterator);
  for (RPG_Monster_GroupsIterator_t iterator = monsters_in.begin();
       iterator != monsters_in.end();
       iterator++)
    for (RPG_Monster_GroupIterator_t iterator2 = (*iterator).begin();
         iterator2 != (*iterator).end();
         iterator2++)
      listOfCombatants.push_back(*iterator2);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("sorting %d combatants...\n"),
             listOfCombatants.size()));

  // step1: go through the list and compute initiatives
  RPG_Engine_CombatSequenceList_t preliminarySequence;
   // make sure there are enough SLOTS for large armies !
   // ruleset says it should be a D_20, but if there are more than 20 combatants...
  // --> just as in RL - the conflict resolution algorithm could potentially run forever...
  RPG_Dice_DieType checkDie = D_20;
  bool num_slots_too_small = (listOfCombatants.size() > D_100);
  if (!num_slots_too_small)
  {
    while (static_cast<unsigned int>(checkDie) < listOfCombatants.size())
      checkDie++;
  } // end IF
  else
  {
    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("too many combatants: %d, trying alternate slot conflict resolution...\n"),
               listOfCombatants.size()));
  } // end ELSE
  RPG_Dice_RollResult_t result;
  for (RPG_Player_ListIterator_t iterator = listOfCombatants.begin();
       iterator != listOfCombatants.end();
       iterator++)
  {
    RPG_Engine_CombatantSequenceElement element = {0, 0, *iterator};
    // compute initiative: DEX check
    element.DEXModifier = RPG_Character_Common_Tools::getAttributeAbilityModifier((*iterator)->getAttribute(ATTRIBUTE_DEXTERITY));
    if (!num_slots_too_small)
    {
      element.initiative = RPG_Chance_Common_Tools::getCheck(element.DEXModifier,
                                                             checkDie);
    } // end IF
    else
    {
      // try another solution...
      result.clear();
      RPG_Dice::generateRandomNumbers(listOfCombatants.size(),
                                      1,
                                      result);
      element.initiative = result.front() + element.DEXModifier;
    } // end ELSE

    preliminarySequence.push_back(element);
  } // end FOR

  // step2: sort according to criteria: descending initiative / DEX modifier
  std::pair<RPG_Engine_CombatantSequenceIterator_t, bool> preliminarySequencePosition;
  RPG_Engine_CombatSequenceList_t conflicts;
  for (RPG_Engine_CombatSequenceListIterator_t iterator = preliminarySequence.begin();
       iterator != preliminarySequence.end();
       iterator++)
  {
    // make sure there is a PROPER sequence:
    // if the set already contains this value we must resolve the conflict (again)
    // *IMPORTANT NOTE*: this algorithm implements the notion of fairness as appropriate between two HUMAN/HUMAN-Monster actors,
    // i.e. we could have just re-rolled the current element until it doesn't clash. In a real-world situation (depending on the relevance of the CURRENT position) this would trigger discussions of WHO would re-roll...
    preliminarySequencePosition = battleSequence_out.insert(*iterator);
    if (preliminarySequencePosition.second == false)
    {
      // find conflicting element
      RPG_Engine_CombatantSequenceIterator_t iterator2 = battleSequence_out.find(*iterator);
      ACE_ASSERT(iterator2 != battleSequence_out.end());

      conflicts.push_back(*iterator);
      conflicts.push_back(*iterator2);

      // erase conflicting element from the preliminary sequence
      battleSequence_out.erase(iterator2);
    } // end IF
  } // end FOR

  // step3: resolve conflicts
  RPG_Engine_CombatantSequenceElement current_conflict;
  while (!conflicts.empty())
  {
//     // handle first conflict
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("resolving %d conflicts...\n"),
//                conflicts.size()));

    current_conflict = conflicts.front();
    // re-roll initiative
    // compute initiative: DEX check
    if (!num_slots_too_small)
    {
      current_conflict.initiative = RPG_Chance_Common_Tools::getCheck(current_conflict.DEXModifier,
                                                                      checkDie);
    } // end IF
    else
    {
      // try another solution...
      result.clear();
      RPG_Dice::generateRandomNumbers(listOfCombatants.size(),
                                      1,
                                      result);
      current_conflict.initiative = result.front() + current_conflict.DEXModifier;
    } // end ELSE

    // make sure there is a PROPER sequence:
    // if the set already contains this value we must resolve the conflict (again)
    // *IMPORTANT NOTE*: this algorithm implements the notion of fairness as appropriate between two HUMAN/HUMAN-Monster actors,
    // i.e. we could have just re-rolled the current element until it doesn't clash. In a real-world situation this
    // would trigger discussions of WHO would re-roll...
    preliminarySequencePosition = battleSequence_out.insert(current_conflict);
    if (preliminarySequencePosition.second == false)
    {
      // find conflicting element
      RPG_Engine_CombatantSequenceIterator_t iterator2 = battleSequence_out.find(current_conflict);
      ACE_ASSERT(iterator2 != battleSequence_out.end());

      conflicts.push_back(current_conflict);
      conflicts.push_back(*iterator2);

      // erase conflicting element from the preliminary sequence
      battleSequence_out.erase(iterator2);
    } // end IF

    conflicts.pop_front();
  } // end WHILE

  ACE_ASSERT(listOfCombatants.size() == battleSequence_out.size());
  // debug info
  int i = 1;
//   for (RPG_Engine_CombatantSequenceRIterator_t iterator = battleSequence_out.rbegin();
//        iterator != battleSequence_out.rend();
//        iterator++, i++)
  for (RPG_Engine_CombatantSequenceIterator_t iterator = battleSequence_out.begin();
       iterator != battleSequence_out.end();
       iterator++, i++)
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("combatant #%d: name: \"%s\", DEXModifier: %d, initiative: %d\n"),
               i,
               (*iterator).handle->getName().c_str(),
               (*iterator).DEXModifier,
               (*iterator).initiative));
}

void
RPG_Engine_Common_Tools::performCombatRound(const RPG_Combat_AttackSituation& attackSituation_in,
                                            const RPG_Combat_DefenseSituation& defenseSituation_in,
                                            const RPG_Engine_CombatantSequence_t& battleSequence_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Common_Tools::performCombatRound"));

  // everybody gets their turn
  bool isPlayer = false;
  RPG_Engine_CombatantSequenceIterator_t foeFinder;
  RPG_Dice_RollResult_t result;
  for (RPG_Engine_CombatantSequenceIterator_t iterator = battleSequence_in.begin();
       iterator != battleSequence_in.end();
       iterator++)
  {
    // sanity checks
    // step0a: determine whether this combatant is fit enough to fight
    if (isCharacterHelpless((*iterator).handle))
    {
      // not fit enough...
      continue;
    } // end IF
    // step0b: determine whether any foes are still standing...
    if (!isValidFoeAvailable((*iterator).handle->isPlayerCharacter(),
                             battleSequence_in))
    {
      // nothing to do...
      break;
    } // end IF

    // step1: find (random) opponent
    // step1a: determine friend or foe
    isPlayer = (*iterator).handle->isPlayerCharacter();
    do
    {
      foeFinder = battleSequence_in.begin();
      result.clear();
      RPG_Dice::generateRandomNumbers(battleSequence_in.size(),
                                      1,
                                      result);
      std::advance(foeFinder, result.front() - 1);
    } while ((iterator == foeFinder) ||                                // dont't attack ourselves !
             ((*foeFinder).handle->isPlayerCharacter() == isPlayer) || // don't attack friends
             (isCharacterDisabled((*foeFinder).handle)));              // leave the disabled alone (...for now)

    // step2: attack foe !
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("\"%s\" (HP: %d/%d) attacks \"%s\" (HP: %d/%d)...\n"),
//                (*iterator).handle->getName().c_str(),
//                (*iterator).handle->getNumCurrentHitPoints(),
//                (*iterator).handle->getNumTotalHitPoints(),
//                (*foeFinder).handle->getName().c_str(),
//                (*foeFinder).handle->getNumCurrentHitPoints(),
//                (*foeFinder).handle->getNumTotalHitPoints()));

    // *TODO*: for now, we ignore range and movement
    attack((*iterator).handle,
           const_cast<RPG_Player_Base*>((*foeFinder).handle),
           attackSituation_in,
           defenseSituation_in,
           true,
           RPG_COMBAT_DEF_ADJACENT_DISTANCE);
  } // end FOR
}

bool
RPG_Engine_Common_Tools::isMonsterGroupHelpless(const RPG_Monster_Group_t& groupInstance_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Common_Tools::isMonsterGroupHelpless"));

  unsigned int numHelplessMonsters = 0;
  for (RPG_Monster_GroupIterator_t iterator = groupInstance_in.begin();
       iterator != groupInstance_in.end();
       iterator++)
    if (isCharacterHelpless(*iterator))
      numHelplessMonsters++;

  return (numHelplessMonsters == groupInstance_in.size());
}

bool
RPG_Engine_Common_Tools::isCharacterHelpless(const RPG_Player_Base* const character_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Common_Tools::isCharacterHelpless"));

  ACE_ASSERT(character_in);

  if ((character_in->hasCondition(CONDITION_PARALYZED)) || // spell, ...
      (character_in->hasCondition(CONDITION_HELD))      || // bound as per spell, ...
      (character_in->hasCondition(CONDITION_BOUND))     || // bound ase per rope, ...
      (character_in->hasCondition(CONDITION_SLEEPING))  || // natural, spell, ...
      (character_in->hasCondition(CONDITION_PETRIFIED)) || // turned to stone
      isCharacterDisabled(character_in))                   // disabled
    return true;

  return false;
}

bool
RPG_Engine_Common_Tools::isValidFoeAvailable(const bool& isMonsterAvailable_in,
                                             const RPG_Engine_CombatantSequence_t& battleSequence_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Common_Tools::isValidFoeAvailable"));

  for (RPG_Engine_CombatantSequenceIterator_t iterator = battleSequence_in.begin();
       iterator != battleSequence_in.end();
       iterator++)
  {
    if (isCharacterHelpless((*iterator).handle))
      continue;

    if (isMonsterAvailable_in && !((*iterator).handle->isPlayerCharacter()))
      return true;
    else if (!isMonsterAvailable_in && (*iterator).handle->isPlayerCharacter())
      return true;
  } // end FOR

  return false;
}

bool
RPG_Engine_Common_Tools::isCharacterDisabled(const RPG_Player_Base* const character_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Common_Tools::isCharacterDisabled"));

  ACE_ASSERT(character_in);

  if ((character_in->hasCondition(CONDITION_DEAD))        || // HP<-10
      (character_in->hasCondition(CONDITION_DYING))       || // -10<HP<0
      (character_in->hasCondition(CONDITION_STABLE))      || // -10<HP<0 && !DYING
      (character_in->hasCondition(CONDITION_UNCONSCIOUS)) || // -10<HP<0 && (DYING || STABLE)
      (character_in->hasCondition(CONDITION_DISABLED)))      // (HP==0) || (STABLE && !UNCONSCIOUS)
    return true;

  return false;
}

unsigned int
RPG_Engine_Common_Tools::numCompatibleMonsterAttackActions(const RPG_Combat_AttackForm& attackForm_in,
                                                           const RPG_Monster_AttackActions_t& actions_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Common_Tools::numCompatibleMonsterAttackActions"));

  unsigned int result = 0;

  switch (attackForm_in)
  {
    case ATTACKFORM_MELEE:
    {
      for (RPG_Monster_AttackActionsIterator_t iterator = actions_in.begin();
           iterator != actions_in.end();
           iterator++)
      {
        // *NOTE*: for this purpose, touch attacks count as melee attacks
        // if (and ONLY IF) it's not a ranged touch...
        if ((std::find((*iterator).attackForms.begin(),
                       (*iterator).attackForms.end(),
                       ATTACKFORM_MELEE) != (*iterator).attackForms.end()))
          result++;
        else if ((std::find((*iterator).attackForms.begin(),
                            (*iterator).attackForms.end(),
                            ATTACKFORM_TOUCH) != (*iterator).attackForms.end()) &&
                 (std::find((*iterator).attackForms.begin(),
                            (*iterator).attackForms.end(),
                            ATTACKFORM_RANGED) == (*iterator).attackForms.end()))
          result++;
      } // end FOR

      break;
    }
    default:
    {
      for (RPG_Monster_AttackActionsIterator_t iterator = actions_in.begin();
            iterator != actions_in.end();
            iterator++)
      {
        if (std::find((*iterator).attackForms.begin(),
                      (*iterator).attackForms.end(),
                      attackForm_in) != (*iterator).attackForms.end())
          result++;
      } // end FOR

      break;
    }
  } // end SWITCH

  return result;
}

bool
RPG_Engine_Common_Tools::isCompatibleMonsterAttackAction(const RPG_Combat_AttackForm& attackForm_in,
                                                         const RPG_Monster_AttackAction& action_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Common_Tools::isCompatibleMonsterAttackAction"));

  switch (attackForm_in)
  {
    case ATTACKFORM_MELEE:
    {
      if (std::find(action_in.attackForms.begin(),
                    action_in.attackForms.end(),
                    ATTACKFORM_MELEE) != action_in.attackForms.end())
        return true;

      // touch attacks are also melee attacks if (and ONLY IF) it's not a "ranged" touch...
      if ((std::find(action_in.attackForms.begin(),
                     action_in.attackForms.end(),
                     ATTACKFORM_TOUCH) != action_in.attackForms.end()) &&
          (std::find(action_in.attackForms.begin(),
                     action_in.attackForms.end(),
                     ATTACKFORM_RANGED) == action_in.attackForms.end()))
        return true;

      return false;
    }
    default:
    {
      return (std::find(action_in.attackForms.begin(),
                        action_in.attackForms.end(),
                        attackForm_in) != action_in.attackForms.end());
    }
  } // end SWITCH
}

void
RPG_Engine_Common_Tools::attack(const RPG_Player_Base* const attacker_in,
                                RPG_Player_Base* const target_inout,
                                const RPG_Combat_AttackSituation& attackSituation_in,
                                const RPG_Combat_DefenseSituation& defenseSituation_in,
                                const bool& isFullRoundAction_in,
                                const unsigned short& distance_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Common_Tools::attackFoe"));

  // sanity check
  ACE_ASSERT(attacker_in && target_inout);

  RPG_Dice_Roll roll;
  roll.numDice = 1;
  roll.typeDice = D_20;
  roll.modifier = 0;
  RPG_Dice_RollResult_t result;
  int attack_roll = 0;
  int currentAttackBonus = 0;
  RPG_Item_WeaponType weapon_type = RPG_ITEM_WEAPONTYPE_INVALID;
  RPG_Item_WeaponProperties weapon_properties;
  bool is_threat = false;
  bool is_critical_hit = false;
  RPG_Common_Attribute attribute = RPG_COMMON_ATTRIBUTE_INVALID;
  RPG_Combat_AttackForm attackForm = RPG_COMBAT_ATTACKFORM_INVALID;
  int targetArmorClass = 0;
  float STR_factor = 1.0;
  RPG_Combat_Damage damage;
  RPG_Combat_DamageElement damage_element;
  RPG_Common_PhysicalDamageList_t physicalDamageTypeList;
  if (attacker_in->isPlayerCharacter())
  {
    // attack roll: D_20 + attack bonus + other modifiers
    // step1: compute attack bonus(ses) --> number of attacks
    // *TODO*: consider multi-weapon/offhand attacks...
    const RPG_Player_Player_Base* const player_base = dynamic_cast<const RPG_Player_Player_Base* const>(attacker_in);
    ACE_ASSERT(player_base);
    // attack bonus: [base attack bonus + STR/DEX modifier + size modifier] (+ range penalty)
    // *TODO*: consider that a creature with FEAT_WEAPON_FINESSE may use its DEX modifier for melee attacks...
    attribute = ATTRIBUTE_STRENGTH;
    // consider that the player may (temporarily) have a bigger reach...
    attackForm = (distance_in > RPG_Common_Tools::sizeToReach(player_base->getSize()) ? ATTACKFORM_RANGED : ATTACKFORM_MELEE);
    if (attackForm == ATTACKFORM_RANGED)
      attribute = ATTRIBUTE_DEXTERITY;
    RPG_Character_BaseAttackBonus_t attackBonus = player_base->getAttackBonus(attribute,
                                                                              attackSituation_in);
    ACE_ASSERT(!attackBonus.empty());

//     // debug info
//     int index = 1;
//     for (RPG_Character_BaseAttackBonusIterator_t iterator = attackBonus.begin();
//          iterator != attackBonus.end();
//          iterator++, index++)
//     {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("player: \"%s\" attack #%d: base attack bonus: %d\n"),
//                  player_base->getName().c_str(),
//                  index,
//                  static_cast<int> (*iterator)));
//     } // end FOR

    // --> check primary weapon
    weapon_type = const_cast<RPG_Player_Player_Base* const>(player_base)->getEquipment().getPrimaryWeapon(player_base->getOffHand());
    weapon_properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getWeaponProperties(weapon_type);
    // consider range penalty...
    if (weapon_properties.rangeIncrement)
    {
      for (RPG_Character_BaseAttackBonusIterator_t iterator = attackBonus.begin();
           iterator != attackBonus.end();
           iterator++)
        *iterator += (static_cast<int>((distance_in / weapon_properties.rangeIncrement)) * -2);
    } // end IF
    // *TODO*: consider other modifiers...
    // *TODO*: consider multi-weapon/offhand attacks...

    // step2: compute target AC
    // AC = 10 + (natural) armor bonus (+ shield bonus) + DEX modifier + size modifier [+ other modifiers]
    const RPG_Monster* monster = NULL;
    monster = dynamic_cast<const RPG_Monster*>(target_inout);
    ACE_ASSERT(monster);
    targetArmorClass = monster->getArmorClass(defenseSituation_in);
    // *TODO*: consider other modifiers:
    // - enhancement bonuses
    // - deflection bonuses
    // - natural armor
    // - dodge bonuses

    // step3: check if target is within reach at all
    unsigned short minReach = 0;
    unsigned short maxReach = 0;
    maxReach = (RPG_Item_Common_Tools::isProjectileWeapon(weapon_type)) ? (weapon_properties.rangeIncrement * 5)
                                                                        : (weapon_properties.rangeIncrement * 10);
    if (weapon_properties.rangeIncrement == 0)
    {
      maxReach = RPG_Common_Tools::sizeToReach(player_base->getSize());
      // consider reach weapons
      if (weapon_properties.isReachWeapon)
      {
        minReach = maxReach;
        maxReach *= 2;
      } // end IF
    } // end IF

    if ((distance_in < minReach) || (distance_in > maxReach))
    {
      // debug info
      ACE_DEBUG((LM_INFO,
                 ACE_TEXT("player \"%s\": primary weapon (min/max. reach: %d/%d) is not within range %d of \"%s\", returning\n"),
                 player_base->getName().c_str(),
                 minReach,
                 maxReach,
                 distance_in,
                 monster->getName().c_str()));

      // *TODO*: consider the possibility of throwing a melee weapon...
//       maxReach = 50; // not really meant to be thrown...

      // nothing to do...
      return;
    } // end IF

    // *TODO*: consider multi-weapon/offhand attacks...
    // step4: perform attack(s)
    for (RPG_Character_BaseAttackBonusConstIterator_t iterator = attackBonus.begin();
         iterator != attackBonus.end();
         iterator++)
    {
      attack_roll = 0;
      currentAttackBonus = *iterator;
      is_threat = false;
      is_critical_hit = false;

      // step2a: roll D_20
      result.clear();
      RPG_Dice::simulateRoll(roll,
                             1,
                             result);
      attack_roll = result.front();

      // a natural roll of 20 is ALWAYS a hit (roll again to test for critical), a 1 a miss...
      // for increased threat ranges, a score lower than 20 is NOT automatically a hit...
      if (attack_roll >= weapon_properties.criticalHit.minToHitRoll)
      {
        // we have scored a threat...
        is_threat = true;
        // --> roll again to test for critical hit
        result.clear();
        RPG_Dice::simulateRoll(roll,
                               1,
                               result);
      } // end IF

      if (attack_roll == 1)
        goto is_player_miss;

      // hit or miss ?
      if (((attack_roll + currentAttackBonus) < targetArmorClass) && (attack_roll != 20))
        goto is_player_miss;
      else if (is_threat)
      {
        // check for critical
        if ((static_cast<int>(result.front()) + currentAttackBonus) >= targetArmorClass)
          is_critical_hit = true;
      } // end ELSE

// is_player_hit:
      // compute damage
      damage.elements.clear();
      physicalDamageTypeList.clear();
      physicalDamageTypeList = RPG_Item_Common_Tools::weaponDamageTypeToPhysicalDamageType(weapon_properties.typeOfDamage);
      for (RPG_Common_PhysicalDamageListIterator_t iterator = physicalDamageTypeList.begin();
           iterator != physicalDamageTypeList.end();
           iterator++)
      {
        RPG_Combat_DamageTypeUnion damageType;
        damageType.physicaldamagetype = *iterator;
        damage_element.types.push_back(damageType);
      } // end FOR
      damage_element.amount = weapon_properties.baseDamage;
      // compute STR factor
      // *TODO*: consider off-hand, double-handed/weapon fighting etc...
      // light weapon: primary x1, off-hand x.5
      // one-handed: primary x1, off-hand x.5, two-handed x1.5:
      // two-handed: x1.5
      if (RPG_Item_Common_Tools::isTwoHandedWeapon(weapon_type))
        STR_factor = 1.5;
      // add STR modifier for melee attacks and thrown weapons...
      // consider:
      // - bonusses only apply to slings and composite bows
      // - penalties apply to ANY bow or sling
      if ((attackForm == ATTACKFORM_MELEE) ||
          ((attackForm == ATTACKFORM_RANGED) && RPG_Item_Common_Tools::isThrownWeapon(weapon_type)) ||
          (RPG_Character_Common_Tools::getAttributeAbilityModifier(player_base->getAttribute(ATTRIBUTE_STRENGTH)) &&
           ((weapon_type == RANGED_WEAPON_SLING) ||
            (weapon_type == RANGED_WEAPON_BOW_SHORT_COMPOSITE) ||
            (weapon_type == RANGED_WEAPON_BOW_LONG_COMPOSITE))) ||
          ((RPG_Character_Common_Tools::getAttributeAbilityModifier(player_base->getAttribute(ATTRIBUTE_STRENGTH)) < 0) &&
           ((RPG_Item_Common_Tools::isProjectileWeapon(weapon_type) &&
            (weapon_type != RANGED_WEAPON_CROSSBOW_LIGHT) &&
            (weapon_type != RANGED_WEAPON_CROSSBOW_HEAVY) &&
            (weapon_type != RANGED_WEAPON_CROSSBOW_HAND) &&
            (weapon_type != RANGED_WEAPON_CROSSBOW_REPEATING_LIGHT) &&
            (weapon_type != RANGED_WEAPON_CROSSBOW_REPEATING_HEAVY)))))
        damage_element.amount.modifier += static_cast<int>(ACE_OS::floor((RPG_Character_Common_Tools::getAttributeAbilityModifier(player_base->getAttribute(ATTRIBUTE_STRENGTH)) * STR_factor) + 0.5));
      // *TODO*: extra damage over and above a weapon’s normal damage is not multiplied
      if (is_critical_hit) // *IMPORTANT NOTE*: this applies for physical/natural damage only !
        damage_element.amount *= static_cast<int>(weapon_properties.criticalHit.damageModifier);
      damage_element.secondary.numDice = 0;
      damage_element.secondary.typeDice = RPG_DICE_DIETYPE_INVALID;
      damage_element.secondary.modifier = 0;
      damage_element.duration.incubationPeriod.numDice = 0;
      damage_element.duration.incubationPeriod.typeDice = RPG_DICE_DIETYPE_INVALID;
      damage_element.duration.incubationPeriod.modifier = 0;
      damage_element.duration.interval = 0;
      damage_element.duration.totalDuration = 0;
      damage_element.others.clear();
      damage_element.attribute = RPG_COMMON_ATTRIBUTE_INVALID;
      damage_element.counterMeasures.clear();
      damage_element.effect = EFFECT_IMMEDIATE;
      damage.elements.push_back(damage_element);

      // debug info
      ACE_DEBUG((LM_INFO,
                 ACE_TEXT("\"%s\" attacks \"%s\" (AC: %d) with %s and hits: %d%s...\n"),
                 player_base->getName().c_str(),
                 monster->getName().c_str(),
                 targetArmorClass,
                 RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString(weapon_type).c_str(),
                 (attack_roll + currentAttackBonus),
                 (is_critical_hit ? ACE_TEXT_ALWAYS_CHAR(" (critical)") : ACE_TEXT_ALWAYS_CHAR(""))));

      target_inout->sustainDamage(damage);

      // if the target has been disabled, we're done...
      // *TODO*: consider remaining actions...
      if (isCharacterHelpless(target_inout))
        break;

      // if this was a Standard Action, we're done
      if (!isFullRoundAction_in)
        break;

      // perform next attack
      continue;

is_player_miss:
      // debug info
      ACE_DEBUG((LM_INFO,
                 ACE_TEXT("\"%s\" attacks \"%s\" (AC: %d) with %s and misses: %d...\n"),
                 player_base->getName().c_str(),
                 monster->getName().c_str(),
                 targetArmorClass,
                 RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString(weapon_type).c_str(),
                 (attack_roll + currentAttackBonus)));

      // if this was a Standard Action, we're done
      if (!isFullRoundAction_in)
        break;
    } // end FOR
  } // end IF
  else
  {
    // if the attacker is a "regular" monster, we have a specific description of its weapons/abilities
    // step1a: get monster properties
    const RPG_Monster* const monster = dynamic_cast<const RPG_Monster* const>(attacker_in);
    ACE_ASSERT(monster);
    const RPG_Monster_Properties& monster_properties = RPG_MONSTER_DICTIONARY_SINGLETON::instance()->getProperties(monster->getName());

    // step1b: compute target AC
    // AC = 10 + armor bonus + shield bonus + DEX modifier + size modifier [+ other modifiers]
    RPG_Player_Base* const player_base = dynamic_cast<RPG_Player_Base* const>(target_inout);
    ACE_ASSERT(player_base);
    targetArmorClass = player_base->getArmorClass(defenseSituation_in);
    // *TODO*: consider other modifiers:
    // - enhancement bonuses
    // - deflection bonuses
    // - natural armor
    // - dodge bonuses

    // choose appropriate form of attack...
    // consider that the monster may (temporarily) have a bigger reach...
    attackForm = (distance_in > RPG_Common_Tools::sizeToReach(monster->getSize()) ? ATTACKFORM_RANGED : ATTACKFORM_MELEE);

    // step2: perform attack(s)
    // *TODO*: if available (AND preconditions are met), we MAY also choose a special attack...
    // current (non-)strategy: melee/ranged --> special attack
    bool is_special_attack = false;
    unsigned int numberOfPossibleAttackActions = 0;
    RPG_Monster_AttackActionsIterator_t attack_iterator;
    RPG_Monster_SpecialAttackActionsIterator_t special_iterator;
    const RPG_Monster_AttackAction* current_action = NULL;
    int randomPossibleIndex = 0;
    int possibleIndex = 0;
    if (isFullRoundAction_in)
    {
      // sanity check
      numberOfPossibleAttackActions = numCompatibleMonsterAttackActions(attackForm,
                                                                        monster_properties.attack.fullAttackActions);
      if (monster_properties.attack.fullAttackActions.empty() ||
          (numberOfPossibleAttackActions == 0))
        goto init_monster_standard_actions;

      attack_iterator = monster_properties.attack.fullAttackActions.begin();
      if (!monster_properties.attack.actionsAreInclusive)
      {
        // choose any single appropriate (i.e. possible) (set of) full action(s)
        // step1: count the number of available sets
        int numberOfPossibleSets = 0;
        RPG_Monster_AttackActionsIterator_t iterator2 = monster_properties.attack.fullAttackActions.begin();
        do
        {
          if (isCompatibleMonsterAttackAction(attackForm,
                                              *iterator2))
            numberOfPossibleSets++;

          // skip over other attacks in this set...
          while ((*iterator2).fullAttackIncludesNextAction)
            iterator2++;

          // start next set
          iterator2++;
        } while (iterator2 != monster_properties.attack.fullAttackActions.end());

        // choose (random) possible set...
        result.clear();
        RPG_Dice::generateRandomNumbers(numberOfPossibleSets,
                                        1,
                                        result);
        randomPossibleIndex = result.front() - 1;
        possibleIndex = 0;
        do
        {
          // is this a possible set ?
          if (isCompatibleMonsterAttackAction(attackForm,
                                              *attack_iterator))
          {
            // maybe we have found our set...
            if (possibleIndex == randomPossibleIndex)
              break;

            possibleIndex++;
          } // end IF

          // skip to next set...
          while ((*attack_iterator).fullAttackIncludesNextAction)
            attack_iterator++;
        } while (true);
      } // end IF

      current_action = &*attack_iterator;
      goto monster_perform_single_action;
    } // end IF

init_monster_standard_actions:
    // sanity check
    numberOfPossibleAttackActions = numCompatibleMonsterAttackActions(attackForm,
                                                                      monster_properties.attack.standardAttackActions);
    if (monster_properties.attack.standardAttackActions.empty() ||
        (numberOfPossibleAttackActions == 0))
      goto init_monster_special_attack;

    attack_iterator = monster_properties.attack.standardAttackActions.begin();
    // if the attack actions are not inclusive, we need to choose a single (set of) (suitable) one(s)...
    if (!monster_properties.attack.actionsAreInclusive)
    {
      // choose any single appropriate standard action instead
      result.clear();
      RPG_Dice::generateRandomNumbers(numberOfPossibleAttackActions,
                                      1,
                                      result);
      randomPossibleIndex = result.front() - 1;
      possibleIndex = 0;
      do
      {
          // is this a possible set ?
        if (isCompatibleMonsterAttackAction(attackForm,
                                            *attack_iterator))
        {
            // maybe we have found our action...
          if (possibleIndex == randomPossibleIndex)
            break;

          possibleIndex++;
        } // end IF

        // skip to next action...
        while ((*attack_iterator).fullAttackIncludesNextAction)
          attack_iterator++;
      } while (true);
    } // end IF

    current_action = &*attack_iterator;
    goto monster_perform_single_action;

init_monster_special_attack:
    // sanity check
    for (RPG_Monster_SpecialAttackActionsIterator_t iterator2 = monster_properties.specialAttacks.begin();
         iterator2 != monster_properties.specialAttacks.end();
         iterator2++)
    {
      if (isCompatibleMonsterAttackAction(attackForm,
                                          (*iterator2).action))
        numberOfPossibleAttackActions++;
    } // end FOR
    if (numberOfPossibleAttackActions == 0)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("found no suitable special attack for monster \"%s\", aborting\n"),
                 monster->getName().c_str()));

      // what else can we do ?
      return;
    } // end IF

    special_iterator = monster_properties.specialAttacks.begin();
    // choose any single appropriate action
    result.clear();
    RPG_Dice::generateRandomNumbers(numberOfPossibleAttackActions,
                                    1,
                                    result);
    randomPossibleIndex = result.front() - 1;
    possibleIndex = 0;
    do
    {
      // is this a possible set ?
      if (isCompatibleMonsterAttackAction(attackForm,
                                          (*special_iterator).action))
      {
        // maybe we have found our action...
        if (possibleIndex == randomPossibleIndex)
          break;

        possibleIndex++;
      } // end IF

      // skip to next action...
      special_iterator++;
    } while (true);

    current_action = &((*special_iterator).action);
    is_special_attack = true;

monster_perform_single_action:
    // check if the attack is appropriate:
    // --> i.e. within range
    if ((std::find(current_action->attackForms.begin(),
                   current_action->attackForms.end(),
                   ATTACKFORM_MELEE) != current_action->attackForms.end()) ||
        (std::find(current_action->attackForms.begin(),
                   current_action->attackForms.end(),
                   ATTACKFORM_TOUCH) != current_action->attackForms.end()))
    {
      // *TODO*: consider manufactured/reach weapons
      if (monster_properties.reach < distance_in)
        goto monster_advance_attack_iterator;
    } // end IF
    else
    {
      // check max range
      if (current_action->ranged.maxRange < distance_in)
        goto monster_advance_attack_iterator;
    } // end ELSE

    // run attack(s)
    for (unsigned char i = 0;
         i < current_action->numAttacksPerRound;
         i++)
    {
      attack_roll = 0;
      currentAttackBonus = 0;
      is_threat = false;
      is_critical_hit = false;

      // step2a: roll D_20
      result.clear();
      RPG_Dice::simulateRoll(roll,
                            1,
                            result);
      attack_roll = result.front();

      // a natural roll of 20 is ALWAYS a hit (roll again to test for critical), a 1 a miss...
      // for increased threat ranges, a score lower than 20 is NOT automatically a hit...
      // *TODO*: consider any manufactured (and equipped) weapons...
      if (attack_roll == 20)
      {
        // we have scored a threat...
        is_threat = true;
        // --> roll again to test for critical hit
        result.clear();
        RPG_Dice::simulateRoll(roll,
                              1,
                              result);
      } // end IF

      if (attack_roll == 1)
        goto is_monster_miss;

      // attack bonus: [base attack bonus + STR/DEX modifier + size modifier] (+ range penalty)
      if (current_action->attackBonuses.size() == current_action->numAttacksPerRound)
        currentAttackBonus = current_action->attackBonuses[i];
      else
        currentAttackBonus = current_action->attackBonuses[0];

      // consider range penalty...
      if (current_action->ranged.increment)
        currentAttackBonus += (static_cast<int>((distance_in / current_action->ranged.increment)) * -2);
      // *TODO*: consider other modifiers...

      // hit or miss ?
      if (((attack_roll + currentAttackBonus) < targetArmorClass) && (attack_roll != 20))
        goto is_monster_miss;
      else if (is_threat)
      {
        // check for critical
        if ((static_cast<int>(result.front()) + currentAttackBonus) >= targetArmorClass)
          is_critical_hit = true;
      } // end ELSE

// is_monster_hit:
      // compute damage
      damage = current_action->damage;
      // insert (missing) damage types
      for (std::vector<RPG_Combat_DamageElement>::iterator iterator2 = damage.elements.begin();
           iterator2 != damage.elements.end();
           iterator2++)
      {
        if ((*iterator2).types.empty())
        {
          RPG_Common_PhysicalDamageList_t damageTypeList;
          switch (current_action->weapon.discriminator)
          {
            case RPG_Monster_WeaponTypeUnion::NATURALWEAPON:
            {
              damageTypeList = RPG_Monster_Common_Tools::naturalWeaponToPhysicalDamageType(current_action->weapon.naturalweapon);

              break;
            }
            case RPG_Monster_WeaponTypeUnion::WEAPONTYPE:
            {
              weapon_properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getWeaponProperties(current_action->weapon.weapontype);
              damageTypeList = RPG_Item_Common_Tools::weaponDamageTypeToPhysicalDamageType(weapon_properties.typeOfDamage);

              break;
            }
            default:
            {
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("invalid weapon type: %d, continuing\n"),
                         current_action->weapon.discriminator));

              break;
            }
          } // end SWITCH
          RPG_Combat_DamageTypeUnion damageType_union;
          damageType_union.discriminator = RPG_Combat_DamageTypeUnion::PHYSICALDAMAGETYPE;
          for (RPG_Common_PhysicalDamageListIterator_t iterator3 = damageTypeList.begin();
               iterator3 != damageTypeList.end();
               iterator3++)
          {
            damageType_union.physicaldamagetype = *iterator3;
            (*iterator2).types.push_back(damageType_union);
          } // end FOR
        } // end IF
      } // end FOR
      if (is_critical_hit)
      {
        for (std::vector<RPG_Combat_DamageElement>::iterator iterator2 = damage.elements.begin();
             iterator2 != damage.elements.end();
             iterator2++)
        {
          // *IMPORTANT NOTE*: this applies for physical/natural damage only !
          if ((*iterator2).types.front().discriminator != RPG_Combat_DamageTypeUnion::PHYSICALDAMAGETYPE)
            continue;

          // *TODO*: consider manufactured (and equipped) weapons may have different modifiers
          // *IMPORTANT NOTE*: STR modifier already included...
          (*iterator2).amount *= 2;
        } // end FOR
      } // end IF

      ACE_DEBUG((LM_INFO,
                 ACE_TEXT("\"%s\" attacks \"%s\" (AC: %d) with %s and hits: %d%s...\n"),
                 monster->getName().c_str(),
                 player_base->getName().c_str(),
                 targetArmorClass,
                 RPG_Monster_Common_Tools::weaponTypeToString(current_action->weapon).c_str(),
                 (attack_roll + currentAttackBonus),
                 (is_critical_hit ? ACE_TEXT_ALWAYS_CHAR(" (critical)") : ACE_TEXT_ALWAYS_CHAR(""))));

      target_inout->sustainDamage(damage);

      // if the target has been disabled, we're done...
      // *TODO*: consider remaining actions...
      if (isCharacterHelpless(target_inout))
        return;

      // if this was a Standard Action, we're done
      if (!isFullRoundAction_in)
        goto monster_advance_attack_iterator;

      // perform next attack
      continue;

is_monster_miss:
      ACE_DEBUG((LM_INFO,
                 ACE_TEXT("\"%s\" attacks \"%s\" (AC: %d) with %s and misses: %d...\n"),
                 monster->getName().c_str(),
                 player_base->getName().c_str(),
                 targetArmorClass,
                 RPG_Monster_Common_Tools::weaponTypeToString(current_action->weapon).c_str(),
                 (attack_roll + currentAttackBonus)));
    } // end FOR

monster_advance_attack_iterator:
    if (!is_special_attack)
    {
      if (monster_properties.attack.actionsAreInclusive ||
          (isFullRoundAction_in && (*attack_iterator).fullAttackIncludesNextAction))
      {
        attack_iterator++;

        // run next attack, if appropriate
        if (attack_iterator != ((isFullRoundAction_in &&
                                 !monster_properties.attack.fullAttackActions.empty()) ? monster_properties.attack.fullAttackActions.end()
                                                                                       : monster_properties.attack.standardAttackActions.end()))
        {
          current_action = &*attack_iterator;
          goto monster_perform_single_action;
        } // end IF
      } // end IF
    } // end IF
  } // end ELSE
}

RPG_Engine_Player_XMLTree_Type*
RPG_Engine_Common_Tools::playerXMLToEntityXML(const RPG_Player_PlayerXML_XMLTree_Type& player_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Engine_Common_Tools::playerXMLToEntityXML"));

  RPG_Map_Position_XMLTree_Type position(0, 0);
  RPG_Graphics_Sprite sprite = RPG_ENGINE_DEF_ENTITY_SPRITE;

  RPG_Engine_Player_XMLTree_Type* entity_p = NULL;
  entity_p = new(std::nothrow) RPG_Engine_Player_XMLTree_Type(player_in.name(),
                                                              player_in.alignment(),
                                                              player_in.attributes(),
                                                              player_in.defaultSize(),
                                                              player_in.maxHP(),
                                                              player_in.conditions(),
                                                              player_in.HP(),
                                                              player_in.XP(),
                                                              player_in.gold(),
                                                              player_in.inventory(),
                                                              player_in.gender(),
                                                              player_in.classXML(),
                                                              player_in.offhand(),
                                                              position,
                                                              RPG_Graphics_SpriteHelper::RPG_Graphics_SpriteToString(sprite));
  ACE_ASSERT(entity_p);
  if (!entity_p)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory, aborting\n")));

    return NULL;
  }

  // *NOTE*: add race, skills, feats, abilities, known spells, prepared spells sequences "manually"
  entity_p->race(player_in.race());
  if (player_in.skills().present())
    entity_p->skills(player_in.skills());
  if (player_in.feats().present())
    entity_p->feats(player_in.feats());
  if (player_in.abilities().present())
    entity_p->abilities(player_in.abilities());
  if (player_in.knownSpells().present())
    entity_p->knownSpells(player_in.knownSpells());
  if (player_in.spells().present())
    entity_p->spells(player_in.spells());

  return entity_p;
}
