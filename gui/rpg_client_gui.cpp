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

// *NOTE*: need this to import correct PACKAGE_STRING/VERSION/... !
#ifdef HAVE_CONFIG_H
#include <gui-config.h>
#endif

#include "rpg_client_defines.h"
#include "rpg_client_common.h"
#include "rpg_client_window_main.h"
#include "rpg_client_window_level.h"

#include <rpg_map_defines.h>
#include <rpg_map_common_tools.h>
#include <rpg_map_level.h>

#include <rpg_engine_common_tools.h>

#include <rpg_character_defines.h>
#include <rpg_character_player.h>

#include <rpg_item_base.h>
#include <rpg_item_instance_manager.h>
#include <rpg_item_weapon.h>
#include <rpg_item_armor.h>

#include <rpg_common_tools.h>
#include <rpg_common_file_tools.h>

#include <rpg_graphics_defines.h>
#include <rpg_graphics_dictionary.h>
#include <rpg_graphics_surface.h>
#include <rpg_graphics_cursor.h>
#include <rpg_graphics_SDL_tools.h>
#include <rpg_graphics_common_tools.h>

#include <rpg_sound_dictionary.h>
#include <rpg_sound_common_tools.h>

#include <gnome.h>
#include <glade/glade.h>
#include <gtk/gtk.h>

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include <ace/ACE.h>
#include <ace/Version.h>
#include <ace/Get_Opt.h>
#include <ace/Profile_Timer.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <ace/Signal.h>
#include <ace/Sig_Handler.h>
#include <ace/High_Res_Timer.h>
#include <ace/Configuration.h>
#include <ace/Configuration_Import_Export.h>
#include <ace/Thread_Manager.h>
#include <ace/Dirent_Selector.h>

#include <string>
#include <iostream>
#include <sstream>

static GTK_cb_data_t    userData    = {0,
                                       NULL,
                                       NULL,
                                       NULL,
                                       {0,
                                        0,
                                        RPG_Map_Positions_t(),
                                        RPG_Map_Positions_t(),
                                        RPG_Map_Positions_t()},
                                       RPG_Map_Positions_t(),
                                       RPG_Character_Player::dummy()};
// static GtkBuilder* builder  = NULL;
static GladeXML*        xml         = NULL;
static GtkWidget*       main_dialog = NULL;
static SDL_Surface*     screen      = NULL;
static int              grp_id      = -1;
static ACE_Thread_Mutex hover_lock;
static unsigned long    hover_time  = 0;

