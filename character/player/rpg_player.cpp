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

#include "rpg_player.h"

#include "rpg_player_defines.h"
#include "rpg_player_common_tools.h"
#include "rpg_player_XML_tree.h"

#include <rpg_character_race_common_tools.h>
#include <rpg_character_class_common_tools.h>

#include <rpg_item_instance_common.h>
#include <rpg_item_instance_manager.h>
#include <rpg_item_base.h>
#include <rpg_item_armor.h>
#include <rpg_item_weapon.h>
#include <rpg_item_common_tools.h>
#include <rpg_item_dictionary.h>

#include <rpg_common_macros.h>
#include <rpg_common_defines.h>
#include <rpg_common_file_tools.h>
#include <rpg_common_xsderrorhandler.h>

#include <ace/Log_Msg.h>

#include <fstream>

RPG_Player::RPG_Player(// base attributes
                       const std::string& name_in,
                       const RPG_Character_Gender& gender_in,
                       const RPG_Character_Race_t& race_in,
                       const RPG_Character_Class& class_in,
                       const RPG_Character_Alignment& alignment_in,
                       const RPG_Character_Attributes& attributes_in,
                       const RPG_Character_Skills_t& skills_in,
                       const RPG_Character_Feats_t& feats_in,
                       const RPG_Character_Abilities_t& abilities_in,
                       const RPG_Character_OffHand& offhand_in,
                       const unsigned short int& maxHitPoints_in,
                       const RPG_Magic_SpellTypes_t& knownSpells_in,
                       // current status
                       const RPG_Character_Conditions_t& condition_in,
                       const short int& hitpoints_in,
                       const unsigned int& experience_in,
                       const unsigned int& wealth_in,
                       const RPG_Magic_Spells_t& spells_in,
                       const RPG_Item_List_t& inventory_in)
 : inherited(// base attributes
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
  RPG_TRACE(ACE_TEXT("RPG_Player::RPG_Player"));

  // wear default items...
  defaultEquip();
//   myEquipment.dump();
}

RPG_Player::RPG_Player(const RPG_Player& player_in)
 : inherited(player_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player::RPG_Player"));

}

RPG_Player::~RPG_Player()
{
  RPG_TRACE(ACE_TEXT("RPG_Player::~RPG_Player"));

}

RPG_Player*
RPG_Player::load(const std::string& filename_in,
                 const std::string& schemaRepository_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player::load"));

  // sanity check(s)
  if (!RPG_Common_File_Tools::isReadable(filename_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_File_Tools::isReadable(\"%s\"), aborting\n"),
               filename_in.c_str()));

    return NULL;
  } // end IF

  std::ifstream ifs;
  ifs.exceptions(std::ifstream::badbit | std::ifstream::failbit);
//   ::xml_schema::flags = ::xml_schema::flags::dont_validate;
  ::xml_schema::flags flags = 0;
  ::xml_schema::properties props;
  std::string path;
  // *NOTE*: use the working directory as a fallback...
  if (schemaRepository_in.empty())
    path = RPG_Common_File_Tools::getWorkingDirectory();
  else
  {
    // sanity check(s)
    if (!RPG_Common_File_Tools::isDirectory(schemaRepository_in))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Common_File_Tools::isDirectory(\"%s\"), aborting\n"),
                 schemaRepository_in.c_str()));

      return NULL;
    } // end IF

    path = schemaRepository_in;
  } // end ELSE
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_SCHEMA_FILE);
  // sanity check(s)
  if (!RPG_Common_File_Tools::isReadable(path))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_File_Tools::isReadable(\"%s\"), aborting\n"),
               path.c_str()));

    return NULL;
  } // end IF

  props.schema_location(ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_XML_TARGET_NAMESPACE),
                        path);
//   props.no_namespace_schema_location(RPG_Player_SCHEMA_FILE);
//   props.schema_location("http://www.w3.org/XML/1998/namespace", "xml.xsd");

  std::auto_ptr<RPG_Player_PlayerXML_XMLTree_Type> player_p;
  try
  {
    ifs.open(filename_in.c_str(),
             std::ios_base::in);

    player_p = ::player_t(ifs,
                          RPG_XSDErrorHandler,
                          flags,
                          props);

    ifs.close();

//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("loaded player \"%s\" from file: \"%s\"\n"),
//                player.getName().c_str(),
//                filename_in.c_str()));

    return RPG_Player_Common_Tools::playerXMLToPlayer(*player_p);
  }
  catch (const std::ifstream::failure&)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("\"%s\": unable to open or read error, aborting\n"),
               filename_in.c_str()));
  }
  catch (const ::xml_schema::parsing& exception)
  {
    std::ostringstream converter;
    converter << exception;
    std::string text = converter.str();
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Player::load(\"%s\"): exception occurred: \"%s\", aborting\n"),
               filename_in.c_str(),
               text.c_str()));
  }
  catch (const ::xml_schema::exception& exception)
  {
    std::ostringstream converter;
    converter << exception;
    std::string text = converter.str();
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Player::load(\"%s\"): exception occurred: \"%s\", aborting\n"),
               filename_in.c_str(),
               text.c_str()));
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Player::load(\"%s\"): exception occurred, aborting\n"),
               filename_in.c_str()));
  }

  return NULL;
}