void
update_character_profile(const RPG_Character_Player& player_in)
{
  ACE_TRACE(ACE_TEXT("::update_character_profile"));

  // sanity check(s)
  ACE_ASSERT(xml);

  std::string text;
  std::stringstream converter;
  GtkWidget* current = NULL;

  // step1: name
  current = GTK_WIDGET(glade_xml_get_widget(xml,
                                            ACE_TEXT_ALWAYS_CHAR("name")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     player_in.getName().c_str());

  // step2: gender
  text.clear();
  switch (player_in.getGender())
  {
    case RPG_CHARACTER_GENDER_MAX:
    case RPG_CHARACTER_GENDER_INVALID:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid gender (was: \"%s\"), aborting\n"),
                 RPG_Character_GenderHelper::RPG_Character_GenderToString(player_in.getGender()).c_str()));

      break;
    }
    case GENDER_NONE:
      text = ACE_TEXT_ALWAYS_CHAR("N/A"); break;
    default:
      text = RPG_Common_Tools::enumToString(RPG_Character_GenderHelper::RPG_Character_GenderToString(player_in.getGender())); break;
  } // end SWITCH
  current = GTK_WIDGET(glade_xml_get_widget(xml,
                       ACE_TEXT_ALWAYS_CHAR("gender")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     text.c_str());

  // step3: race
  text.clear();
  RPG_Character_Race_t player_race = player_in.getRace();
  if (player_race.count() == 0)
    text = ACE_TEXT_ALWAYS_CHAR("N/A");
  else
  {
    unsigned int race_index = 1;
    for (unsigned int index = 0;
         index < player_race.size();
         index++, race_index++)
    {
      if (player_race.test(index))
      {
        switch (ACE_static_cast(RPG_Character_Race,
                                race_index))
        {
          case RPG_CHARACTER_RACE_MAX:
          case RPG_CHARACTER_RACE_INVALID:
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("invalid race (was: \"%s\"), aborting\n"),
                       RPG_Character_RaceHelper::RPG_Character_RaceToString(ACE_static_cast(RPG_Character_Race,
                                                                                            race_index)).c_str()));

            break;
          }
          default:
            text = RPG_Common_Tools::enumToString(RPG_Character_RaceHelper::RPG_Character_RaceToString(ACE_static_cast(RPG_Character_Race,
                                                                                                                race_index))); break;
        } // end SWITCH
        text += ACE_TEXT_ALWAYS_CHAR(",");
      } // end IF
    } // end FOR
    text.erase(--text.end());
  } // end ELSE
  current = GTK_WIDGET(glade_xml_get_widget(xml,
                       ACE_TEXT_ALWAYS_CHAR("race")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     text.c_str());

  // step4: class
  text.clear();
  RPG_Character_Class player_class = player_in.getClass();
  switch (player_class.metaClass)
  {
    case RPG_CHARACTER_METACLASS_MAX:
    case RPG_CHARACTER_METACLASS_INVALID:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid metaclass (was: \"%s\"), aborting\n"),
                 RPG_Character_MetaClassHelper::RPG_Character_MetaClassToString(player_class.metaClass).c_str()));

      break;
    }
    case METACLASS_NONE:
      text = ACE_TEXT_ALWAYS_CHAR("N/A"); break;
    default:
      text = RPG_Common_Tools::enumToString(RPG_Character_MetaClassHelper::RPG_Character_MetaClassToString(player_class.metaClass)); break;
  } // end SWITCH
  if (!player_class.subClasses.empty())
    text += ACE_TEXT_ALWAYS_CHAR(" (");
  for (RPG_Character_SubClassesIterator_t iterator = player_class.subClasses.begin();
       iterator != player_class.subClasses.end();
       iterator++)
  {
    switch (*iterator)
    {
      case RPG_COMMON_SUBCLASS_MAX:
      case RPG_COMMON_SUBCLASS_INVALID:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid subclass (was: \"%s\"), aborting\n"),
                   RPG_Common_SubClassHelper::RPG_Common_SubClassToString(*iterator).c_str()));

        break;
      }
      case SUBCLASS_NONE:
        text += ACE_TEXT_ALWAYS_CHAR("N/A"); break;
      default:
        text += RPG_Common_Tools::enumToString(RPG_Common_SubClassHelper::RPG_Common_SubClassToString(*iterator)); break;
    } // end SWITCH
    text += ACE_TEXT_ALWAYS_CHAR(",");
  } // end FOR
  if (!player_class.subClasses.empty())
  {
    text.erase(--text.end());
    text += ACE_TEXT_ALWAYS_CHAR(")");
  } // end IF
  current = GTK_WIDGET(glade_xml_get_widget(xml,
                       ACE_TEXT_ALWAYS_CHAR("class")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     text.c_str());

  // step5: alignment
  text.clear();
  RPG_Character_Alignment player_alignment = player_in.getAlignment();
  // "Neutral" "Neutral" --> "True Neutral"
  if ((player_alignment.civic == ALIGNMENTCIVIC_NEUTRAL) &&
      (player_alignment.ethic == ALIGNMENTETHIC_NEUTRAL))
    text = ACE_TEXT_ALWAYS_CHAR("True Neutral");
  else
  {
    switch (player_alignment.civic)
    {
      case RPG_CHARACTER_ALIGNMENTCIVIC_MAX:
      case RPG_CHARACTER_ALIGNMENTCIVIC_INVALID:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid alignment (civic) (was: \"%s\"), aborting\n"),
                   RPG_Character_AlignmentCivicHelper::RPG_Character_AlignmentCivicToString(player_alignment.civic).c_str()));

        break;
      }
      default:
        text += RPG_Common_Tools::enumToString(RPG_Character_AlignmentCivicHelper::RPG_Character_AlignmentCivicToString(player_alignment.civic)); break;
    } // end SWITCH
    text += ACE_TEXT_ALWAYS_CHAR(" ");
    switch (player_alignment.civic)
    {
      case RPG_CHARACTER_ALIGNMENTETHIC_MAX:
      case RPG_CHARACTER_ALIGNMENTETHIC_INVALID:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid alignment (ethic) (was: \"%s\"), aborting\n"),
                   RPG_Character_AlignmentEthicHelper::RPG_Character_AlignmentEthicToString(player_alignment.ethic).c_str()));

        break;
      }
      default:
        text += RPG_Common_Tools::enumToString(RPG_Character_AlignmentEthicHelper::RPG_Character_AlignmentEthicToString(player_alignment.ethic)); break;
    } // end SWITCH
  } // end ELSE
  current = GTK_WIDGET(glade_xml_get_widget(xml,
                       ACE_TEXT_ALWAYS_CHAR("alignment")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     text.c_str());

  // step6: condition
  text.clear();
  RPG_Character_Conditions_t player_condition = player_in.getCondition();
  if (player_condition.size() > 1)
    text += ACE_TEXT_ALWAYS_CHAR(" (");
  for (RPG_Character_ConditionsIterator_t iterator = player_condition.begin();
       iterator != player_condition.end();
       iterator++)
  {
    switch (*iterator)
    {
      case RPG_COMMON_CONDITION_MAX:
      case RPG_COMMON_CONDITION_INVALID:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid condition (was: \"%s\"), aborting\n"),
                   RPG_Common_ConditionHelper::RPG_Common_ConditionToString(*iterator).c_str()));

        break;
      }
      default:
        text += RPG_Common_Tools::enumToString(RPG_Common_ConditionHelper::RPG_Common_ConditionToString(*iterator)); break;
    } // end SWITCH
    text += ACE_TEXT_ALWAYS_CHAR(",");
  } // end FOR
  if (!player_condition.empty())
    text.erase(--text.end());
  if (player_condition.size() > 1)
    text += ACE_TEXT_ALWAYS_CHAR(")");

  current = GTK_WIDGET(glade_xml_get_widget(xml,
                       ACE_TEXT_ALWAYS_CHAR("condition")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     text.c_str());

  // step7: HP
  unsigned short int total_hp = player_in.getNumTotalHitPoints();
  short int hp = player_in.getNumHitPoints();
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
  converter << hp;
  converter << ACE_TEXT_ALWAYS_CHAR(" / ");
  converter << total_hp;
  current = GTK_WIDGET(glade_xml_get_widget(xml,
                       ACE_TEXT_ALWAYS_CHAR("hitpoints")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());

  // step8: AC
  signed char armor_class_normal = player_in.getArmorClass(DEFENSE_NORMAL);
  signed char armor_class_touch = player_in.getArmorClass(DEFENSE_TOUCH);
  signed char armor_class_flatfooted = player_in.getArmorClass(DEFENSE_FLATFOOTED);
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
  converter << ACE_static_cast(int, armor_class_normal);
  converter << ACE_TEXT_ALWAYS_CHAR(" / ");
  converter << ACE_static_cast(int, armor_class_touch);
  converter << ACE_TEXT_ALWAYS_CHAR(" / ");
  converter << ACE_static_cast(int, armor_class_flatfooted);
  current = GTK_WIDGET(glade_xml_get_widget(xml,
                       ACE_TEXT_ALWAYS_CHAR("armorclass")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());

  // step9: XP
  unsigned int experience = player_in.getExperience();
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
  converter << experience;
  current = GTK_WIDGET(glade_xml_get_widget(xml,
                       ACE_TEXT_ALWAYS_CHAR("experience")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());

  // step10: level(s)
  text.clear();
  unsigned char level = 0;
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
  if (player_class.subClasses.empty())
  {
    converter << ACE_static_cast(unsigned int, level);
  } // end IF
  else
  {
    for (RPG_Character_SubClassesIterator_t iterator = player_class.subClasses.begin();
         iterator != player_class.subClasses.end();
         iterator++)
    {
      level = player_in.getLevel(*iterator);
      converter << ACE_static_cast(unsigned int, level);
      converter << ACE_TEXT_ALWAYS_CHAR(" / ");
    } // end FOR
  } // end ELSE
  text = converter.str();
  if (!player_class.subClasses.empty())
  {
    // trim tailing " / "
    std::string::iterator iterator = text.end();
    std::advance(iterator, -3);
    text.erase(iterator, text.end());
  } // end IF
  current = GTK_WIDGET(glade_xml_get_widget(xml,
                       ACE_TEXT_ALWAYS_CHAR("level")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     text.c_str());

  // step11: gold
  unsigned int gold = player_in.getWealth();
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
  converter << gold;
  current = GTK_WIDGET(glade_xml_get_widget(xml,
                       ACE_TEXT_ALWAYS_CHAR("gold")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());

  // step12: attributes
  unsigned int attribute = 0;
  attribute = player_in.getAttribute(ATTRIBUTE_STRENGTH);
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
  converter << attribute;
  current = GTK_WIDGET(glade_xml_get_widget(xml,
                       ACE_TEXT_ALWAYS_CHAR("strength")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());
  attribute = player_in.getAttribute(ATTRIBUTE_DEXTERITY);
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
  converter << attribute;
  current = GTK_WIDGET(glade_xml_get_widget(xml,
                       ACE_TEXT_ALWAYS_CHAR("dexterity")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());
  attribute = player_in.getAttribute(ATTRIBUTE_CONSTITUTION);
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
  converter << attribute;
  current = GTK_WIDGET(glade_xml_get_widget(xml,
                       ACE_TEXT_ALWAYS_CHAR("constitution")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());
  attribute = player_in.getAttribute(ATTRIBUTE_INTELLIGENCE);
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
  converter << attribute;
  current = GTK_WIDGET(glade_xml_get_widget(xml,
                       ACE_TEXT_ALWAYS_CHAR("intelligence")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());
  attribute = player_in.getAttribute(ATTRIBUTE_WISDOM);
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
  converter << attribute;
  current = GTK_WIDGET(glade_xml_get_widget(xml,
                       ACE_TEXT_ALWAYS_CHAR("wisdom")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());
  attribute = player_in.getAttribute(ATTRIBUTE_CHARISMA);
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
  converter << attribute;
  current = GTK_WIDGET(glade_xml_get_widget(xml,
                       ACE_TEXT_ALWAYS_CHAR("charisma")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());

  // step13: feats
  current = GTK_WIDGET(glade_xml_get_widget(xml,
                                            ACE_TEXT_ALWAYS_CHAR("feats_vbox")));
  ACE_ASSERT(current);
  GList* entries = gtk_container_get_children(GTK_CONTAINER(current));
//   ACE_ASSERT(entries);
  if (entries)
  {
    // *TODO*: clear all entries
  } // end IF
  GtkWidget* label = NULL;
  RPG_Character_Feats_t player_feats = player_in.getFeats();
  for (RPG_Character_FeatsConstIterator_t iterator = player_feats.begin();
       iterator != player_feats.end();
       iterator++)
  {
    label = NULL;
    label = gtk_label_new(RPG_Common_Tools::enumToString(RPG_Character_FeatHelper::RPG_Character_FeatToString(*iterator)).c_str());
    ACE_ASSERT(label);
//     gtk_container_add(GTK_CONTAINER(current), label);
    gtk_box_pack_start(GTK_BOX(current), label,
                       TRUE, // expand
                       TRUE, // fill
                       0);   // padding
  } // end FOR

  // step14: abilities
  current = GTK_WIDGET(glade_xml_get_widget(xml,
                                            ACE_TEXT_ALWAYS_CHAR("abilities_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
//   ACE_ASSERT(entries);
  if (entries)
  {
    // *TODO*: clear all entries
  } // end IF
  RPG_Character_Abilities_t player_abilities = player_in.getAbilities();
  for (RPG_Character_AbilitiesConstIterator_t iterator = player_abilities.begin();
       iterator != player_abilities.end();
       iterator++)
  {
    label = NULL;
    label = gtk_label_new(RPG_Common_Tools::enumToString(RPG_Character_AbilityHelper::RPG_Character_AbilityToString(*iterator)).c_str());
    ACE_ASSERT(label);
//     gtk_container_add(GTK_CONTAINER(current), label);
    gtk_box_pack_start(GTK_BOX(current), label,
                       TRUE, // expand
                       TRUE, // fill
                       0);   // padding
  } // end FOR

  // step15: skills
  current = GTK_WIDGET(glade_xml_get_widget(xml,
                                            ACE_TEXT_ALWAYS_CHAR("skills_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
//   ACE_ASSERT(entries);
  if (entries)
  {
    // *TODO*: clear all entries
  } // end IF
  RPG_Character_Skills_t player_skills = player_in.getSkills();
  for (RPG_Character_SkillsConstIterator_t iterator = player_skills.begin();
       iterator != player_skills.end();
       iterator++)
  {
    text.clear();
    text += RPG_Common_Tools::enumToString(RPG_Common_SkillHelper::RPG_Common_SkillToString((*iterator).first));
    text += ACE_TEXT_ALWAYS_CHAR(" : ");
    converter.str(ACE_TEXT(""));
    converter.clear();
    converter << ACE_static_cast(int, (*iterator).second);
    text += converter.str();
    label = NULL;
    label = gtk_label_new(text.c_str());
    ACE_ASSERT(label);
//     gtk_container_add(GTK_CONTAINER(current), label);
    gtk_box_pack_start(GTK_BOX(current), label,
                       TRUE, // expand
                       TRUE, // fill
                       0);   // padding
  } // end FOR

  // step16: spells
  current = GTK_WIDGET(glade_xml_get_widget(xml,
                                            ACE_TEXT_ALWAYS_CHAR("spells_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
//   ACE_ASSERT(entries);
  if (entries)
  {
    // *TODO*: clear all entries
  } // end IF
  RPG_Magic_Spells_t player_known_spells = player_in.getKnownSpells();
  RPG_Magic_SpellList_t player_spells = player_in.getSpells();
  unsigned int number = 0;
  for (RPG_Magic_SpellsIterator_t iterator = player_known_spells.begin();
       iterator != player_known_spells.end();
       iterator++)
  {
    // *NOTE*: these spells are KNOWN
    // --> for each spells, check whether (and how often) it has been prepared/memorized
    // i.e. Wizard/Cleric and other classes
    number = 0;
    for (RPG_Magic_SpellListIterator_t iterator2 = player_spells.begin();
         iterator2 != player_spells.end();
         iterator2++)
      if (*iterator2 == *iterator)
        number++;

    text.clear();
    text += RPG_Common_Tools::enumToString(RPG_Magic_SpellTypeHelper::RPG_Magic_SpellTypeToString(*iterator));
    if (number)
    {
      text += ACE_TEXT_ALWAYS_CHAR(" : ");
      converter.str(ACE_TEXT(""));
      converter.clear();
      converter << number;
      text += converter.str();
    } // end IF

    label = NULL;
    label = gtk_label_new(text.c_str());
    ACE_ASSERT(label);
//     gtk_container_add(GTK_CONTAINER(current), label);
    gtk_box_pack_start(GTK_BOX(current), label,
                       TRUE, // expand
                       TRUE, // fill
                       0);   // padding
  } // end FOR

  // step17: inventory
  current = GTK_WIDGET(glade_xml_get_widget(xml,
                                            ACE_TEXT_ALWAYS_CHAR("inventory_items_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
//   ACE_ASSERT(entries);
  if (entries)
  {
    // *TODO*: clear all entries
  } // end IF
  RPG_Character_Inventory player_inventory = player_in.getInventory();
//   RPG_Character_Equipment player_equipment = player_in.getEquipment();
  RPG_Item_Base* item = NULL;
  for (RPG_Item_ListIterator_t iterator = player_inventory.myItems.begin();
       iterator != player_inventory.myItems.end();
       iterator++)
  {
    text.clear();
    item = NULL;

    // retrieve item handle
    if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->get(*iterator,
                                                              item))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid item (ID: %d), continuing\n"),
                 *iterator));

      continue;
    } // end IF
    ACE_ASSERT(item);

    switch (item->getType())
    {
      case ITEM_WEAPON:
      {
        RPG_Item_Weapon* weapon = ACE_dynamic_cast(RPG_Item_Weapon*, item);
        ACE_ASSERT(weapon);

        // *TODO*: pretty-print this string
        text += RPG_Common_Tools::enumToString(RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString(weapon->getWeaponType()),
                                               false); // don't strip leading "xxx_"

        break;
      }
      case ITEM_ARMOR:
      {
        RPG_Item_Armor* armor = ACE_dynamic_cast(RPG_Item_Armor*, item);
        ACE_ASSERT(armor);

        text += RPG_Common_Tools::enumToString(RPG_Item_ArmorTypeHelper::RPG_Item_ArmorTypeToString(armor->getArmorType()));

        break;
      }
      default:
      {
        // *TODO*: pretty-print this string

        break;
      }
    } // end SWITCH

    // equipped ? --> add '*'
    if (player_in.getEquipment()->isEquipped(*iterator))
      text += ACE_TEXT_ALWAYS_CHAR(" *");

    label = NULL;
    label = gtk_label_new(text.c_str());
    ACE_ASSERT(label);
//     gtk_container_add(GTK_CONTAINER(current), label);
    gtk_box_pack_start(GTK_BOX(current), label,
                       TRUE, // expand
                       TRUE, // fill
                       0);   // padding
  } // end FOR
}

// callbacks used by ::scandir...
static int
dirent_selector(const dirent* entry_in)
{
  ACE_TRACE(ACE_TEXT("::dirent_selector"));

  // *NOTE*: select *.xml files
  std::string filename(entry_in->d_name);
  std::string extension(RPG_CHARACTER_PLAYER_PROFILE_EXT);
  if (filename.rfind(extension,
      std::string::npos) != (filename.size() - extension.size()))
  {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("ignoring \"%s\"...\n"),
//                entry_in->d_name));

    return 0;
  } // end IF

  return 1;
}

static int
dirent_comparator(const dirent** entry1_in,
                  const dirent** entry2_in)
{
  ACE_TRACE(ACE_TEXT("::dirent_comparator"));

  return ACE_OS::strcmp((*entry1_in)->d_name,
                          (*entry2_in)->d_name);
}

void
load_character_profiles(const std::string& repository_in,
                        GtkListStore* listStore_in)
//                         GtkComboBox* comboBox_in)
{
  ACE_TRACE(ACE_TEXT("::load_character_profiles"));

  // sanity check(s)
  ACE_ASSERT(listStore_in);
  if (!RPG_Common_File_Tools::isDirectory(repository_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to load_character_profiles(\"%s\"), not a directory, aborting\n"),
               repository_in.c_str()));

    return;
  } // end IF

  // retrieve all existing character profiles (*.xml) and sort them alphabetically...
  ACE_Dirent_Selector entries;
  if (entries.open(repository_in.c_str(),
                   &::dirent_selector,
                   &::dirent_comparator) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Dirent_Selector::open(\"%s\"): \"%m\", aborting\n"),
               repository_in.c_str()));

    return;
  } // end IF

  // clear existing entries
  // *WARNING* triggers the "changed" signal of the combobox...
  gtk_list_store_clear(listStore_in);

  // iterate over entries
  std::string entry;
  std::string extension(RPG_CHARACTER_PLAYER_PROFILE_EXT);
  GtkTreeIter iter;
  for (unsigned int i = 0;
       i < ACE_static_cast(unsigned int, entries.length());
       i++)
  {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("character profile[%u]: %s\n"),
//                num_profiles,
//                entries[i]->d_name));

    // sanitize name (chop off extension)
    entry = entries[i]->d_name;
    entry.erase(entry.rfind(extension,
                std::string::npos),
                std::string::npos);

    // append new (text) entry
    gtk_list_store_append(listStore_in, &iter);
    gtk_list_store_set(listStore_in, &iter,
                       0, entry.c_str(), // column 0
                       -1);
  } // end FOR

  // clean up
  entries.close();

  // debug info
  GValue value;
  ACE_OS::memset(&value,
                 0,
                 sizeof(value));
  const gchar* text = NULL;
  if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(listStore_in),
                                    &iter))
  {
    gtk_tree_model_get_value(GTK_TREE_MODEL(listStore_in), &iter,
                             0, &value);
    text = g_value_get_string(&value);
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("character profile[0]: %s\n"),
               std::string(text).c_str()));

    g_value_unset(&value);
  } // end IF
  for (unsigned int i = 1;
       gtk_tree_model_iter_next(GTK_TREE_MODEL(listStore_in),
                                &iter);
       i++)
  {
    ACE_OS::memset(&value,
                   0,
                   sizeof(value));
    text = NULL;

    gtk_tree_model_get_value(GTK_TREE_MODEL(listStore_in), &iter,
                             0, &value);
    text = g_value_get_string(&value);
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("character profile[%u]: %s\n"),
               i,
               std::string(text).c_str()));

    g_value_unset(&value);
  } // end FOR
}

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT gint
about_activated_GTK_cb(GtkWidget* widget_in,
//                        GdkEvent* event_in,
                       gpointer userData_in)
{
  ACE_TRACE(ACE_TEXT("::about_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
//   ACE_UNUSED_ARG(event_in);
  ACE_UNUSED_ARG(userData_in);

  // sanity check(s)
  ACE_ASSERT(xml);

  // retrieve about dialog handle
  GtkWidget* about_dialog = GTK_WIDGET(glade_xml_get_widget(xml,
                                                            RPG_CLIENT_DEF_GNOME_ABOUTDIALOG_NAME));
  ACE_ASSERT(about_dialog);
  if (!about_dialog)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
               RPG_CLIENT_DEF_GNOME_ABOUTDIALOG_NAME));

    // clean up
    g_object_unref(G_OBJECT(xml));
    xml = NULL;

    return FALSE;
  } // end IF

  // draw it
  if (!GTK_WIDGET_VISIBLE(about_dialog))
    gtk_widget_show_all(about_dialog);

  return FALSE;
}

G_MODULE_EXPORT gint
properties_activated_GTK_cb(GtkWidget* widget_in,
//                             GdkEvent* event_in,
                            gpointer userData_in)
{
  ACE_TRACE(ACE_TEXT("::properties_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
//   ACE_UNUSED_ARG(event_in);
  ACE_UNUSED_ARG(userData_in);

  return FALSE;
}

G_MODULE_EXPORT gint
quit_activated_GTK_cb(GtkWidget* widget_in,
//                       GdkEvent* event_in,
                      gpointer userData_in)
{
  ACE_TRACE(ACE_TEXT("::quit_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
//   ACE_UNUSED_ARG(event_in);
  ACE_UNUSED_ARG(userData_in);

  // stop reactor
  if (ACE_Reactor::instance()->end_event_loop() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Reactor::end_event_loop(): \"%m\", continuing\n")));
  } // end IF

  // ... and wait for the reactor worker(s) to join
  ACE_Thread_Manager::instance()->wait_grp(grp_id);

  // no more data will arrive from here on...

  // leave GTK
  gtk_main_quit();

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("leaving...\n")));

  // this is the "delete-event" handler
  // --> destroy the main dialog widget
  return TRUE;
}

G_MODULE_EXPORT gint
create_character_activated_GTK_cb(GtkWidget* widget_in,
//                                   GdkEvent* event_in,
                                  gpointer userData_in)
{
  ACE_TRACE(ACE_TEXT("::create_character_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
//   ACE_UNUSED_ARG(event_in);
  ACE_UNUSED_ARG(userData_in);

  return FALSE;
}

G_MODULE_EXPORT gint
load_character_activated_GTK_cb(GtkWidget* widget_in,
//                                 GdkEvent* event_in,
                                gpointer userData_in)
{
  ACE_TRACE(ACE_TEXT("::load_character_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
//   ACE_UNUSED_ARG(event_in);
  ACE_UNUSED_ARG(userData_in);

  // *NOTE*: this callback just presents the file picker
  // --> the business logic happens in character_file_actived_GTK_cb

  // sanity check(s)
  ACE_ASSERT(xml);

  // retrieve file chooser dialog handle
  GtkFileChooserDialog* filechooser_dialog = GTK_FILE_CHOOSER_DIALOG(glade_xml_get_widget(xml,
                                                                                          RPG_CLIENT_DEF_GNOME_FILECHOOSERDIALOG_NAME));
  ACE_ASSERT(filechooser_dialog);
  if (!filechooser_dialog)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
               RPG_CLIENT_DEF_GNOME_FILECHOOSERDIALOG_NAME));

    // clean up
    g_object_unref(G_OBJECT(xml));
    xml = NULL;

    return FALSE;
  } // end IF

  // draw it
  if (!GTK_WIDGET_VISIBLE(filechooser_dialog))
    gtk_widget_show_all(GTK_WIDGET(filechooser_dialog));

  return FALSE;
}

G_MODULE_EXPORT gint
character_file_activated_GTK_cb(GtkWidget* widget_in,
//                                 GdkEvent* event_in,
                                gpointer userData_in)
{
  ACE_TRACE(ACE_TEXT("::character_file_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
//   ACE_UNUSED_ARG(event_in);
  GTK_cb_data_t* userData_p = ACE_static_cast(GTK_cb_data_t*, userData_in);

  // sanity check(s)
  ACE_ASSERT(userData_p);
  ACE_ASSERT(xml);

  // retrieve file chooser dialog handle
  GtkFileChooserDialog* filechooser_dialog = GTK_FILE_CHOOSER_DIALOG(glade_xml_get_widget(xml,
                                                                     RPG_CLIENT_DEF_GNOME_FILECHOOSERDIALOG_NAME));
  ACE_ASSERT(filechooser_dialog);

  // hide widget
  gtk_widget_hide(GTK_WIDGET(filechooser_dialog));

  // retrieve selected filename
  std::string filename(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooser_dialog)));

  // load player profile
  userData_p->player = RPG_Character_Player::load(filename);

  // update character profile widgets
  ::update_character_profile(userData_p->player);

  // make character display frame sensitive (if it's not already)
  GtkFrame* character_frame = GTK_FRAME(glade_xml_get_widget(xml,
                                                             ACE_TEXT_ALWAYS_CHAR("character")));
  ACE_ASSERT(character_frame);
  gtk_widget_set_sensitive(GTK_WIDGET(character_frame), TRUE);

  return FALSE;
}

G_MODULE_EXPORT gint
save_character_activated_GTK_cb(GtkWidget* widget_in,
//                                 GdkEvent* event_in,
                                gpointer userData_in)
{
  ACE_TRACE(ACE_TEXT("::save_character_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
//   ACE_UNUSED_ARG(event_in);
  ACE_UNUSED_ARG(userData_in);

  return FALSE;
}

G_MODULE_EXPORT gint
join_game_activated_GTK_cb(GtkWidget* widget_in,
//                            GdkEvent* event_in,
                           gpointer userData_in)
{
  ACE_TRACE(ACE_TEXT("::join_game_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
//   ACE_UNUSED_ARG(event_in);
  ACE_UNUSED_ARG(userData_in);

  return FALSE;
}

G_MODULE_EXPORT gint
characters_activated_GTK_cb(GtkWidget* widget_in,
//                             GdkEvent* event_in,
                            gpointer userData_in)
{
  ACE_TRACE(ACE_TEXT("::characters_activated_GTK_cb"));

//   ACE_UNUSED_ARG(event_in);
  GTK_cb_data_t* userData_p = ACE_static_cast(GTK_cb_data_t*, userData_in);

  // sanity check(s)
  ACE_ASSERT(widget_in);
  ACE_ASSERT(userData_p);

  // retrieve active item
  std::string active_item;
  GtkTreeIter selected;
  GtkTreeModel* model = NULL;
  GValue value;
  const gchar* text = NULL;
  if (!gtk_combo_box_get_active_iter(GTK_COMBO_BOX(widget_in), &selected))
  {
    // *WARNING*: refreshing the combobox triggers removal of items
    // which also generates this signal...
    return FALSE;
  } // end IF
  model = gtk_combo_box_get_model(GTK_COMBO_BOX(widget_in));
  ACE_ASSERT(model);
  ACE_OS::memset(&value,
                 0,
                 sizeof(value));
  gtk_tree_model_get_value(model, &selected,
                           0, &value);
  text = g_value_get_string(&value);
  // sanity check
  ACE_ASSERT(text);
  active_item = text;
  g_value_unset(&value);

  // construct filename
  std::string filename = RPG_CLIENT_DEF_CHARACTER_REPOSITORY;
  filename += ACE_DIRECTORY_SEPARATOR_STR;
  filename += active_item;
  filename += RPG_CHARACTER_PLAYER_PROFILE_EXT;

  // load player profile
  userData_p->player = RPG_Character_Player::load(filename);

  // update character profile widgets
  ::update_character_profile(userData_p->player);

  // make character display frame sensitive (if it's not already)
  GtkFrame* character_frame = GTK_FRAME(glade_xml_get_widget(xml,
                                                             ACE_TEXT_ALWAYS_CHAR("character")));
  ACE_ASSERT(character_frame);
  gtk_widget_set_sensitive(GTK_WIDGET(character_frame), TRUE);

  return FALSE;
}

G_MODULE_EXPORT gint
characters_refresh_activated_GTK_cb(GtkWidget* widget_in,
//                                     GdkEvent* event_in,
                                    gpointer userData_in)
{
  ACE_TRACE(ACE_TEXT("::characters_refresh_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
//   ACE_UNUSED_ARG(event_in);
  ACE_UNUSED_ARG(userData_in);

  // retrieve tree model
  GtkComboBox* available_characters = GTK_COMBO_BOX(glade_xml_get_widget(xml,
                                                                         RPG_CLIENT_DEF_GNOME_CHARBOX_NAME));
  ACE_ASSERT(available_characters);
  GtkTreeModel* model = gtk_combo_box_get_model(available_characters);
  ACE_ASSERT(model);

  // re-load profile data
  ::load_character_profiles(RPG_CLIENT_DEF_CHARACTER_REPOSITORY,
                            GTK_LIST_STORE(model));

  // set sensitive as appropriate
  GtkFrame* character_frame = GTK_FRAME(glade_xml_get_widget(xml,
                                                             ACE_TEXT_ALWAYS_CHAR("character")));
  ACE_ASSERT(character_frame);
  if (g_list_length(gtk_container_get_children(GTK_CONTAINER(available_characters))))
  {
    gtk_widget_set_sensitive(GTK_WIDGET(available_characters), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(character_frame), TRUE);
  } // end IF
  else
  {
    gtk_widget_set_sensitive(GTK_WIDGET(available_characters), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(character_frame), FALSE);
  } // end ELSE

  // ... activate first entry as appropriate
  if (gtk_widget_is_sensitive(GTK_WIDGET(available_characters)))
    gtk_combo_box_set_active(available_characters, 0);

  return FALSE;
}
#ifdef __cplusplus
}
#endif /* __cplusplus */

Uint32
event_timer_SDL_cb(Uint32 interval_in,
                   void* argument_in)
{
  ACE_TRACE(ACE_TEXT("::event_timer_SDL_cb"));

  unsigned long* current_hovertime_p = ACE_static_cast(unsigned long*, argument_in);
  ACE_ASSERT(current_hovertime_p);

  // synch access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(hover_lock);

    *current_hovertime_p += interval_in;
    if (*current_hovertime_p > RPG_GRAPHICS_WINDOW_HOTSPOT_HOVER_DELAY)
    {
      // mouse is hovering --> trigger an event
      SDL_Event event;
      event.type = RPG_GRAPHICS_SDL_HOVEREVENT;
      event.user.code = ACE_static_cast(int, *current_hovertime_p);

      if (SDL_PushEvent(&event))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to SDL_PushEvent(): \"%s\", continuing\n"),
                   SDL_GetError()));
      } // end IF
    } // end IF
  } // end lock scope

  // re-schedule
  return interval_in;
}

Uint32
input_timer_SDL_cb(Uint32 interval_in,
                   void* argument_in)
{
  ACE_TRACE(ACE_TEXT("::input_timer_SDL_cb"));

  // create a timer event
  SDL_Event event;
  event.type = RPG_CLIENT_SDL_TIMEREVENT;
  event.user.data1 = argument_in;

  // push it onto the event queue
  if (SDL_PushEvent(&event))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_PushEvent(): \"%s\", continuing\n"),
               SDL_GetError()));
  } // end IF

  // one-shot timer --> cancel
  return 0;
}

// wait for an input event; stop after timeout_in second(s) (0: wait forever)
void
do_SDL_waitForInput(const unsigned long& timeout_in,
                    SDL_Event& event_out)
{
  ACE_TRACE(ACE_TEXT("::do_SDL_waitForInput"));

  SDL_TimerID timer = NULL;
  if (timeout_in)
    timer = SDL_AddTimer((timeout_in * 1000), // interval (ms)
                         input_timer_SDL_cb,  // timeout callback
                         NULL);               // callback argument
  ACE_ASSERT(timer);
  if (!timer)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_AddTimer(%u): \"%s\", aborting\n"),
               (timeout_in * 1000),
               SDL_GetError()));

    return;
  } // end IF

  // loop until something interesting happens
  do
  {
    if (SDL_WaitEvent(&event_out) != 1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_WaitEvent(): \"%s\", aborting\n"),
                 SDL_GetError()));

      // what else can we do ?
      break;
    } // end IF
    if (event_out.type == SDL_KEYDOWN ||
        event_out.type == SDL_KEYUP ||
        event_out.type == SDL_MOUSEMOTION ||
        event_out.type == SDL_MOUSEBUTTONDOWN ||
        event_out.type == SDL_QUIT ||
        event_out.type == RPG_CLIENT_SDL_TIMEREVENT)
      break;
  } while (true);

  // clean up
  if (timeout_in &&
      (event_out.type != RPG_CLIENT_SDL_TIMEREVENT))
    if (!SDL_RemoveTimer(timer))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_RemoveTimer(): \"%s\", continuing\n"),
               SDL_GetError()));
  } // end IF
}

G_MODULE_EXPORT gint
do_SDLEventLoop_GTK_cb(gpointer userData_in)
{
  ACE_TRACE(ACE_TEXT("::do_SDLEventLoop_GTK_cb"));

  GTK_cb_data_t* userData_p = ACE_static_cast(GTK_cb_data_t*, userData_in);
  ACE_ASSERT(userData_p);

  SDL_Event event;
  RPG_Graphics_Position_t mouse_position = std::make_pair(0, 0);
  RPG_Graphics_IWindow* window = NULL;
  bool need_redraw = false;
  bool done = false;
//   while (SDL_WaitEvent(&event) > 0)
  if (SDL_PollEvent(&event))
  {
    // if necessary, reset hover_time
    if (event.type != RPG_GRAPHICS_SDL_HOVEREVENT)
    {
      // synch access
      ACE_Guard<ACE_Thread_Mutex> aGuard(hover_lock);

      hover_time = 0;
    } // end IF

    switch (event.type)
    {
      case SDL_KEYDOWN:
      {
        switch (event.key.keysym.sym)
        {
          case SDLK_m:
          {
            std::string dump_path = RPG_MAP_DUMP_DIR;
            dump_path += ACE_DIRECTORY_SEPARATOR_STR;
            dump_path += ACE_TEXT("map.txt");
            if (!RPG_Map_Common_Tools::save(dump_path,  // file
                                            userData_p->seedPoints, // seed points
                                            userData_p->plan))      // plan
            {
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("failed to RPG_Map_Common_Tools::save(\"%s\"), aborting\n"),
                         dump_path.c_str()));

              done = true;
            } // end IF

            break;
          }
          case SDLK_q:
          {
            // finished event processing
            done = true;

            break;
          }
          default:
          {
            break;
          }
        } // end SWITCH

        if (done)
          break; // leave
        // *WARNING*: falls through !
      }
      case SDL_ACTIVEEVENT:
      case SDL_MOUSEMOTION:
      case SDL_MOUSEBUTTONDOWN:
      case RPG_GRAPHICS_SDL_HOVEREVENT: // hovering...
      {
        // find window
        switch (event.type)
        {
          case SDL_MOUSEMOTION:
            mouse_position = std::make_pair(event.motion.x,
                                            event.motion.y); break;
          case SDL_MOUSEBUTTONDOWN:
            mouse_position = std::make_pair(event.button.x,
                                            event.button.y); break;
          default:
          {
            int x,y;
            SDL_GetMouseState(&x, &y);
            mouse_position = std::make_pair(x,
                                            y);

            break;
          }
        } // end SWITCH

        window = userData_p->main_window->getWindow(mouse_position);
        ACE_ASSERT(window);

        // notify previously "active" window upon losing "focus"
        if (event.type == SDL_MOUSEMOTION)
        {
          if (userData_p->previous_window &&
//                 (userData_p->previous_window != mainWindow)
              (userData_p->previous_window != window))
          {
            event.type = RPG_GRAPHICS_SDL_MOUSEMOVEOUT;
            try
            {
              userData_p->previous_window->handleEvent(event,
                                                       userData_p->previous_window,
                                                       need_redraw);
            }
            catch (...)
            {
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("caught exception in RPG_Graphics_IWindow::handleEvent(), continuing\n")));
            }
            event.type = SDL_MOUSEMOTION;
          } // end IF
        } // end IF
        // remember last "active" window
        userData_p->previous_window = window;

        // notify "active" window
        try
        {
          window->handleEvent(event,
                              window,
                              need_redraw);
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                      ACE_TEXT("caught exception in RPG_Graphics_IWindow::handleEvent(), continuing\n")));
        }

        break;
      }
      case SDL_QUIT:
      {
        // finished event processing
        done = true;

        break;
      }
      case SDL_KEYUP:
      case SDL_MOUSEBUTTONUP:
      case SDL_JOYAXISMOTION:
      case SDL_JOYBALLMOTION:
      case SDL_JOYHATMOTION:
      case SDL_JOYBUTTONDOWN:
      case SDL_JOYBUTTONUP:
      case SDL_SYSWMEVENT:
      case SDL_VIDEORESIZE:
      case SDL_VIDEOEXPOSE:
      case RPG_CLIENT_SDL_TIMEREVENT:
      default:
      {

        break;
      }
    } // end SWITCH

        // redraw map ?
    if (need_redraw)
    {
      try
      {
        userData_p->map_window->draw();
        userData_p->map_window->refresh();
      }
      catch (...)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("caught exception in RPG_Graphics_IWindow::draw()/refresh(), continuing\n")));
      }
    } // end IF

    // redraw cursor ?
    switch (event.type)
    {
      case SDL_KEYDOWN:
      case SDL_MOUSEBUTTONDOWN:
      {
        // map hasn't changed --> no need to redraw
        if (!need_redraw)
          break;

        // *WARNING*: falls through !
      }
      case SDL_MOUSEMOTION:
      case RPG_GRAPHICS_SDL_HOVEREVENT:
      {
        // map has changed, cursor MAY have been drawn over...
        // --> redraw cursor
        SDL_Rect dirtyRegion;
        RPG_GRAPHICS_CURSOR_SINGLETON::instance()->put(mouse_position.first,
                                                       mouse_position.second,
                                                       screen,
                                                       dirtyRegion);
        RPG_Graphics_Surface::update(dirtyRegion,
                                     screen);

        break;
      }
      default:
      {
        break;
      }
    } // end SWITCH

    if (done)
    {
      if (!SDL_RemoveTimer(userData_p->event_timer))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to SDL_RemoveTimer(): \"%s\", continuing\n"),
                   SDL_GetError()));
      } // end IF

      // stop reactor
      if (ACE_Reactor::instance()->end_event_loop() == -1)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_Reactor::end_event_loop(): \"%m\", continuing\n")));
      } // end IF

      // ... and wait for the reactor worker(s) to join
      ACE_Thread_Manager::instance()->wait_grp(grp_id);

      // no more data will arrive from here on...

      // leave GTK
      gtk_main_quit();

      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("leaving...\n")));

      // quit idle task
      return 0;
    } // end IF
  } // end IF

  // continue idle task
  return 1;
}