const bool
RPG_Player::save(const std::string& filename_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Player::save"));

  // sanity check(s)
  if (RPG_Common_File_Tools::isReadable(filename_in))
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

  std::ofstream ofs;
  ofs.exceptions(std::ofstream::badbit | std::ofstream::failbit);
  ::xml_schema::namespace_infomap map;
  map[""].name = ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_XML_TARGET_NAMESPACE);
  map[""].schema = ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_SCHEMA_FILE);
  std::string character_set(ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_XML_SCHEMA_CHARSET));
  //   ::xml_schema::flags = ::xml_schema::flags::dont_validate;
  ::xml_schema::flags flags = 0;

  RPG_Player_PlayerXML_XMLTree_Type* player_model = NULL;
  try
  {
    ofs.open(filename_in.c_str(),
             (std::ios_base::out | std::ios_base::trunc));

    player_model = RPG_Player_Common_Tools::playerToPlayerXML(*this);
    ACE_ASSERT(player_model);

    ::player_t(ofs,
               *player_model,
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

    throw exception;
  }
  catch (const ::xml_schema::serialization& exception)
  {
    std::ostringstream converter;
    converter << exception;
    std::string text = converter.str();
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Player::save(\"%s\"): exception occurred: \"%s\", aborting\n"),
               filename_in.c_str(),
               text.c_str()));

    throw exception;
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Player::save(\"%s\"): exception occurred, aborting\n"),
               filename_in.c_str()));

    throw;
  }

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("saved player \"%s\" to file: \"%s\"\n"),
             getName().c_str(),
             filename_in.c_str()));

  // clean up
  delete player_model;

  return true;
}

RPG_Player*
RPG_Player::dummy()
{
  RPG_TRACE(ACE_TEXT("RPG_Player::dummy"));

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
  try
  {
    player_p = new RPG_Player(// base attributes
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
                              // current status
                              condition,
                              1,
                              0,
                              0,
                              preparedSpells,
                              items);
  }
  catch (const std::bad_alloc& exception)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Player::dummy(): exception occurred: \"%s\", aborting\n"),
               exception.what()));

    return player_p;
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Player::dummy(): exception occurred, aborting\n")));

    return player_p;
  }
  ACE_ASSERT(player_p);

  return player_p;
}

void
RPG_Player::defaultEquip()
{
  RPG_TRACE(ACE_TEXT("RPG_Player::operator="));

  RPG_Item_Base* handle = NULL;
  for (RPG_Item_ListIterator_t iterator = myInventory.myItems.begin();
       iterator != myInventory.myItems.end();
       iterator++)
  {
    handle = NULL;
    if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->get(*iterator,
                                                              handle))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid item ID: %d, aborting\n"),
                 *iterator));

      return;
    } // end IF
    ACE_ASSERT(handle);

    switch (handle->getType())
    {
      case ITEM_ARMOR:
      {
        RPG_Item_Armor* armor = dynamic_cast<RPG_Item_Armor*>(handle);
        ACE_ASSERT(armor);
//         RPG_Item_ArmorProperties armor_properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getArmorProperties(armor_base->getArmorType());
        // shield or (body) armor ?
        // *TODO*: what about other types of armor ?
        RPG_Character_EquipmentSlot slot = EQUIPMENTSLOT_BODY;
        if (RPG_Item_Common_Tools::isShield(armor->getArmorType()))
        {
          slot = ((getOffHand() == OFFHAND_LEFT) ? EQUIPMENTSLOT_HAND_LEFT
                                                 : EQUIPMENTSLOT_HAND_RIGHT);
        } // end IF
        myEquipment.equip(*iterator, slot);

        break;
      }
      case ITEM_WEAPON:
      {
        RPG_Item_Weapon* weapon = dynamic_cast<RPG_Item_Weapon*>(handle);
        ACE_ASSERT(weapon);
//         RPG_Item_WeaponProperties weapon_properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getWeaponProperties(weapon_base->getWeaponType());
        // - by default, equip melee weapons only
        // *TODO*: what about other types of weapons ?
        if (!RPG_Item_Common_Tools::isMeleeWeapon(weapon->getWeaponType()))
          break;

        RPG_Character_EquipmentSlot slot = ((getOffHand() == OFFHAND_LEFT) ? EQUIPMENTSLOT_HAND_RIGHT
                                                                           : EQUIPMENTSLOT_HAND_LEFT);
        myEquipment.equip(*iterator, slot);
        if (RPG_Item_Common_Tools::isTwoHandedWeapon(weapon->getWeaponType()))
        {
          slot = ((getOffHand() == OFFHAND_LEFT) ? EQUIPMENTSLOT_HAND_LEFT
                                                 : EQUIPMENTSLOT_HAND_RIGHT);
          myEquipment.equip(*iterator, slot);
        } // end IF

        break;
      }
      default:
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("item ID %d: invalid type: \"%s\", continuing\n"),
                   *iterator,
                   RPG_Item_TypeHelper::RPG_Item_TypeToString(handle->getType()).c_str()));

        break;
      }
    } // end SWITCH
  } // end FOR
}