void
print_usage(const std::string& programName_in)
{
  ACE_TRACE(ACE_TEXT("::print_usage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-c [FILE]   : config file") << ACE_TEXT(" [") << RPG_CLIENT_DEF_INI_FILE << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-e [FILE]   : monster dictionary (*.xml)") << ACE_TEXT(" [") << RPG_CLIENT_DEF_MONSTER_DICTIONARY << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-f [FILE]   : floor plan (*.txt)") << std::endl;
  std::cout << ACE_TEXT("-g [FILE]   : graphics dictionary (*.xml)") << ACE_TEXT(" [") << RPG_CLIENT_DEF_GRAPHICS_DICTIONARY << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-i [FILE]   : item dictionary (*.xml)") << ACE_TEXT(" [") << RPG_CLIENT_DEF_ITEM_DICTIONARY << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-l          : log to a file") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-m [FILE]   : magic dictionary (*.xml)") << ACE_TEXT(" [") << RPG_CLIENT_DEF_MAGIC_DICTIONARY << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-s [FILE]   : sound dictionary (*.xml)") << ACE_TEXT(" [") << RPG_CLIENT_DEF_SOUND_DICTIONARY << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-t          : trace information") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-u [FILE]   : UI file") << ACE_TEXT(" [") << RPG_CLIENT_DEF_GNOME_UI_FILE << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-v          : print version information and exit") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-x<[VALUE]> : use thread pool <#threads>") << ACE_TEXT(" [") << RPG_CLIENT_DEF_ACE_USES_TP  << ACE_TEXT(" : ") << RPG_CLIENT_DEF_ACE_NUM_TP_THREADS << ACE_TEXT("]") << std::endl;
} // end print_usage

const bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
                  std::string& iniFile_out,
                  std::string& monsterDictionary_out,
                  std::string& floorPlan_out,
                  std::string& graphicsDictionary_out,
                  std::string& itemDictionary_out,
                  bool& logToFile_out,
                  std::string& magicDictionary_out,
                  std::string& soundDictionary_out,
                  bool& traceInformation_out,
                  std::string& UIfile_out,
                  bool& printVersionAndExit_out,
                  unsigned long& numThreadPoolThreads_out)
{
  ACE_TRACE(ACE_TEXT("::process_arguments"));

  // init results
  iniFile_out              = RPG_CLIENT_DEF_INI_FILE;
  monsterDictionary_out    = RPG_CLIENT_DEF_MONSTER_DICTIONARY;
  floorPlan_out.clear();
  soundDictionary_out      = RPG_CLIENT_DEF_GRAPHICS_DICTIONARY;
  itemDictionary_out       = RPG_CLIENT_DEF_ITEM_DICTIONARY;
  logToFile_out            = false;
  magicDictionary_out      = RPG_CLIENT_DEF_MAGIC_DICTIONARY;
  soundDictionary_out      = RPG_CLIENT_DEF_SOUND_DICTIONARY;
  traceInformation_out     = false;
  UIfile_out               = RPG_CLIENT_DEF_GNOME_UI_FILE;
  printVersionAndExit_out  = false;
  numThreadPoolThreads_out = (RPG_CLIENT_DEF_ACE_USES_TP ? RPG_CLIENT_DEF_ACE_NUM_TP_THREADS : 0);

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("c:e:f:g:i:lm:s:tu:vx::"),
                             1,                         // skip command name
                             1,                         // report parsing errors
                             ACE_Get_Opt::PERMUTE_ARGS, // ordering
                             0);                        // don't use long options

  int option = 0;
  std::stringstream converter;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'c':
      {
        iniFile_out = argumentParser.opt_arg();

        break;
      }
      case 'e':
      {
        monsterDictionary_out = argumentParser.opt_arg();

        break;
      }
      case 'f':
      {
        floorPlan_out = argumentParser.opt_arg();

        break;
      }
      case 'g':
      {
        graphicsDictionary_out = argumentParser.opt_arg();

        break;
      }
      case 'i':
      {
        itemDictionary_out = argumentParser.opt_arg();

        break;
      }
      case 'l':
      {
        logToFile_out = true;

        break;
      }
      case 'm':
      {
        magicDictionary_out = argumentParser.opt_arg();

        break;
      }
      case 's':
      {
        soundDictionary_out = argumentParser.opt_arg();

        break;
      }
      case 't':
      {
        traceInformation_out = true;

        break;
      }
      case 'u':
      {
        UIfile_out = argumentParser.opt_arg();

        break;
      }
      case 'v':
      {
        printVersionAndExit_out = true;

        break;
      }
      case 'x':
      {
        numThreadPoolThreads_out = RPG_CLIENT_DEF_ACE_NUM_TP_THREADS;
        converter.clear();
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
        converter >> numThreadPoolThreads_out;

        break;
      }
      // error handling
      case ':':
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("option \"%c\" requires an argument, aborting\n"),
                   argumentParser.opt_opt()));

        return false;
      }
      case '?':
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("unrecognized option \"%s\", aborting\n"),
                   argumentParser.last_option()));

        return false;
      }
      case 0:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("found long option \"%s\", aborting\n"),
                   argumentParser.long_option()));

        return false;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("parse error, aborting\n")));

        return false;
      }
    } // end SWITCH
  } // end WHILE

  return true;
}

const bool
init_threadPool()
{
  ACE_TRACE(ACE_TEXT("::init_threadPool"));

  ACE_TP_Reactor* threadpool_reactor = NULL;
  ACE_NEW_RETURN(threadpool_reactor,
                 ACE_TP_Reactor(),
                 false);
  ACE_Reactor* new_reactor = NULL;
  ACE_NEW_RETURN(new_reactor,
                 ACE_Reactor(threadpool_reactor, 1), // delete in dtor
                 false);
  // make this the "default" reactor...
  ACE_Reactor::instance(new_reactor, 1); // delete in dtor

  return true;
}

static
ACE_THR_FUNC_RETURN
tp_worker_func(void* args_in)
{
  ACE_TRACE(ACE_TEXT("::tp_worker_func"));

  ACE_UNUSED_ARG(args_in);

  while (!ACE_Reactor::event_loop_done())
  {
    // block and wait for an event...
    if (ACE_Reactor::instance()->handle_events(NULL) == -1)
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("(%t) error handling events: \"%m\"\n")));
  } // end WHILE

  ACE_ERROR((LM_DEBUG,
             ACE_TEXT("(%t) worker leaving...\n")));

  return 0;
}

static
ACE_THR_FUNC_RETURN
reactor_worker_func(void* args_in)
{
  ACE_TRACE(ACE_TEXT("::reactor_worker_func"));

  ACE_UNUSED_ARG(args_in);

  // assume ownership over the reactor...
  ACE_Reactor::instance()->owner(ACE_OS::thr_self(),
                                 NULL);

  if (ACE_Reactor::instance()->run_reactor_event_loop() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Reactor::run_reactor_event_loop(): \"%m\", aborting\n")));
  } // end IF

  ACE_ERROR((LM_DEBUG,
             ACE_TEXT("(%t) worker leaving...\n")));

  return 0;
}

const bool
do_initAudio(const SDL_audio_config_t& audioConfig_in)
{
  ACE_TRACE(ACE_TEXT("::do_initAudio"));

  // init SDL audio handling
//   SDL_AudioSpec wanted;
//   wanted.freq = audioConfig_in.frequency;
//   wanted.format = audioConfig_in.format;
//   wanted.channels = audioConfig_in.channels;
//   wanted.samples = audioConfig_in.samples;
// //   wanted.callback = fill_audio;
// //   wanted.userdata = NULL;
//
//   // Open the audio device, forcing the desired format
//   if (SDL_OpenAudio(&wanted, NULL) < 0)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to SDL_OpenAudio(): \"%s\", aborting\n"),
//                SDL_GetError()));
//
//     return;
//   } // end IF
  if (Mix_OpenAudio(audioConfig_in.frequency,
                    audioConfig_in.format,
                    audioConfig_in.channels,
                    audioConfig_in.samples) < 0)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to Mix_OpenAudio(): \"%s\", aborting\n"),
               SDL_GetError()));

    return false;
  } // end IF
//   Mix_AllocateChannels(4);
  SDL_audio_config_t obtained;
  obtained.frequency = 0;
  obtained.format = 0;
  obtained.channels = 0;
  obtained.samples = 0;
  if (Mix_QuerySpec(&obtained.frequency,
                    &obtained.format,
                    &obtained.channels) == 0)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to Mix_QuerySpec(): \"%s\", aborting\n"),
               SDL_GetError()));

    return false;
  } // end IF
  char driver[MAXPATHLEN];
  if (!SDL_AudioDriverName(driver, sizeof(driver)))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_AudioDriverName(): \"%s\", aborting\n"),
               SDL_GetError()));

    return false;
  } // end IF

  // initialize audioCD playing
  if (SDL_CDNumDrives() <= 0)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_CDNumDrives(): \"%s\", aborting\n"),
               SDL_GetError()));

    return false;
  } // end IF
  SDL_CD* cdrom = NULL;
  cdrom = SDL_CDOpen(0); // open default drive

  ACE_DEBUG((LM_INFO,
             ACE_TEXT("*** audio capabilities (driver: \"%s\") ***\nfrequency: %d\nformat: %u\nchannels: %u\nCD [id, status]: \"%s\" [%d, %d]\n"),
             driver,
             obtained.frequency,
             ACE_static_cast(unsigned long, obtained.format),
             ACE_static_cast(unsigned long, obtained.channels),
             SDL_CDName(0),
             cdrom->id,
             cdrom->status));

  return true;
}

const bool
do_initGUI(const std::string& graphicsDirectory_in,
           const std::string& UIfile_in,
           const GTK_cb_data_t& userData_in,
           const SDL_video_config_t& videoConfig_in)
{
  ACE_TRACE(ACE_TEXT("::do_initGUI"));

  // sanity check(s)
  if (!RPG_Common_File_Tools::isDirectory(graphicsDirectory_in.c_str()))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid directory \"%s\", aborting\n"),
               graphicsDirectory_in.c_str()));

    return false;
  } // end IF
  if (!RPG_Common_File_Tools::isReadable(UIfile_in.c_str()))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("UI definition file \"%s\" doesn't exist, aborting\n"),
               UIfile_in.c_str()));

    return false;
  } // end IF
  if (xml)
  {
    // clean up
    g_object_unref(G_OBJECT(xml));
    xml = NULL;
  } // end IF
  if (main_dialog)
  {
    // clean up
    gtk_widget_destroy(main_dialog);
    main_dialog = NULL;
  } // end IF

  // step1: load widget tree
  xml = glade_xml_new(UIfile_in.c_str(), // definition file
                      NULL,              // root widget --> construct all
                      NULL);             // domain
  ACE_ASSERT(xml);
  if (!xml)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_new(): \"%m\", aborting\n")));

    return false;
  } // end IF

  // step2: retrieve dialog handles
  main_dialog = GTK_WIDGET(glade_xml_get_widget(xml,
                                                RPG_CLIENT_DEF_GNOME_MAINDIALOG_NAME));
  ACE_ASSERT(main_dialog);
  if (!main_dialog)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
               RPG_CLIENT_DEF_GNOME_MAINDIALOG_NAME));

    // clean up
    g_object_unref(G_OBJECT(xml));
    xml = NULL;

    return false;
  } // end IF

  GtkWidget* about_dialog = GTK_WIDGET(glade_xml_get_widget(xml,
                                                            RPG_CLIENT_DEF_GNOME_ABOUTDIALOG_NAME));
  ACE_ASSERT(about_dialog);
  if (!about_dialog)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
               RPG_CLIENT_DEF_GNOME_ABOUTDIALOG_NAME));

    // clean up
    g_object_unref(G_OBJECT(xml));
    xml = NULL;

    return false;
  } // end IF

  // step3: populate combobox
  GtkComboBox* available_characters = GTK_COMBO_BOX(glade_xml_get_widget(xml,
                                                                         RPG_CLIENT_DEF_GNOME_CHARBOX_NAME));
  ACE_ASSERT(available_characters);
  if (!available_characters)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
               RPG_CLIENT_DEF_GNOME_CHARBOX_NAME));

    // clean up
    g_object_unref(G_OBJECT(xml));
    xml = NULL;

    return false;
  } // end IF
  gtk_cell_layout_clear(GTK_CELL_LAYOUT(available_characters));
  GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
  ACE_ASSERT(renderer);
  if (!renderer)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_cell_renderer_text_new(): \"%m\", aborting\n")));

    // clean up
    g_object_unref(G_OBJECT(xml));
    xml = NULL;

    return false;
  } // end IF
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(available_characters), renderer,
                             TRUE); // expand ?
//   gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(available_characters), renderer,
//                                 ACE_TEXT_ALWAYS_CHAR("text"), 0);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(available_characters), renderer,
                                 ACE_TEXT_ALWAYS_CHAR("text"), 0,
                                 NULL);
  GtkListStore* list = gtk_list_store_new(1,
                                          G_TYPE_STRING);
  ACE_ASSERT(list);
  if (!list)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_list_store_new(): \"%m\", aborting\n")));

    // clean up
    g_object_unref(G_OBJECT(xml));
    xml = NULL;

    return false;
  } // end IF
  ::load_character_profiles(RPG_CLIENT_DEF_CHARACTER_REPOSITORY,
                            list);
  gtk_combo_box_set_model(available_characters,
                          GTK_TREE_MODEL(list));
  g_object_unref(G_OBJECT(list));
  gtk_widget_set_sensitive(GTK_WIDGET(available_characters),
                           (g_list_length(gtk_container_get_children(GTK_CONTAINER(available_characters))) ? TRUE
                                                                                                           : FALSE));

  // step4a: connect default signals
  gpointer userData_p = ACE_const_cast(GTK_cb_data_t*, &userData_in);
  g_signal_connect(main_dialog,
                   ACE_TEXT_ALWAYS_CHAR("destroy"),
                   G_CALLBACK(quit_activated_GTK_cb),
//                    G_CALLBACK(gtk_widget_destroyed),
//                    &main_dialog,
                   userData_p);
  g_signal_connect(about_dialog,
                   ACE_TEXT_ALWAYS_CHAR("response"),
                   G_CALLBACK(gtk_widget_hide),
                   &about_dialog);

   // step4b: connect custom signals
  GtkButton* button = NULL;
  button = GTK_BUTTON(glade_xml_get_widget(xml,
                                           ACE_TEXT_ALWAYS_CHAR("create")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(create_character_activated_GTK_cb),
                   userData_p);
//   glade_xml_signal_connect_data(xml,
//                                 ACE_TEXT_ALWAYS_CHAR("create_character_activated_GTK_cb"),
//                                 G_CALLBACK(create_character_activated_GTK_cb),
//                                 userData_p);

  GtkFileChooser* filechooser = NULL;
  filechooser = GTK_FILE_CHOOSER(glade_xml_get_widget(xml,
                                                      RPG_CLIENT_DEF_GNOME_FILECHOOSERDIALOG_NAME));
  ACE_ASSERT(filechooser);
  g_signal_connect(filechooser,
                   ACE_TEXT_ALWAYS_CHAR("file-activated"),
                   G_CALLBACK(character_file_activated_GTK_cb),
                   userData_p);
//   glade_xml_signal_connect_data(xml,
//                                 ACE_TEXT_ALWAYS_CHAR("character_file_activated_GTK_cb"),
//                                 G_CALLBACK(character_file_activated_GTK_cb),
//                                 userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(xml,
                                           ACE_TEXT_ALWAYS_CHAR("load")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(load_character_activated_GTK_cb),
                   userData_p);
//   glade_xml_signal_connect_data(xml,
//                                 ACE_TEXT_ALWAYS_CHAR("load_character_activated_GTK_cb"),
//                                 G_CALLBACK(load_character_activated_GTK_cb),
//                                 userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(xml,
                                           ACE_TEXT_ALWAYS_CHAR("save")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(save_character_activated_GTK_cb),
                   userData_p);
//   glade_xml_signal_connect_data(xml,
//                                 ACE_TEXT_ALWAYS_CHAR("save_character_activated_GTK_cb"),
//                                 G_CALLBACK(save_character_activated_GTK_cb),
//                                 userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(xml,
                                           ACE_TEXT_ALWAYS_CHAR("join")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(join_game_activated_GTK_cb),
                   userData_p);
//   glade_xml_signal_connect_data(xml,
//                                 ACE_TEXT_ALWAYS_CHAR("join_game_activated_GTK_cb"),
//                                 G_CALLBACK(join_game_activated_GTK_cb),
//                                 userData_p);

  GtkComboBox* combobox = NULL;
  combobox = GTK_COMBO_BOX(glade_xml_get_widget(xml,
                                                RPG_CLIENT_DEF_GNOME_CHARBOX_NAME));
  ACE_ASSERT(combobox);
  g_signal_connect(combobox,
                   ACE_TEXT_ALWAYS_CHAR("changed"),
                   G_CALLBACK(characters_activated_GTK_cb),
                   userData_p);
//   glade_xml_signal_connect_data(xml,
//                                 ACE_TEXT_ALWAYS_CHAR("characters_activated_GTK_cb"),
//                                 G_CALLBACK(characters_activated_GTK_cb),
//                                 userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(xml,
                                           ACE_TEXT_ALWAYS_CHAR("refresh")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(characters_refresh_activated_GTK_cb),
                   userData_p);
//   glade_xml_signal_connect_data(xml,
//                                 ACE_TEXT_ALWAYS_CHAR("characters_refresh_activated_GTK_cb"),
//                                 G_CALLBACK(characters_refresh_activated_GTK_cb),
//                                 userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(xml,
                                           ACE_TEXT_ALWAYS_CHAR("quit")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(quit_activated_GTK_cb),
                   userData_p);
//   glade_xml_signal_connect_data(xml,
//                                 ACE_TEXT_ALWAYS_CHAR("quit_activated_GTK_cb"),
//                                 G_CALLBACK(quit_activated_GTK_cb),
//                                 userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(xml,
                                           ACE_TEXT_ALWAYS_CHAR("about")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(about_activated_GTK_cb),
                   userData_p);
//   glade_xml_signal_connect_data(xml,
//                                 ACE_TEXT_ALWAYS_CHAR("about_activated_GTK_cb"),
//                                 G_CALLBACK(about_activated_GTK_cb),
//                                 userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(xml,
                                           ACE_TEXT_ALWAYS_CHAR("properties")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(properties_activated_GTK_cb),
                   userData_p);
//   glade_xml_signal_connect_data(xml,
//                                 ACE_TEXT_ALWAYS_CHAR("properties_activated_GTK_cb"),
//                                 G_CALLBACK(properties_activated_GTK_cb),
//                                 userData_p);

  // step5: auto-connect signals/slots
//   glade_xml_signal_autoconnect(xml);

//   // step6: use correct screen
//   if (parentWidget_in)
//     gtk_window_set_screen(GTK_WINDOW(dialog),
//                           gtk_widget_get_screen(ACE_const_cast(GtkWidget*,
//                                                                parentWidget_in)));

  // step6: draw it
  gtk_widget_show_all(main_dialog);

  // step7: activate first repository entry (if any)
  if (gtk_widget_is_sensitive(GTK_WIDGET(available_characters)))
    gtk_combo_box_set_active(available_characters, 0);

  // init SDL UI handling

  // ***** keyboard setup *****
  // enable Unicode translation
  SDL_EnableUNICODE(1);
  // enable key repeat
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,
                      SDL_DEFAULT_REPEAT_INTERVAL);
//   // ignore keyboard events
//   SDL_EventState(SDL_KEYDOWN, SDL_IGNORE);
//   SDL_EventState(SDL_KEYUP, SDL_IGNORE);

  // SDL event filter (filter mouse motion events and the like)
//   SDL_SetEventFilter(event_filter_SDL_cb);

  // ***** window/screen setup *****
  // set window caption
  SDL_WM_SetCaption(ACE_TEXT_ALWAYS_CHAR(PACKAGE_STRING),  // window caption
                    ACE_TEXT_ALWAYS_CHAR(PACKAGE_STRING)); // icon caption
  // set window icon
  RPG_Graphics_t icon_graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->getGraphic(TYPE_IMAGE_WM_ICON);
  ACE_ASSERT(icon_graphic.type == TYPE_IMAGE_WM_ICON);
  std::string path = graphicsDirectory_in;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += icon_graphic.file;
  SDL_Surface* icon_image = NULL;
  icon_image = RPG_Graphics_Surface::load(path,   // graphics file
                                          false); // don't convert to display format (no screen yet !)
  if (!icon_image)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadFile(\"%s\"), aborting\n"),
               path.c_str()));

    return false;
  } // end IF
  SDL_WM_SetIcon(icon_image, // surface
                 NULL);      // mask (--> everything)
//   // don't show (double) cursor
//   SDL_ShowCursor(SDL_DISABLE);

  char driver[MAXPATHLEN];
  if (!SDL_VideoDriverName(driver,
                           sizeof(driver)))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_VideoDriverName(): \"%s\", aborting\n"),
               SDL_GetError()));

    return false;
  } // end IF
  // retrieve/list "best" available video mode
  const SDL_VideoInfo* videoInfo = NULL;
  videoInfo = SDL_GetVideoInfo();
  ACE_ASSERT(videoInfo);

  ACE_DEBUG((LM_INFO,
             ACE_TEXT("*** video capabilities (driver: \"%s\") ***\nhardware surfaces: \"%s\"\nwindow manager: \"%s\"\nhardware to hardware blits accelerated: \"%s\"\nhardware to hardware colorkey blits accelerated: \"%s\"\nhardware to hardware alpha blits accelerated: \"%s\"\nsoftware to hardware blits accelerated: \"%s\"\nsoftware to hardware colorkey blits accelerated: \"%s\"\nsoftware to hardware alpha blits accelerated: \"%s\"\ncolor fills accelerated: \"%s\"\nvideo memory: %d bytes\n*** (suggested) video mode ***\npalette: %@\nbits[bytes]/pixel: %d[%d]\nmask[RGBA]: %x %x %x %x\nshift[RGBA]: %d %d %d %d\nloss[RGBA]: %d %d %d %d\ntransparent colorkey: %d\noverall surface alpha: %d\n"),
             driver,
             (videoInfo->hw_available ? ACE_TEXT("yes") : ACE_TEXT("no")),
             (videoInfo->wm_available ? ACE_TEXT("yes") : ACE_TEXT("no")),
             (videoInfo->blit_hw ? ACE_TEXT("yes") : ACE_TEXT("no")),
             (videoInfo->blit_hw_CC ? ACE_TEXT("yes") : ACE_TEXT("no")),
             (videoInfo->blit_hw_A ? ACE_TEXT("yes") : ACE_TEXT("no")),
             (videoInfo->blit_sw ? ACE_TEXT("yes") : ACE_TEXT("no")),
             (videoInfo->blit_sw_CC ? ACE_TEXT("yes") : ACE_TEXT("no")),
             (videoInfo->blit_sw_A ? ACE_TEXT("yes") : ACE_TEXT("no")),
             (videoInfo->blit_fill ? ACE_TEXT("yes") : ACE_TEXT("no")),
             videoInfo->video_mem,
             videoInfo->vfmt->palette,
             videoInfo->vfmt->BitsPerPixel,
             videoInfo->vfmt->BytesPerPixel,
             videoInfo->vfmt->Rmask, videoInfo->vfmt->Gmask, videoInfo->vfmt->Bmask, videoInfo->vfmt->Amask,
             videoInfo->vfmt->Rshift, videoInfo->vfmt->Gshift, videoInfo->vfmt->Bshift, videoInfo->vfmt->Ashift,
             videoInfo->vfmt->Rloss, videoInfo->vfmt->Gloss, videoInfo->vfmt->Bloss, videoInfo->vfmt->Aloss,
             videoInfo->vfmt->colorkey,
             ACE_static_cast(int, videoInfo->vfmt->alpha)));

  // set flags
  Uint32 surface_flags = ((videoInfo->hw_available ? (SDL_HWSURFACE |
                                                      (videoConfig_in.doubleBuffer ? SDL_DOUBLEBUF : SDL_ANYFORMAT))
                                                   : SDL_SWSURFACE) |
//                           SDL_ASYNCBLIT | // "...will usually slow down blitting on single CPU machines,
//                                           //  but may provide a speed increase on SMP systems..."
                          SDL_ANYFORMAT |    // "...Allow any video depth/pixel-format..."
                          SDL_HWPALETTE |    // "...Surface has exclusive palette..."
//                           (videoConfig_in.doubleBuffer ? SDL_DOUBLEBUF : SDL_ANYFORMAT) |
                          (videoConfig_in.fullScreen ? (SDL_FULLSCREEN | SDL_NOFRAME) : SDL_RESIZABLE));
  // get available fullscreen/hardware/... modes
  SDL_Rect** modes = NULL;
  modes = SDL_ListModes(NULL,           // use same as videoInfo
                        surface_flags); // surface flags
  // --> any valid modes available ?
  if (modes == NULL)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("no available resolutions (flags: %x: change your settings), aborting\n"),
               surface_flags));

    return false;
  } // end IF
  else if (modes == ACE_reinterpret_cast(SDL_Rect**, -1))
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("ALL resolutions available (flags: %x)...\n"),
               surface_flags));
  }
  else
  {
    // print valid modes
    ACE_DEBUG((LM_INFO,
               ACE_TEXT("*** available resolutions [flags: %x] ***\n"),
               surface_flags));
    for (unsigned int i = 0;
         modes[i];
         i++)
      ACE_DEBUG((LM_INFO,
                 ACE_TEXT("[%d]: %d x %d\n"),
                 i + 1,
                 modes[i]->w,
                 modes[i]->h));
  } // end ELSE

  // check to see whether the requested mode is possible
  int suggested_bpp = SDL_VideoModeOK(videoConfig_in.screen_width,
                                      videoConfig_in.screen_height,
                                      videoConfig_in.screen_colordepth,
                                      surface_flags);
  switch (suggested_bpp)
  {
    case 0:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("requested mode (width: %d, height: %d, depth: %d, flags: %x) not supported (change your settings), aborting\n"),
                 videoConfig_in.screen_width,
                 videoConfig_in.screen_height,
                 videoConfig_in.screen_colordepth,
                 surface_flags));

      return false;
    }
    default:
    {
      if (suggested_bpp != videoConfig_in.screen_colordepth)
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("suggested depth: %d...\n"),
                   suggested_bpp));
      } // end IF

      break;
    }
  } // end SWITCH

  // switch to graphics mode... here we go !
  screen = SDL_SetVideoMode(videoConfig_in.screen_width,
                            videoConfig_in.screen_height,
                            videoConfig_in.screen_colordepth,
                            surface_flags);
  if (!screen)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetVideoMode(%d, %d, %d, %x): \"%s\", aborting\n"),
               videoConfig_in.screen_width,
               videoConfig_in.screen_height,
               videoConfig_in.screen_colordepth,
               surface_flags,
               SDL_GetError()));

    return false;
  } // end IF

  ACE_DEBUG((LM_INFO,
             ACE_TEXT("*** screen flags ***\nsurface: %sRAM\nasynch blits: \"%s\"\nany video depth/pixel-format: \"%s\"\nsurface has exclusive palette: \"%s\"\ndouble-buffered: \"%s\"\nblit uses hardware acceleration: \"%s\"\nblit uses a source color key: \"%s\"\nsurface is RLE encoded: \"%s\"\nblit uses source alpha blending: \"%s\"\nsurface uses preallocated memory: \"%s\"\n"),
             ((screen->flags & SDL_HWSURFACE) ? ACE_TEXT("Video") : ACE_TEXT("")),
             ((screen->flags & SDL_ASYNCBLIT) ? ACE_TEXT("yes") : ACE_TEXT("no")),
             ((screen->flags & SDL_ANYFORMAT) ? ACE_TEXT("yes") : ACE_TEXT("no")),
             ((screen->flags & SDL_HWPALETTE) ? ACE_TEXT("yes") : ACE_TEXT("no")),
             ((screen->flags & SDL_DOUBLEBUF) ? ACE_TEXT("yes") : ACE_TEXT("no")),
             ((screen->flags & SDL_HWACCEL) ? ACE_TEXT("yes") : ACE_TEXT("no")),
             ((screen->flags & SDL_SRCCOLORKEY) ? ACE_TEXT("yes") : ACE_TEXT("no")),
             ((screen->flags & SDL_RLEACCEL) ? ACE_TEXT("yes") : ACE_TEXT("no")),
             ((screen->flags & SDL_SRCALPHA) ? ACE_TEXT("yes") : ACE_TEXT("no")),
             ((screen->flags & SDL_PREALLOC) ? ACE_TEXT("yes") : ACE_TEXT("no"))));

  return true;
}

const bool
do_runIntro()
{
  ACE_TRACE(ACE_TEXT("::do_runIntro"));

  // step1: play intro music
  RPG_Sound_Common_Tools::playSound(EVENT_MAIN_TITLE);

  // step2: show start logo
  SDL_Surface* logo = RPG_Graphics_Common_Tools::loadGraphic(TYPE_IMAGE_INTRO_MAIN);
  if (!logo)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadGraphic(%s), aborting\n"),
               RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(TYPE_IMAGE_INTRO_MAIN).c_str()));

    return false;
  } // end IF
  // *TODO* stretch this image fullscreen
  // center logo image
  RPG_Graphics_Surface::put((screen->w - logo->w) / 2, // location x
                            (screen->h - logo->h) / 2, // location y
                            *logo,
                            screen);
//   SDL_FreeSurface(logo);
  RPG_Graphics_Common_Tools::fade(true,                                // fade in
                                  5.0,                                 // interval
                                  RPG_Graphics_SDL_Tools::CLR32_BLACK, // fade from black
                                  screen);                             // screen
  SDL_Event event;
  do_SDL_waitForInput(10,     // wait 10 seconds max
                      event);
//   do_handleSDLEvent(event);
  RPG_Graphics_Common_Tools::fade(false,                               // fade out
                                  3.0,                                 // interval
                                  RPG_Graphics_SDL_Tools::CLR32_BLACK, // fade to black
                                  screen);                             // screen

  return true;
}

void
do_work(const RPG_Client_Config& config_in)
{
  ACE_TRACE(ACE_TEXT("::do_work"));

  // step0a: init RPG engine
  RPG_Engine_Common_Tools::init(config_in.magic_dictionary,
                                config_in.item_dictionary,
                                config_in.monster_dictionary);
  RPG_Sound_Common_Tools::initStringConversionTables();
  RPG_Graphics_Common_Tools::initStringConversionTables();

  // step0b: (if necessary) init the TP_Reactor
  if (config_in.num_threadpool_threads)
  {
    if (!init_threadPool())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to init_threadPool(), aborting\n")));

      return;
    } // end IF
  } // end IF

  // step1: init audio
  RPG_Sound_Common_Tools::init(config_in.sound_directory,
                               config_in.sound_cache_size);
  //  init sound dictionary
  try
  {
    RPG_SOUND_DICTIONARY_SINGLETON::instance()->init(config_in.sound_dictionary);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Sound_Dictionary::init, returning\n")));

    return;
  }
  if (!do_initAudio(config_in.audio_config))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize audio, aborting\n")));

    return;
  } // end IF

  // step2: init UI
  // init graphics dictionary
  try
  {
    RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->init(config_in.graphics_dictionary);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Graphics_Dictionary::init, returning\n")));

    return;
  }
  if (!do_initGUI(config_in.graphics_directory, // graphics directory
                  config_in.glade_file,         // glade file
                  userData,                     // GTK cb data
                  config_in.video_config))      // SDL video config
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize video, aborting\n")));

    return;
  } // end IF
  ACE_ASSERT(xml);
  ACE_ASSERT(main_dialog);
  ACE_ASSERT(screen);
  RPG_Graphics_Common_Tools::init(config_in.graphics_directory,
                                  config_in.graphics_cache_size);

//   // step3: run intro
//   if (!do_runIntro())
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to run intro, aborting\n")));
//
//     return;
//   } // end IF

  // step4: setup main "window"
  std::string title = RPG_CLIENT_DEF_GRAPHICS_MAINWINDOW_TITLE;
  RPG_Client_WindowMain mainWindow(RPG_Graphics_WindowSize_t(screen->w,
                                                             screen->h),     // size
                                   RPG_CLIENT_DEF_GRAPHICS_WINDOWSTYLE_TYPE, // interface elements
                                   title,                                    // title (== caption)
                                   TYPE_FONT_MAIN_LARGE);                    // title font
  mainWindow.setScreen(screen);
  mainWindow.init();
  mainWindow.draw();
  mainWindow.refresh();

  // ***** mouse setup *****
  SDL_WarpMouse((screen->w / 2),
                (screen->h / 2));

  // step4b: set default cursor
  RPG_GRAPHICS_CURSOR_SINGLETON::instance()->set(TYPE_CURSOR_NORMAL);

  // step5: setup level "window"
  RPG_Client_WindowLevel mapWindow(mainWindow); // parent
  mapWindow.setScreen(screen);
  // refresh screen
  try
  {
    mapWindow.draw();
    mapWindow.refresh();
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Graphics_IWindow::draw()/refresh(), continuing\n")));
  }

  // step5: setup level
  // step5a: setup style
  RPG_Graphics_MapStyle_t mapStyle;
  mapStyle.floor_style = RPG_CLIENT_DEF_GRAPHICS_FLOORSTYLE;
  mapStyle.wall_style = RPG_CLIENT_DEF_GRAPHICS_WALLSTYLE;
  mapStyle.half_height_walls = RPG_CLIENT_DEF_GRAPHICS_WALLSTYLE_HALF;
  mapStyle.door_style = RPG_CLIENT_DEF_GRAPHICS_DOORSTYLE;

  // step5a: setup map
  userData.plan.size_x = 0;
  userData.plan.size_y = 0;
  userData.plan.unmapped.clear();
  userData.plan.walls.clear();
  userData.plan.doors.clear();
  userData.seedPoints.clear();
  if (config_in.map_file.empty())
    RPG_Map_Common_Tools::createFloorPlan(config_in.map_config.map_size_x,
                                          config_in.map_config.map_size_y,
                                          config_in.map_config.num_areas,
                                          config_in.map_config.square_rooms,
                                          config_in.map_config.maximize_rooms,
                                          config_in.map_config.min_room_size,
                                          config_in.map_config.doors,
                                          config_in.map_config.corridors,
                                          true, // *NOTE*: currently, doors fill one position
                                          config_in.map_config.max_num_doors_per_room,
                                          userData.seedPoints,
                                          userData.plan);
  else
  {
    if (!RPG_Map_Common_Tools::load(config_in.map_file,
                                    userData.seedPoints,
                                    userData.plan))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Map_Common_Tools::load(\"%s\"), aborting\n"),
                 config_in.map_file.c_str()));

      return;
    } // end IF
  } // end ELSE

//   // debug info
//   RPG_Map_Common_Tools::displayFloorPlan(userData.seedPoints,
//                                          userData.plan);

  mapWindow.init(mapStyle,
                 userData.plan);
  // refresh screen
  try
  {
    mapWindow.draw();
    mapWindow.refresh();
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Graphics_IWindow::draw()/refresh(), continuing\n")));
  }

  // start timer (triggers hover- and other events)
  userData.event_timer = NULL;
  userData.event_timer = SDL_AddTimer(RPG_CLIENT_SDL_EVENT_TIMEOUT, // interval (ms)
                                      event_timer_SDL_cb,           // event timer callback
                                      &hover_time);                 // callback argument
  if (!userData.event_timer)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_AddTimer(%u): \"%s\", aborting\n"),
               RPG_CLIENT_SDL_EVENT_TIMEOUT,
               SDL_GetError()));

    return;
  } // end IF

  // step6: setup event loops
  // - perform (signal handling, socket I/O, ...) --> ACE_Reactor
  // - UI events --> GTK main loop [--> SDL event handler]

//   // *NOTE*: make sure we generally restart system calls (after e.g. EINTR) for the reactor...
//   ACE_Reactor::instance()->restart(1);

  // setup dispatch of SDL events from the GTK (== "main") thread
  userData.previous_window = NULL;
  userData.main_window = &mainWindow;
  userData.map_window = &mapWindow;
  guint SDLEventHandlerID = gtk_idle_add(do_SDLEventLoop_GTK_cb,
                                         &userData);
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("installed SDL event handler (ID: %u)...\n"),
             SDLEventHandlerID));

  // setup dispatch of network events
  // *NOTE*: if we use a thread pool, we invoke a different function...
  // *CONSIDER*: potential source for races here
  // --> network connection events arrive before dispatch of events begins
  if (config_in.num_threadpool_threads)
  {
    // start a (group of) worker(s)...
    grp_id = ACE_Thread_Manager::instance()->spawn_n(config_in.num_threadpool_threads, // # threads
                                                     ::tp_worker_func,            // function
                                                     NULL,                        // argument
                                                     (THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED), // flags
                                                     ACE_DEFAULT_THREAD_PRIORITY, // priority
                                                     -1,                          // group id --> create new
                                                     NULL,                        // task
                                                     NULL,                        // handle(s)
                                                     NULL,                        // stack(s)
                                                     NULL,                        // stack size(s)
                                                     NULL);                       // name(s)
  } // end IF
  else
  {
    // start a worker...
    grp_id = ACE_Thread_Manager::instance()->spawn_n(1,                           // # threads
                                                     ::reactor_worker_func,       // function
                                                     NULL,                        // argument
                                                     (THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED), // flags
                                                     ACE_DEFAULT_THREAD_PRIORITY, // priority
                                                     -1,                          // group id --> create new
                                                     NULL,                        // task
                                                     NULL,                        // handle(s)
                                                     NULL,                        // stack(s)
                                                     NULL,                        // stack size(s)
                                                     NULL);                       // name(s)
  } // end ELSE
  if (grp_id == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Thread_Manager::spawn_n(%u): \"%m\", aborting\n"),
               (config_in.num_threadpool_threads ? config_in.num_threadpool_threads : 1)));

    return;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("started group (ID: %u) of %u worker thread(s)...\n"),
             grp_id,
             (config_in.num_threadpool_threads ? config_in.num_threadpool_threads : 1)));

  // step7: dispatch GTK (and SDL-) events
  gtk_main();

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished working...\n")));
}

void
do_parseIniFile(const std::string& iniFilename_in,
                RPG_Client_Config& config_out)
{
  ACE_TRACE(ACE_TEXT("::do_parseIniFile"));

  // init return value(s)
  config_out.audio_config.frequency = RPG_CLIENT_DEF_AUDIO_FREQUENCY;
  config_out.audio_config.format = RPG_CLIENT_DEF_AUDIO_FORMAT;
  config_out.audio_config.channels = RPG_CLIENT_DEF_AUDIO_CHANNELS;
  config_out.audio_config.samples = RPG_CLIENT_DEF_AUDIO_SAMPLES;

  config_out.video_config.screen_width = RPG_CLIENT_DEF_VIDEO_W;
  config_out.video_config.screen_height = RPG_CLIENT_DEF_VIDEO_H;
  config_out.video_config.screen_colordepth = RPG_CLIENT_DEF_VIDEO_BPP;
  config_out.video_config.fullScreen = RPG_CLIENT_DEF_VIDEO_FULLSCREEN;
  config_out.video_config.doubleBuffer = RPG_CLIENT_DEF_VIDEO_DOUBLEBUFFER;

  ACE_Configuration_Heap config_heap;
  if (config_heap.open())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_Configuration_Heap::open failed, returning\n")));

    return;
  } // end IF

  ACE_Ini_ImpExp import(config_heap);
  if (import.import_config(iniFilename_in.c_str()))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_Ini_ImpExp::import_config(\"%s\") failed, returning\n"),
               iniFilename_in.c_str()));

    return;
  } // end IF

  // find/open "client" section...
  ACE_Configuration_Section_Key section_key;
  if (config_heap.open_section(config_heap.root_section(),
                               RPG_CLIENT_CNF_CLIENT_SECTION_HEADER,
                               0, // MUST exist !
                               section_key) != 0)
  {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("failed to ACE_Configuration_Heap::open_section(%s), returning\n"),
               RPG_CLIENT_CNF_CLIENT_SECTION_HEADER));

    return;
  } // end IF

  // import values...
  int val_index = 0;
  ACE_TString val_name, val_value;
  ACE_Configuration::VALUETYPE val_type;
  while (config_heap.enumerate_values(section_key,
                                      val_index,
                                      val_name,
                                      val_type) == 0)
  {
    if (config_heap.get_string_value(section_key,
                                     val_name.c_str(),
                                     val_value))
    {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("failed to ACE_Configuration_Heap::get_string_value(%s), returning\n"),
                 val_name.c_str()));

      return;
    } // end IF

//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("enumerated %s, type %d\n"),
//                val_name.c_str(),
//                val_type));

    // *TODO*: move these strings...
    if (val_name == ACE_TEXT("audio_frequency"))
    {
      config_out.audio_config.frequency = ::atoi(val_value.c_str());
    }
//     else if (val_name == ACE_TEXT("audio_format"))
//     {
//       audioConfig_out.format = ::atoi(val_value.c_str());
//     }
    else if (val_name == ACE_TEXT("audio_channels"))
    {
      config_out.audio_config.channels = ::atoi(val_value.c_str());
    }
    else if (val_name == ACE_TEXT("audio_samples"))
    {
      config_out.audio_config.samples = ::atoi(val_value.c_str());
    }
    else if (val_name == ACE_TEXT("screen_width"))
    {
      config_out.video_config.screen_width = ::atoi(val_value.c_str());
    }
    else if (val_name == ACE_TEXT("screen_height"))
    {
      config_out.video_config.screen_height = ::atoi(val_value.c_str());
    }
    else if (val_name == ACE_TEXT("screen_colordepth"))
    {
      config_out.video_config.screen_colordepth = ::atoi(val_value.c_str());
    }
    else
    {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("unexpected key \"%s\", continuing\n"),
                 val_name.c_str()));
    } // end ELSE

    ++val_index;
  } // end WHILE

//   // find/open "connection" section...
//   if (config_heap.open_section(config_heap.root_section(),
//                                RPG_CLIENT_CNF_CONNECTION_SECTION_HEADER,
//                                0, // MUST exist !
//                                section_key) != 0)
//   {
//     ACE_ERROR((LM_ERROR,
//                ACE_TEXT("failed to ACE_Configuration_Heap::open_section(%s), returning\n"),
//                RPG_CLIENT_CNF_CONNECTION_SECTION_HEADER));
//
//     return;
//   } // end IF
//
//   // import values...
//   val_index = 0;
//   while (config_heap.enumerate_values(section_key,
//                                       val_index,
//                                       val_name,
//                                       val_type) == 0)
//   {
//     if (config_heap.get_string_value(section_key,
//                                      val_name.c_str(),
//                                      val_value))
//     {
//       ACE_ERROR((LM_ERROR,
//                  ACE_TEXT("failed to ACE_Configuration_Heap::get_string_value(%s), returning\n"),
//                  val_name.c_str()));
//
//       return;
//     } // end IF
//
// //     ACE_DEBUG((LM_DEBUG,
// //                ACE_TEXT("enumerated %s, type %d\n"),
// //                val_name.c_str(),
// //                val_type));
//
//     // *TODO*: move these strings...
//     if (val_name == ACE_TEXT("server"))
//     {
//       serverHostname_out = val_value.c_str();
//     }
//     else if (val_name == ACE_TEXT("port"))
//     {
//       serverPortNumber_out = ::atoi(val_value.c_str());
//     }
//     else if (val_name == ACE_TEXT("password"))
//     {
//       loginOptions_out.password = val_value.c_str();
//     }
//     else if (val_name == ACE_TEXT("nick"))
//     {
//       loginOptions_out.nick = val_value.c_str();
//     }
//     else if (val_name == ACE_TEXT("user"))
//     {
//       loginOptions_out.user.username = val_value.c_str();
//     }
//     else if (val_name == ACE_TEXT("realname"))
//     {
//       loginOptions_out.user.realname = val_value.c_str();
//     }
//     else if (val_name == ACE_TEXT("channel"))
//     {
//       loginOptions_out.channel = val_value.c_str();
//     }
//     else
//     {
//       ACE_ERROR((LM_ERROR,
//                  ACE_TEXT("unexpected key \"%s\", continuing\n"),
//                  val_name.c_str()));
//     } // end ELSE
//
//     ++val_index;
//   } // end WHILE

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("imported \"%s\"...\n"),
//              configFilename_in.c_str()));
}

void
do_printVersion(const std::string& programName_in)
{
  ACE_TRACE(ACE_TEXT("::do_printVersion"));

//   std::cout << programName_in << ACE_TEXT(" : ") << VERSION << std::endl;
  std::cout << programName_in << ACE_TEXT(" : ") << GUI_VERSION << std::endl;

  // create version string...
  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta version
  // number... We need this, as the library soname is compared to this string.
  std::ostringstream version_number;
  version_number << ACE::major_version();
  version_number << ACE_TEXT(".");
  version_number << ACE::minor_version();
  if (ACE::beta_version())
  {
    version_number << ACE_TEXT(".");
    version_number << ACE::beta_version();
  } // end IF

  std::cout << ACE_TEXT("ACE: ") << version_number.str() << std::endl;
//   std::cout << "ACE: "
//             << ACE_VERSION
//             << std::endl;
}

int
ACE_TMAIN(int argc_in,
          ACE_TCHAR* argv_in[])
{
  ACE_TRACE(ACE_TEXT("::main"));

  // step0: init ACE
  // *PORTABILITY*: on Windows, we need to init ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::init() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::init(): \"%m\", aborting\n")));

    return EXIT_FAILURE;
  } // end IF
#endif

  // *PROCESS PROFILE*
  ACE_Profile_Timer process_profile;
  // start profile timer...
  process_profile.start();

  // step1 init/validate configuration

  // step1a: process commandline arguments
  std::string iniFile                = RPG_CLIENT_DEF_INI_FILE;
  std::string monsterDictionary      = RPG_CLIENT_DEF_MONSTER_DICTIONARY;
  std::string graphicsDictionary     = RPG_CLIENT_DEF_GRAPHICS_DICTIONARY;
  std::string floorPlan;
  std::string itemDictionary         = RPG_CLIENT_DEF_ITEM_DICTIONARY;
  bool logToFile                     = false;
  std::string magicDictionary        = RPG_CLIENT_DEF_MAGIC_DICTIONARY;
  std::string soundDictionary        = RPG_CLIENT_DEF_SOUND_DICTIONARY;
  bool traceInformation              = false;
  std::string UIfile                 = RPG_CLIENT_DEF_GNOME_UI_FILE;
  bool printVersionAndExit           = false;
  unsigned long numThreadPoolThreads = (RPG_CLIENT_DEF_ACE_USES_TP ? RPG_CLIENT_DEF_ACE_NUM_TP_THREADS : 0);
  if (!(process_arguments(argc_in,
                          argv_in,
                          iniFile,
                          monsterDictionary,
                          floorPlan,
                          graphicsDictionary,
                          itemDictionary,
                          logToFile,
                          magicDictionary,
                          soundDictionary,
                          traceInformation,
                          UIfile,
                          printVersionAndExit,
                          numThreadPoolThreads)))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv_in[0])));

    // *PORTABILITY*: on Windows, we must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step1b: handle specific program modes
  if (printVersionAndExit)
  {
    do_printVersion(std::string(ACE::basename(argv_in[0])));

    return EXIT_SUCCESS;
  } // end IF

  // step1c: set correct trace level
  //ACE_Trace::start_tracing();
  if (!traceInformation)
  {
    u_long process_priority_mask = (LM_SHUTDOWN |
                                    //LM_TRACE |  // <-- DISABLE trace messages !
                                    //LM_DEBUG |
                                    LM_INFO |
                                    LM_NOTICE |
                                    LM_WARNING |
                                    LM_STARTUP |
                                    LM_ERROR |
                                    LM_CRITICAL |
                                    LM_ALERT |
                                    LM_EMERGENCY);

    // set new mask...
    ACE_LOG_MSG->priority_mask(process_priority_mask,
                               ACE_Log_Msg::PROCESS);

    //ACE_LOG_MSG->stop_tracing();

    // don't go VERBOSE...
    //ACE_LOG_MSG->clr_flags(ACE_Log_Msg::VERBOSE_LITE);
  } // end IF

  // step1da: init GTK callback user data
  userData.plan.size_x                     = 0;
  userData.plan.size_y                     = 0;
  userData.plan.unmapped.clear();
  userData.plan.walls.clear();
  userData.plan.doors.clear();
  userData.seedPoints.clear();
  userData.event_timer                     = NULL;
  userData.previous_window                 = NULL;
  userData.main_window                     = NULL;
  userData.map_window                      = NULL;

  // step1db: init configuration object
  RPG_Client_Config config;

  // *** reactor ***
  config.num_threadpool_threads            = numThreadPoolThreads;

  // *** UI ***
  config.glade_file                        = UIfile;
//   config.gtk_cb_data                       = userData;

  // *** sound ***
  config.audio_config.frequency            = RPG_CLIENT_DEF_AUDIO_FREQUENCY;
  config.audio_config.format               = RPG_CLIENT_DEF_AUDIO_FORMAT;
  config.audio_config.channels             = RPG_CLIENT_DEF_AUDIO_CHANNELS;
  config.audio_config.samples              = RPG_CLIENT_DEF_AUDIO_SAMPLES;
  config.sound_directory                   = RPG_CLIENT_DEF_SOUND_DIRECTORY;
  config.sound_cache_size                  = RPG_CLIENT_DEF_SOUND_CACHESIZE;
  config.sound_dictionary                  = soundDictionary;

  // *** graphics ***
  config.video_config.screen_width         = RPG_CLIENT_DEF_VIDEO_W;
  config.video_config.screen_height        = RPG_CLIENT_DEF_VIDEO_H;
  config.video_config.screen_colordepth    = RPG_CLIENT_DEF_VIDEO_BPP;
  config.video_config.fullScreen           = RPG_CLIENT_DEF_VIDEO_FULLSCREEN;
  config.video_config.doubleBuffer         = RPG_CLIENT_DEF_VIDEO_DOUBLEBUFFER;
  config.graphics_directory                = RPG_CLIENT_DEF_GRAPHICS_DIRECTORY;
  config.graphics_cache_size               = RPG_CLIENT_DEF_GRAPHICS_CACHESIZE;
  config.graphics_dictionary               = graphicsDictionary;

  // *** magic ***
  config.magic_dictionary                  = magicDictionary;

  // *** item ***
  config.item_dictionary                  = itemDictionary;

  // *** monster ***
  config.monster_dictionary               = monsterDictionary;

  // *** map ***
  config.map_config.min_room_size          = RPG_CLIENT_DEF_MAP_MIN_ROOM_SIZE;
  config.map_config.doors                  = RPG_CLIENT_DEF_MAP_DOORS;
  config.map_config.corridors              = RPG_CLIENT_DEF_MAP_CORRIDORS;
  config.map_config.max_num_doors_per_room = RPG_CLIENT_DEF_MAP_MAX_NUM_DOORS_PER_ROOM;
  config.map_config.maximize_rooms         = RPG_CLIENT_DEF_MAP_MAXIMIZE_ROOMS;
  config.map_config.num_areas              = RPG_CLIENT_DEF_MAP_NUM_AREAS;
  config.map_config.square_rooms           = RPG_CLIENT_DEF_MAP_SQUARE_ROOMS;
  config.map_config.map_size_x             = RPG_CLIENT_DEF_MAP_SIZE_X;
  config.map_config.map_size_y             = RPG_CLIENT_DEF_MAP_SIZE_Y;
  config.map_file                          = floorPlan;

//   // step1dc: populate config object with default/collected data
//   // ************ connection config data ************
//   config.socketBufferSize = RPG_NET_DEF_SOCK_RECVBUF_SIZE;
//   config.messageAllocator = &messageAllocator;
//   config.defaultBufferSize = RPG_NET_PROTOCOL_DEF_NETWORK_BUFFER_SIZE;
//   // ************ protocol config data **************
//   config.clientPingInterval = 0; // servers do this...
// //   config.loginOptions = userData.loginOptions;
//   // ************ stream config data ****************
//   config.debugParser = debugParser;
//   config.module = &IRChandlerModule;
//   // *WARNING*: set at runtime, by the appropriate connection handler
//   config.sessionID = 0; // (== socket handle !)
//   config.statisticsReportingInterval = 0; // == off

  // step1de: parse .ini file (if any)
  if (!iniFile.empty())
    do_parseIniFile(iniFile,
                    config);

  // step2a: init SDL
  if (SDL_Init(SDL_INIT_TIMER | // timers
               SDL_INIT_AUDIO |
               SDL_INIT_VIDEO |
               SDL_INIT_CDROM | // audioCD playback
               SDL_INIT_NOPARACHUTE) == -1) // "...Prevents SDL from catching fatal signals..."
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_Init(): \"%s\", aborting\n"),
               SDL_GetError()));

    // *PORTABILITY*: on Windows, we must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF
  if (TTF_Init() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to TTF_Init(): \"%s\", aborting\n"),
               SDL_GetError()));

    SDL_Quit();
    // *PORTABILITY*: on Windows, we must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step2b: init GTK / GTK+ / GNOME
  gtk_init(&argc_in,
           &argv_in);
//   GnomeClient* gnomeSession = NULL;
//   gnomeSession = gnome_client_new();
//   ACE_ASSERT(gnomeSession);
//   gnome_client_set_program(gnomeSession, ACE::basename(argv_in[0]));
  GnomeProgram* gnomeProgram = NULL;
  gnomeProgram = gnome_program_init(RPG_CLIENT_DEF_GNOME_APPLICATION_ID, // app ID
//                                     ACE_TEXT_ALWAYS_CHAR(VERSION),       // version
                                    ACE_TEXT_ALWAYS_CHAR(GUI_VERSION),   // version
                                    LIBGNOMEUI_MODULE,                   // module info
                                    argc_in,                             // cmdline
                                    argv_in,                             // cmdline
                                    NULL);                               // property name(s)
  ACE_ASSERT(gnomeProgram);

  ACE_High_Res_Timer timer;
  timer.start();
  // step3: do actual work
  do_work(config);
  timer.stop();
  // debug info
  std::string working_time_string;
  ACE_Time_Value working_time;
  timer.elapsed_time(working_time);
  RPG_Common_Tools::period2String(working_time,
                                  working_time_string);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("total working time (h:m:s.us): \"%s\"...\n"),
             working_time_string.c_str()));

  // step4: clean up
  TTF_Quit();
  SDL_Quit();

  // stop profile timer...
  process_profile.stop();

  // only process profile left to do...
  ACE_Profile_Timer::ACE_Elapsed_Time elapsed_time;
  elapsed_time.real_time = 0.0;
  elapsed_time.user_time = 0.0;
  elapsed_time.system_time = 0.0;
  if (process_profile.elapsed_time(elapsed_time) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Profile_Timer::elapsed_time: \"%m\", aborting\n")));

    // *PORTABILITY*: on Windows, we must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF
  ACE_Profile_Timer::Rusage elapsed_rusage;
  ACE_OS::memset(&elapsed_rusage,
                 0,
                 sizeof(ACE_Profile_Timer::Rusage));
  process_profile.elapsed_rusage(elapsed_rusage);
  ACE_Time_Value user_time(elapsed_rusage.ru_utime);
  ACE_Time_Value system_time(elapsed_rusage.ru_stime);
  std::string user_time_string;
  std::string system_time_string;
  RPG_Common_Tools::period2String(user_time,
                                  user_time_string);
  RPG_Common_Tools::period2String(system_time,
                                  system_time_string);
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT(" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\nmaximum resident set size = %d\nintegral shared memory size = %d\nintegral unshared data size = %d\nintegral unshared stack size = %d\npage reclaims = %d\npage faults = %d\nswaps = %d\nblock input operations = %d\nblock output operations = %d\nmessages sent = %d\nmessages received = %d\nsignals received = %d\nvoluntary context switches = %d\ninvoluntary context switches = %d\n"),
             elapsed_time.real_time,
             elapsed_time.user_time,
             elapsed_time.system_time,
             user_time_string.c_str(),
             system_time_string.c_str(),
             elapsed_rusage.ru_maxrss,
             elapsed_rusage.ru_ixrss,
             elapsed_rusage.ru_idrss,
             elapsed_rusage.ru_isrss,
             elapsed_rusage.ru_minflt,
             elapsed_rusage.ru_majflt,
             elapsed_rusage.ru_nswap,
             elapsed_rusage.ru_inblock,
             elapsed_rusage.ru_oublock,
             elapsed_rusage.ru_msgsnd,
             elapsed_rusage.ru_msgrcv,
             elapsed_rusage.ru_nsignals,
             elapsed_rusage.ru_nvcsw,
             elapsed_rusage.ru_nivcsw));

  // *PORTABILITY*: on Windows, we must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::fini() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::fini(): \"%m\", aborting\n")));

    return EXIT_FAILURE;
  } // end IF
#endif

  return EXIT_SUCCESS;
} // end main
