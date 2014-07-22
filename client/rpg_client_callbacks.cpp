/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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

#include "rpg_client_callbacks.h"

#include "rpg_client_defines.h"
#include "rpg_client_common.h"
#include "rpg_client_engine.h"
#include "rpg_client_ui_tools.h"
#include "rpg_client_common_tools.h"

#include "rpg_engine_defines.h"
#include "rpg_engine_common.h"
#include "rpg_engine_common_tools.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_cursor.h"
#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_common_tools.h"

#include "rpg_sound_event.h"
#include "rpg_sound_event_manager.h"

#include "rpg_map_defines.h"
#include "rpg_map_common_tools.h"

#include "rpg_player_defines.h"
#include "rpg_player_common_tools.h"

#include "rpg_item_armor.h"
#include "rpg_item_commodity.h"
#include "rpg_item_weapon.h"
#include "rpg_item_instance_manager.h"

#include "rpg_magic_common_tools.h"

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"
#include "rpg_common_tools.h"
#include "rpg_common_file_tools.h"

#include <gmodule.h>

#include <ace/Reactor.h>
#include <ace/Dirent_Selector.h>
#include <ace/Log_Msg.h>

#include <sstream>

void
update_equipment(const RPG_Client_GTK_CBData_t& data_in)
{
  RPG_TRACE(ACE_TEXT("::update_equipment"));

  // sanity check(s)
  ACE_ASSERT(data_in.XML);
  ACE_ASSERT(data_in.entity.character);
  ACE_ASSERT(data_in.entity.character->isPlayerCharacter());
  RPG_Player* player = dynamic_cast<RPG_Player*>(data_in.entity.character);
  ACE_ASSERT(player);

  std::string text;
  std::stringstream converter;
  GtkWidget* current = NULL;

  // step1: list equipment
  current =
      GTK_WIDGET(glade_xml_get_widget(data_in.XML,
                                      ACE_TEXT_ALWAYS_CHAR("equipment_vbox")));
  ACE_ASSERT(current);
  GList* entries = gtk_container_get_children(GTK_CONTAINER(current));
//   ACE_ASSERT(entries);
  if (entries)
  {
    for (GList* iterator = entries;
         iterator;
         iterator = g_list_next(iterator))
    {
      // *NOTE*: effectively removes the widget from the container...
      gtk_widget_destroy(GTK_WIDGET(iterator->data));
    } // end FOR

    g_list_free(entries);
  } // end IF
  GtkWidget* current_box, *check_button, *label = NULL;
  RPG_Player_Equipment& player_equipment = player->getEquipment();
  RPG_Item_Base* item = NULL;
  RPG_Character_EquipmentSlot equipment_slot;
  const RPG_Player_Inventory& player_inventory = player->getInventory();
  std::string widget_name;
  for (RPG_Item_ListIterator_t iterator = player_inventory.myItems.begin();
       iterator != player_inventory.myItems.end();
       iterator++)
  {
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
      case ITEM_ARMOR:
      {
        RPG_Item_Armor* armor = dynamic_cast<RPG_Item_Armor*>(item);
        ACE_ASSERT(armor);

        text =
            RPG_Common_Tools::enumToString(RPG_Item_ArmorTypeHelper::RPG_Item_ArmorTypeToString(armor->getArmorType()));

        break;
      }
      case ITEM_COMMODITY:
      {
        RPG_Item_Commodity* commodity = dynamic_cast<RPG_Item_Commodity*>(item);
        ACE_ASSERT(commodity);

        RPG_Item_CommodityUnion commodity_type = commodity->getCommoditySubType();
        switch (commodity_type.discriminator)
        {
          case RPG_Item_CommodityUnion::COMMODITYBEVERAGE:
          {
            text =
                RPG_Common_Tools::enumToString(RPG_Item_CommodityBeverageHelper::RPG_Item_CommodityBeverageToString(commodity_type.commoditybeverage));

            break;
          }
          case RPG_Item_CommodityUnion::COMMODITYLIGHT:
          {
            text =
                RPG_Common_Tools::enumToString(RPG_Item_CommodityLightHelper::RPG_Item_CommodityLightToString(commodity_type.commoditylight));

            break;
          }
          default:
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("invalid commodity subtype (was: %d), aborting\n"),
                       commodity_type.discriminator));

            break;
          }
        } // end SWITCH

        break;
      }
      case ITEM_WEAPON:
      {
        RPG_Item_Weapon* weapon = dynamic_cast<RPG_Item_Weapon*>(item);
        ACE_ASSERT(weapon);

        // *TODO*: pretty-print this string
        text =
            RPG_Common_Tools::enumToString(RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString(weapon->getWeaponType()), false); // don't strip leading "xxx_"

        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid item type (was: \"%s\"), aborting\n"),
                   RPG_Item_TypeHelper::RPG_Item_TypeToString(item->getType()).c_str()));

        break;
      }
    } // end SWITCH

    current_box = NULL;
    current_box = gtk_hbox_new(TRUE, // homogeneous
                               0);   // spacing
    ACE_ASSERT(current_box);
    label = NULL;
    label = gtk_label_new(text.c_str());
    ACE_ASSERT(label);
    gtk_box_pack_start(GTK_BOX(current_box), label,
                       TRUE, // expand
                       TRUE, // fill
                       0);   // padding
    check_button = NULL;
    check_button = gtk_check_button_new();
    ACE_ASSERT(check_button);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button),
                                 player_equipment.isEquipped(*iterator, equipment_slot));
    converter.str(ACE_TEXT(""));
    if (!(converter << *iterator))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to convert integer (was: %u), aborting\n"),
                 *iterator));

      return;
    } // end IF
    widget_name = converter.str();
    gtk_widget_set_name(check_button,
                        widget_name.c_str());
    g_signal_connect(check_button,
                     ACE_TEXT_ALWAYS_CHAR("toggled"),
                     G_CALLBACK(item_toggled_GTK_cb),
                     &const_cast<RPG_Client_GTK_CBData_t&>(data_in));

    gtk_box_pack_start(GTK_BOX(current_box), check_button,
                       TRUE, // expand
                       TRUE, // fill
                       0);   // padding

//     gtk_container_add(GTK_CONTAINER(current), label);
    gtk_box_pack_start(GTK_BOX(current), current_box,
                       TRUE, // expand
                       TRUE, // fill
                       0);   // padding
  } // end FOR

  gtk_widget_show_all(current);
}

void
update_character_profile(const RPG_Player& player_in,
                         GladeXML* xml_in)
{
  RPG_TRACE(ACE_TEXT("::update_character_profile"));

  // sanity check(s)
  ACE_ASSERT(xml_in);

  std::string text;
  std::stringstream converter;
  GtkWidget* current = NULL;

  // step1: name
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("name")));
  ACE_ASSERT(current);
	gchar* text_p = RPG_Client_UI_Tools::Locale2UTF8(player_in.getName());
	if (!text_p)
	{
		ACE_DEBUG((LM_ERROR,
							 ACE_TEXT("failed to RPG_Client_UI_Tools::Locale2UTF8(\"%s\"), aborting\n"),
							 ACE_TEXT(player_in.getName().c_str())));

		return;
	} // end IF
  gtk_label_set_text(GTK_LABEL(current),
										 text_p);
	g_free(text_p);

  // step2: gender
  const RPG_Character_Gender& player_gender = player_in.getGender();
  switch (player_gender)
  {
    case RPG_CHARACTER_GENDER_MAX:
    case RPG_CHARACTER_GENDER_INVALID:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid gender (was: \"%s\"), aborting\n"),
								 ACE_TEXT(RPG_Character_GenderHelper::RPG_Character_GenderToString(player_gender).c_str())));

      return;
    }
    case GENDER_NONE:
      text = ACE_TEXT_ALWAYS_CHAR("N/A"); break;
    default:
      text =
          RPG_Common_Tools::enumToString(RPG_Character_GenderHelper::RPG_Character_GenderToString(player_gender)); break;
  } // end SWITCH
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("gender")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     text.c_str());

  // step3: race
  text.clear();
  const RPG_Character_Race_t& player_race = player_in.getRace();
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
        switch (static_cast<RPG_Character_Race>(race_index))
        {
          case RPG_CHARACTER_RACE_MAX:
          case RPG_CHARACTER_RACE_INVALID:
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("invalid race (was: \"%s\"), aborting\n"),
											 ACE_TEXT(RPG_Character_RaceHelper::RPG_Character_RaceToString(static_cast<RPG_Character_Race> (race_index)).c_str())));

            return;
          }
          default:
          {
            text +=
                RPG_Common_Tools::enumToString(RPG_Character_RaceHelper::RPG_Character_RaceToString(static_cast<RPG_Character_Race> (race_index)));
            break;
          }
        } // end SWITCH
        text += ACE_TEXT_ALWAYS_CHAR(",");
      } // end IF
    } // end FOR
    text.erase(--text.end());
  } // end ELSE
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("race")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     text.c_str());

  // step4: class
  text.clear();
  const RPG_Character_Class& player_class = player_in.getClass();
  switch (player_class.metaClass)
  {
    case RPG_CHARACTER_METACLASS_MAX:
    case RPG_CHARACTER_METACLASS_INVALID:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid metaclass (was: \"%s\"), aborting\n"),
								 ACE_TEXT(RPG_Character_MetaClassHelper::RPG_Character_MetaClassToString(player_class.metaClass).c_str())));

      break;
    }
    case METACLASS_NONE:
      text = ACE_TEXT_ALWAYS_CHAR("N/A"); break;
    default:
    {
      text =
          RPG_Common_Tools::enumToString(RPG_Character_MetaClassHelper::RPG_Character_MetaClassToString(player_class.metaClass));

      break;
    }
  } // end SWITCH
  if (!player_class.subClasses.empty())
    text += ACE_TEXT_ALWAYS_CHAR(" (");
  for (RPG_Character_SubClassesIterator_t iterator =
       player_class.subClasses.begin();
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
									 ACE_TEXT(RPG_Common_SubClassHelper::RPG_Common_SubClassToString(*iterator).c_str())));

        break;
      }
      case SUBCLASS_NONE:
        text += ACE_TEXT_ALWAYS_CHAR("N/A"); break;
      default:
      {
        text +=
            RPG_Common_Tools::enumToString(RPG_Common_SubClassHelper::RPG_Common_SubClassToString(*iterator));

        break;
      }
    } // end SWITCH
    text += ACE_TEXT_ALWAYS_CHAR(",");
  } // end FOR
  if (!player_class.subClasses.empty())
  {
    text.erase(--text.end());
    text += ACE_TEXT_ALWAYS_CHAR(")");
  } // end IF
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("class")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     text.c_str());

  // step5: alignment
  text.clear();
  const RPG_Character_Alignment& player_alignment = player_in.getAlignment();
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
									 ACE_TEXT(RPG_Character_AlignmentCivicHelper::RPG_Character_AlignmentCivicToString(player_alignment.civic).c_str())));

        break;
      }
      default:
      {
        text +=
            RPG_Common_Tools::enumToString(RPG_Character_AlignmentCivicHelper::RPG_Character_AlignmentCivicToString(player_alignment.civic));

        break;
      }
    } // end SWITCH
    text += ACE_TEXT_ALWAYS_CHAR(" ");
    switch (player_alignment.civic)
    {
      case RPG_CHARACTER_ALIGNMENTETHIC_MAX:
      case RPG_CHARACTER_ALIGNMENTETHIC_INVALID:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid alignment (ethic) (was: \"%s\"), aborting\n"),
									 ACE_TEXT(RPG_Character_AlignmentEthicHelper::RPG_Character_AlignmentEthicToString(player_alignment.ethic).c_str())));

        break;
      }
      default:
      {
        text +=
            RPG_Common_Tools::enumToString(RPG_Character_AlignmentEthicHelper::RPG_Character_AlignmentEthicToString(player_alignment.ethic));

        break;
      }
    } // end SWITCH
  } // end ELSE
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("alignment")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     text.c_str());

  // step6: HP
  //converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  //converter.clear();
	converter << player_in.getNumHitPoints();
  converter << ACE_TEXT_ALWAYS_CHAR(" / ");
	converter << player_in.getNumTotalHitPoints();
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("hitpoints")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());

  // step7: AC
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
	converter << static_cast<int>(player_in.getArmorClass(DEFENSE_NORMAL));
  converter << ACE_TEXT_ALWAYS_CHAR(" / ");
	converter << static_cast<int>(player_in.getArmorClass(DEFENSE_TOUCH));
  converter << ACE_TEXT_ALWAYS_CHAR(" / ");
	converter << static_cast<int>(player_in.getArmorClass(DEFENSE_FLATFOOTED));
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("armorclass")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());

  // step8: XP
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
	converter << player_in.getExperience();
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("experience")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());

  // step9: level(s)
  text.clear();
  unsigned char level = 0;
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
  if (player_class.subClasses.empty())
    converter << static_cast<unsigned int>(level);
  else
  {
    for (RPG_Character_SubClassesIterator_t iterator =
         player_class.subClasses.begin();
         iterator != player_class.subClasses.end();
         iterator++)
    {
      level = player_in.getLevel(*iterator);
      converter << static_cast<unsigned int>(level);
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
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("level")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     text.c_str());

  // step10: gold
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
	converter << player_in.getWealth();
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("gold")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());

  // step11: condition
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("condition_vbox")));
  ACE_ASSERT(current);
  GList* entries = gtk_container_get_children(GTK_CONTAINER(current));
//   ACE_ASSERT(entries);
  if (entries)
  {
    //for (GList* iterator = entries;
    //     iterator;
    //     iterator = g_list_next(iterator))
    //{
    //  // *NOTE*: effectively removes the widget from the container...
    //  gtk_widget_destroy(GTK_WIDGET(iterator->data));
    //} // end FOR

    //g_list_free(entries);
		g_list_free_full(entries,
										 (GDestroyNotify)gtk_widget_destroy);
  } // end IF
  GtkWidget* label = NULL;
  const RPG_Character_Conditions_t& player_condition = player_in.getCondition();
  for (RPG_Character_ConditionsIterator_t iterator = player_condition.begin();
       iterator != player_condition.end();
       iterator++)
  {
    text =
        RPG_Common_Tools::enumToString(RPG_Common_ConditionHelper::RPG_Common_ConditionToString(*iterator));
    label = NULL;
    label = gtk_label_new(text.c_str());
    ACE_ASSERT(label);
//     gtk_container_add(GTK_CONTAINER(current), label);
    gtk_box_pack_start(GTK_BOX(current), label,
                       FALSE, // expand
                       FALSE, // fill
                       0);    // padding
  } // end FOR
  gtk_widget_show_all(current);

  // step12: attributes
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
	converter << static_cast<unsigned int>(player_in.getAttribute(ATTRIBUTE_STRENGTH));
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("strength")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
	converter << static_cast<unsigned int>(player_in.getAttribute(ATTRIBUTE_DEXTERITY));
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("dexterity")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
	converter << static_cast<unsigned int>(player_in.getAttribute(ATTRIBUTE_CONSTITUTION));
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("constitution")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
	converter << static_cast<unsigned int>(player_in.getAttribute(ATTRIBUTE_INTELLIGENCE));
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("intelligence")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
	converter << static_cast<unsigned int>(player_in.getAttribute(ATTRIBUTE_WISDOM));
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("wisdom")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
	converter << static_cast<unsigned int>(player_in.getAttribute(ATTRIBUTE_CHARISMA));
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("charisma")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());

  // step13: feats
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("feats_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
//   ACE_ASSERT(entries);
  if (entries)
  {
		//for (GList* iterator = entries;
		//     iterator;
		//     iterator = g_list_next(iterator))
		//{
		//  // *NOTE*: effectively removes the widget from the container...
		//  gtk_widget_destroy(GTK_WIDGET(iterator->data));
		//} // end FOR

		//g_list_free(entries);
		g_list_free_full(entries,
										 (GDestroyNotify)gtk_widget_destroy);
	} // end IF
  label = NULL;
  const RPG_Character_Feats_t& player_feats = player_in.getFeats();
  for (RPG_Character_FeatsConstIterator_t iterator = player_feats.begin();
       iterator != player_feats.end();
       iterator++)
  {
    text =
        RPG_Common_Tools::enumToString(RPG_Character_FeatHelper::RPG_Character_FeatToString(*iterator));
    label = NULL;
    label = gtk_label_new(text.c_str());
    ACE_ASSERT(label);
//     gtk_container_add(GTK_CONTAINER(current), label);
    gtk_box_pack_start(GTK_BOX(current), label,
                       FALSE, // expand
                       FALSE, // fill
                       0);    // padding
  } // end FOR
  gtk_widget_show_all(current);

  // step14: abilities
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("abilities_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
//   ACE_ASSERT(entries);
  if (entries)
  {
		//for (GList* iterator = entries;
		//     iterator;
		//     iterator = g_list_next(iterator))
		//{
		//  // *NOTE*: effectively removes the widget from the container...
		//  gtk_widget_destroy(GTK_WIDGET(iterator->data));
		//} // end FOR

		//g_list_free(entries);
		g_list_free_full(entries,
										 (GDestroyNotify)gtk_widget_destroy);
	} // end IF
  const RPG_Character_Abilities_t& player_abilities = player_in.getAbilities();
  for (RPG_Character_AbilitiesConstIterator_t iterator =
       player_abilities.begin();
       iterator != player_abilities.end();
       iterator++)
  {
    text =
        RPG_Common_Tools::enumToString(RPG_Character_AbilityHelper::RPG_Character_AbilityToString(*iterator));
    label = NULL;
    label = gtk_label_new(text.c_str());
    ACE_ASSERT(label);
//     gtk_container_add(GTK_CONTAINER(current), label);
    gtk_box_pack_start(GTK_BOX(current), label,
                       FALSE, // expand
                       FALSE, // fill
                       0);    // padding
  } // end FOR
  gtk_widget_show_all(current);

  // step15: skills
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("skills_table")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
//   ACE_ASSERT(entries);
  if (entries)
  {
		//for (GList* iterator = entries;
		//     iterator;
		//     iterator = g_list_next(iterator))
		//{
		//  // *NOTE*: effectively removes the widget from the container...
		//  gtk_widget_destroy(GTK_WIDGET(iterator->data));
		//} // end FOR

		//g_list_free(entries);
		g_list_free_full(entries,
										 (GDestroyNotify)gtk_widget_destroy);
	} // end IF
  //GtkWidget* current_box = NULL;
  const RPG_Character_Skills_t& player_skills = player_in.getSkills();
	gtk_table_resize(GTK_TABLE(current),
									 player_skills.size(), 2);
	unsigned int index = 0;
	GtkAttachOptions attach_options = GTK_SHRINK;
  for (RPG_Character_SkillsConstIterator_t iterator = player_skills.begin();
       iterator != player_skills.end();
       iterator++, index++)
  {
    text =
        RPG_Common_Tools::enumToString(RPG_Common_SkillHelper::RPG_Common_SkillToString((*iterator).first));
    //current_box = NULL;
    //current_box = gtk_hbox_new(TRUE, // homogeneous
    //                           0);   // spacing
    //ACE_ASSERT(current_box);
    label = NULL;
    label = gtk_label_new(text.c_str());
    ACE_ASSERT(label);
    //gtk_box_pack_start(GTK_BOX(current_box), label,
    //                   FALSE, // expand
    //                   FALSE, // fill
    //                   0);    // padding
		gtk_table_attach_defaults(GTK_TABLE(current),
															label,
															0, 1,
															index, index + 1);

    converter.str(ACE_TEXT(""));
    converter.clear();
    converter << static_cast<unsigned int>((*iterator).second);
    text = converter.str();
    label = NULL;
    label = gtk_label_new(text.c_str());
    ACE_ASSERT(label);
    //gtk_box_pack_start(GTK_BOX(current_box), label,
    //                   FALSE, // expand
    //                   FALSE, // fill
    //                   0);    // padding
		gtk_table_attach(GTK_TABLE(current),
										 label,
										 1, 2,
										 index, index + 1,
										 attach_options, attach_options,
										 3, 0);

//     gtk_container_add(GTK_CONTAINER(current), label);
    //gtk_box_pack_start(GTK_BOX(current), current_box,
    //                   FALSE, // expand
    //                   FALSE, // fill
    //                   0);    // padding
  } // end FOR
  gtk_widget_show_all(current);

  // step16: spells
	GtkNotebook* notebook =
		GTK_NOTEBOOK(glade_xml_get_widget(xml_in,
		                                  ACE_TEXT_ALWAYS_CHAR("notebook")));
	ACE_ASSERT(notebook);
	current = gtk_notebook_get_nth_page(notebook,
																			3);
  //current =
  //    GTK_WIDGET(glade_xml_get_widget(xml_in,
  //                                    ACE_TEXT_ALWAYS_CHAR("spells_frame")));
  ACE_ASSERT(current);
	gtk_widget_set_sensitive(current,
													 RPG_Magic_Common_Tools::hasCasterClass(player_class));
  //if (!RPG_Magic_Common_Tools::hasCasterClass(player_class))
  //  gtk_widget_hide_all(current);
  //else
  //  gtk_widget_show_all(current);

  // divine casters know ALL spells from the levels they can cast...
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("known_spells_frame")));
  ACE_ASSERT(current);
  if (!RPG_Magic_Common_Tools::hasArcaneCasterClass(player_class))
    gtk_widget_hide_all(current);
  else
    gtk_widget_show_all(current);

  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("known_spells_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
//   ACE_ASSERT(entries);
  if (entries)
  {
		//for (GList* iterator = entries;
		//     iterator;
		//     iterator = g_list_next(iterator))
		//{
		//  // *NOTE*: effectively removes the widget from the container...
		//  gtk_widget_destroy(GTK_WIDGET(iterator->data));
		//} // end FOR

		//g_list_free(entries);
		g_list_free_full(entries,
										 (GDestroyNotify)gtk_widget_destroy);
  } // end IF
  const RPG_Magic_SpellTypes_t& player_known_spells =
      player_in.getKnownSpells();
  for (RPG_Magic_SpellTypesIterator_t iterator = player_known_spells.begin();
       iterator != player_known_spells.end();
       iterator++)
  {
    text =
        RPG_Common_Tools::enumToString(RPG_Magic_SpellTypeHelper::RPG_Magic_SpellTypeToString(*iterator));

    label = NULL;
    label = gtk_label_new(text.c_str());
    ACE_ASSERT(label);
//     gtk_container_add(GTK_CONTAINER(current), label);
    gtk_box_pack_start(GTK_BOX(current), label,
                       FALSE, // expand
                       FALSE, // fill
                       0);    // padding
  } // end FOR
  gtk_widget_show_all(current);
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("prepared_spells_table")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
//   ACE_ASSERT(entries);
  if (entries)
  {
		//for (GList* iterator = entries;
		//     iterator;
		//     iterator = g_list_next(iterator))
		//{
		//  // *NOTE*: effectively removes the widget from the container...
		//  gtk_widget_destroy(GTK_WIDGET(iterator->data));
		//} // end FOR

		//g_list_free(entries);
		g_list_free_full(entries,
										 (GDestroyNotify)gtk_widget_destroy);
	} // end IF
  const RPG_Magic_Spells_t& player_spells = player_in.getSpells();
	unsigned int count = 0;
	RPG_Magic_SpellTypes_t processsed_types;
	std::map<RPG_Magic_SpellType, unsigned int> spells;
	for (RPG_Magic_SpellsIterator_t iterator = player_spells.begin();
			 iterator != player_spells.end();
			 iterator++)
	{
		if (processsed_types.find(*iterator) != processsed_types.end())
			continue; // already accounted for

		// count instances
		count = 0;
		for (RPG_Magic_SpellsIterator_t iterator2 = player_spells.begin();
				 iterator2 != player_spells.end();
				 iterator2++)
		if (*iterator2 == *iterator)
			count++;

		spells[*iterator] = count;
		processsed_types.insert(*iterator);
	} // end FOR
	gtk_table_resize(GTK_TABLE(current),
									 spells.size(), 2);
	index = 0;
	for (std::map<RPG_Magic_SpellType, unsigned int>::const_iterator iterator = spells.begin();
       iterator != spells.end();
       iterator++, index++)
  {
    text =
        RPG_Common_Tools::enumToString(RPG_Magic_SpellTypeHelper::RPG_Magic_SpellTypeToString((*iterator).first));
    //current_box = NULL;
    //current_box = gtk_hbox_new(TRUE, // homogeneous
    //                           0);   // spacing
    //ACE_ASSERT(current_box);
    label = NULL;
    label = gtk_label_new(text.c_str());
    ACE_ASSERT(label);
    //gtk_box_pack_start(GTK_BOX(current_box), label,
    //                   FALSE, // expand
    //                   FALSE, // fill
    //                   0);    // padding
		gtk_table_attach_defaults(GTK_TABLE(current),
															label,
															0, 1,
															index, index + 1);

    converter.str(ACE_TEXT(""));
    converter.clear();
    converter << (*iterator).second;
    text = converter.str();
    label = NULL;
    label = gtk_label_new(text.c_str());
    ACE_ASSERT(label);
    //gtk_box_pack_start(GTK_BOX(current_box), label,
    //                   FALSE, // expand
    //                   FALSE, // fill
    //                   0);    // padding
		gtk_table_attach(GTK_TABLE(current),
										 label,
										 1, 2,
										 index, index + 1,
										 attach_options, attach_options,
										 3, 0);

    //     gtk_container_add(GTK_CONTAINER(current), label);
    //gtk_box_pack_start(GTK_BOX(current), current_box,
    //                   FALSE, // expand
    //                   FALSE, // fill
    //                   0);    // padding
  } // end FOR
  gtk_widget_show_all(current);

  // step17: inventory
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("inventory_items_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
//   ACE_ASSERT(entries);
  if (entries)
  {
		//for (GList* iterator = entries;
		//     iterator;
		//     iterator = g_list_next(iterator))
		//{
		//  // *NOTE*: effectively removes the widget from the container...
		//  gtk_widget_destroy(GTK_WIDGET(iterator->data));
		//} // end FOR

		//g_list_free(entries);
		g_list_free_full(entries,
										 (GDestroyNotify)gtk_widget_destroy);
	} // end IF
  const RPG_Player_Inventory& player_inventory = player_in.getInventory();
  RPG_Player_Equipment& player_equipment =
      const_cast<RPG_Player&>(player_in).getEquipment();
  RPG_Item_Base* item = NULL;
  RPG_Character_EquipmentSlot equipment_slot;
  for (RPG_Item_ListIterator_t iterator = player_inventory.myItems.begin();
       iterator != player_inventory.myItems.end();
       iterator++)
  {
    item = NULL;

    // retrieve item handle
    if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->get(*iterator,
                                                              item))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid item (ID: %d), aborting\n"),
                 *iterator));

      return;
    } // end IF
    ACE_ASSERT(item);

    switch (item->getType())
    {
      case ITEM_ARMOR:
      {
        RPG_Item_Armor* armor = NULL;
        try
        {
          armor = dynamic_cast<RPG_Item_Armor*>(item);
        }
        catch (...)
        {
          armor = NULL;
        }
        if (!armor)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to dynamic_cast<RPG_Item_Armor*>(%@), aborting\n"),
                     item));

          return;
        } // end IF

        text =
            RPG_Common_Tools::enumToString(RPG_Item_ArmorTypeHelper::RPG_Item_ArmorTypeToString(armor->getArmorType()));

        break;
      }
      case ITEM_COMMODITY:
      {
        RPG_Item_Commodity* commodity = NULL;
        try
        {
          commodity = dynamic_cast<RPG_Item_Commodity*>(item);
        }
        catch (...)
        {
          commodity = NULL;
        }
        if (!commodity)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to dynamic_cast<RPG_Item_Commodity*>(%@), aborting\n"),
                     item));

          return;
        } // end IF

        RPG_Item_CommodityUnion commodity_type =
            commodity->getCommoditySubType();
        switch (commodity_type.discriminator)
        {
          case RPG_Item_CommodityUnion::COMMODITYBEVERAGE:
          {
            text =
                RPG_Common_Tools::enumToString(RPG_Item_CommodityBeverageHelper::RPG_Item_CommodityBeverageToString(commodity_type.commoditybeverage));

            break;
          }
          case RPG_Item_CommodityUnion::COMMODITYLIGHT:
          {
            text =
                RPG_Common_Tools::enumToString(RPG_Item_CommodityLightHelper::RPG_Item_CommodityLightToString(commodity_type.commoditylight));

            break;
          }
          default:
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("invalid commodity subtype (was: %d), aborting\n"),
                       commodity_type.discriminator));

            return;
          }
        } // end SWITCH

        break;
      }
      case ITEM_WEAPON:
      {
        RPG_Item_Weapon* weapon = NULL;
        try
        {
          weapon = dynamic_cast<RPG_Item_Weapon*>(item);
        }
        catch (...)
        {
          weapon = NULL;
        }
        if (!weapon)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to dynamic_cast<RPG_Item_Weapon*>(%@), aborting\n"),
                     item));

          return;
        } // end IF

        // *TODO*: pretty-print this string
        text =
            RPG_Common_Tools::enumToString(RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString(weapon->getWeaponType()), false); // don't strip leading "xxx_"

        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid item type (was: \"%s\"), aborting\n"),
                   ACE_TEXT(RPG_Item_TypeHelper::RPG_Item_TypeToString(item->getType()).c_str())));

        return;
      }
    } // end SWITCH

    // equipped ? --> add '*'
    if (player_equipment.isEquipped(*iterator, equipment_slot))
      text += ACE_TEXT_ALWAYS_CHAR('*');

    label = NULL;
    label = gtk_label_new(text.c_str());
    ACE_ASSERT(label);
//     gtk_container_add(GTK_CONTAINER(current), label);
    gtk_box_pack_start(GTK_BOX(current), label,
                       FALSE, // expand
                       FALSE, // fill
                       0);    // padding
  } // end FOR
  gtk_widget_show_all(current);
}

void
update_entity_profile(const RPG_Engine_Entity_t& entity_in,
                      GladeXML* xml_in)
{
  RPG_TRACE(ACE_TEXT("::update_entity_profile"));

  // sanity checks
  ACE_ASSERT(entity_in.character);
  ACE_ASSERT(entity_in.character->isPlayerCharacter());
  RPG_Player* player = dynamic_cast<RPG_Player*>(entity_in.character);
  ACE_ASSERT(player);
  ACE_ASSERT(xml_in);

  // step1: update character profile
  ::update_character_profile(*player, xml_in);

  // step2: update image (if available)
  // retrieve image widget
  GtkImage* image =
      GTK_IMAGE(glade_xml_get_widget(xml_in,
                                     ACE_TEXT_ALWAYS_CHAR("image_sprite")));
  ACE_ASSERT(image);

  // retrieve character handle
  RPG_Player_Player_Base* player_base = NULL;
  try
  {
    player_base = dynamic_cast<RPG_Player_Player_Base*>(entity_in.character);
  }
  catch (...)
  {
    player_base = NULL;
  }
  if (!player_base)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to dynamic_cast<RPG_Player_Player_Base*>(%@), returning\n"),
               entity_in.character));

    return;
  } // end IF
  std::string filename;
  RPG_Graphics_GraphicTypeUnion type;
  type.sprite = RPG_Client_Common_Tools::class2Sprite(player_base->getClass());
  if (type.sprite != RPG_GRAPHICS_SPRITE_INVALID)
  {
    type.discriminator = RPG_Graphics_GraphicTypeUnion::SPRITE;
    RPG_Graphics_t graphic;
    graphic.category = RPG_GRAPHICS_CATEGORY_INVALID;
    graphic.type.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;
    // retrieve properties from the dictionary
    graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->get(type);
    ACE_ASSERT((graphic.type.sprite == type.sprite) &&
               (graphic.type.discriminator ==
                RPG_Graphics_GraphicTypeUnion::SPRITE));
    // sanity check
    if (graphic.category != CATEGORY_SPRITE)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid category (was: \"%s\"), not a sprite type, aborting\n"),
                 ACE_TEXT(RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString(graphic.category).c_str())));

      return;
    } // end IF

    // assemble path
    RPG_Graphics_Common_Tools::graphicToFile(graphic, filename);
    ACE_ASSERT(!filename.empty());
    // sanity check(s)
    if (!RPG_Common_File_Tools::isReadable(filename))
    {
      ACE_DEBUG((LM_ERROR,
                  ACE_TEXT("failed to RPG_Common_File_Tools::isReadable(\"%s\"), aborting\n"),
                  ACE_TEXT(filename.c_str())));

      return;
    } // end IF
  } // end IF
  if (!filename.empty())
    gtk_image_set_from_file(image, filename.c_str());
  else
    gtk_image_clear(image);
}

void
reset_character_profile(GladeXML* xml_in)
{
  RPG_TRACE(ACE_TEXT("::reset_character_profile"));

  // sanity checks
  ACE_ASSERT(xml_in);

  // step1: reset character profile
  GtkWidget* current = NULL;

  // step1a: name
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("name")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     NULL);

  // step1b: gender
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("gender")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     NULL);

  // step1c: race
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("race")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     NULL);

  // step1d: class
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("class")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     NULL);

  // step1e: alignment
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("alignment")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     NULL);

  // step1f: HP
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("hitpoints")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     NULL);

  // step1g: AC
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("armorclass")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     NULL);

  // step1h: XP
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("experience")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     NULL);

  // step1i: level(s)
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("level")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     NULL);

  // step1j: gold
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("gold")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     NULL);

  // step1k: condition
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("condition_vbox")));
  ACE_ASSERT(current);
  GList* entries = gtk_container_get_children(GTK_CONTAINER(current));
//   ACE_ASSERT(entries);
  if (entries)
  {
    for (GList* iterator = entries;
         iterator;
         iterator = g_list_next(iterator))
    {
      // *NOTE*: effectively removes the widget from the container...
      gtk_widget_destroy(GTK_WIDGET(iterator->data));
    } // end FOR

    g_list_free(entries);
  } // end IF

  // step1l: attributes
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("strength")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     NULL);
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("dexterity")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     NULL);
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("constitution")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     NULL);
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("intelligence")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     NULL);
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("wisdom")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     NULL);
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("charisma")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     NULL);

  // step1m: feats
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("feats_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
//   ACE_ASSERT(entries);
  if (entries)
  {
		//for (GList* iterator = entries;
		//     iterator;
		//     iterator = g_list_next(iterator))
		//{
		//  // *NOTE*: effectively removes the widget from the container...
		//  gtk_widget_destroy(GTK_WIDGET(iterator->data));
		//} // end FOR

		//g_list_free(entries);
		g_list_free_full(entries,
										 (GDestroyNotify)gtk_widget_destroy);
  } // end IF

  // step1n: abilities
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("abilities_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
//   ACE_ASSERT(entries);
  if (entries)
  {
		//for (GList* iterator = entries;
		//     iterator;
		//     iterator = g_list_next(iterator))
		//{
		//  // *NOTE*: effectively removes the widget from the container...
		//  gtk_widget_destroy(GTK_WIDGET(iterator->data));
		//} // end FOR

		//g_list_free(entries);
		g_list_free_full(entries,
										 (GDestroyNotify)gtk_widget_destroy);
  } // end IF

  // step1o: skills
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("skills_table")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
//   ACE_ASSERT(entries);
  if (entries)
  {
		//for (GList* iterator = entries;
		//     iterator;
		//     iterator = g_list_next(iterator))
		//{
		//  // *NOTE*: effectively removes the widget from the container...
		//  gtk_widget_destroy(GTK_WIDGET(iterator->data));
		//} // end FOR

		//g_list_free(entries);
		g_list_free_full(entries,
										 (GDestroyNotify)gtk_widget_destroy);
  } // end IF

  // step1p: spells
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("spells_frame")));
  ACE_ASSERT(current);
  gtk_widget_show_all(current);
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("known_spells_frame")));
  ACE_ASSERT(current);
  gtk_widget_show_all(current);
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("known_spells_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
//   ACE_ASSERT(entries);
  if (entries)
  {
		//for (GList* iterator = entries;
		//     iterator;
		//     iterator = g_list_next(iterator))
		//{
		//  // *NOTE*: effectively removes the widget from the container...
		//  gtk_widget_destroy(GTK_WIDGET(iterator->data));
		//} // end FOR

		//g_list_free(entries);
		g_list_free_full(entries,
										 (GDestroyNotify)gtk_widget_destroy);
  } // end IF
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("prepared_spells_table")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
//   ACE_ASSERT(entries);
  if (entries)
  {
		//for (GList* iterator = entries;
		//     iterator;
		//     iterator = g_list_next(iterator))
		//{
		//  // *NOTE*: effectively removes the widget from the container...
		//  gtk_widget_destroy(GTK_WIDGET(iterator->data));
		//} // end FOR

		//g_list_free(entries);
		g_list_free_full(entries,
										 (GDestroyNotify)gtk_widget_destroy);
  } // end IF

  // step1q: inventory
  current =
      GTK_WIDGET(glade_xml_get_widget(xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("inventory_items_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
//   ACE_ASSERT(entries);
  if (entries)
  {
		//for (GList* iterator = entries;
		//     iterator;
		//     iterator = g_list_next(iterator))
		//{
		//  // *NOTE*: effectively removes the widget from the container...
		//  gtk_widget_destroy(GTK_WIDGET(iterator->data));
		//} // end FOR

		//g_list_free(entries);
		g_list_free_full(entries,
										 (GDestroyNotify)gtk_widget_destroy);
  } // end IF
}

// callbacks used by ::scandir...
int
dirent_selector_profiles(const dirent* entry_in)
{
  RPG_TRACE(ACE_TEXT("::dirent_selector_profiles"));

  // *NOTE*: select player profiles
  std::string filename(entry_in->d_name);
  std::string extension(ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_PROFILE_EXT));
  std::string::size_type position =
      filename.rfind(extension, std::string::npos);
  if ((position == std::string::npos) ||
		  (position != (filename.size() - extension.size())))
//                     -1) != (filename.size() - extension.size()))
  {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("ignoring \"%s\"...\n"),
//                ACE_TEXT(entry_in->d_name)));

    return 0;
  } // end IF

  return 1;
}

int
dirent_selector_maps(const dirent* entry_in)
{
  RPG_TRACE(ACE_TEXT("::dirent_selector_maps"));

  // *NOTE*: select maps
  std::string filename(entry_in->d_name);
  std::string extension(ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT));
  std::string::size_type position =
      filename.rfind(extension, std::string::npos);
  if ((position == std::string::npos) ||
		  (position != (filename.size() - extension.size())))
//                     -1) != (filename.size() - extension.size()))
  {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("ignoring \"%s\"...\n"),
//                ACE_TEXT(entry_in->d_name)));

    return 0;
  } // end IF

  return 1;
}

int
dirent_selector_savedstates(const dirent* entry_in)
{
  RPG_TRACE(ACE_TEXT("::dirent_selector_savedstates"));

  // *NOTE*: select engine states
  std::string filename(entry_in->d_name);
  std::string extension(ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_STATE_EXT));
  std::string::size_type position =
      filename.rfind(extension, std::string::npos);
  if ((position == std::string::npos) ||
      (position != (filename.size() - extension.size())))
//                     -1) != (filename.size() - extension.size()))
  {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("ignoring \"%s\"...\n"),
//                ACE_TEXT(entry_in->d_name)));

    return 0;
  } // end IF

  return 1;
}

int
dirent_comparator(const dirent** entry1_in,
                  const dirent** entry2_in)
{
  RPG_TRACE(ACE_TEXT("::dirent_comparator"));

  return ACE_OS::strcmp((*entry1_in)->d_name,
                        (*entry2_in)->d_name);
}

unsigned int
load_files(const RPG_Client_Repository& repository_in,
           GtkListStore* listStore_in)
{
  RPG_TRACE(ACE_TEXT("::load_files"));

  unsigned int return_value = 0;

  // sanity check(s)
  ACE_ASSERT(listStore_in);
  std::string repository;
  ACE_SCANDIR_SELECTOR selector = NULL;
  std::string extension;
  switch (repository_in)
  {
    case REPOSITORY_MAPS:
    {
      repository = RPG_Map_Common_Tools::getMapsDirectory();
      selector = ::dirent_selector_maps;
      extension = ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT);
      break;
    }
    case REPOSITORY_PROFILES:
    {
      repository = RPG_Player_Common_Tools::getPlayerProfilesDirectory();
      selector = ::dirent_selector_profiles;
      extension = ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_PROFILE_EXT);
      break;
    }
    case REPOSITORY_ENGINESTATE:
    {
      repository = RPG_Engine_Common_Tools::getEngineStateDirectory();
      selector = ::dirent_selector_savedstates;
      extension = ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_STATE_EXT);
      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid repository (was: %d), aborting\n"),
                 repository_in));

      return 0;
    }
  } // end IF
  if (!RPG_Common_File_Tools::isDirectory(repository))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to load_files(\"%s\"), not a directory, aborting\n"),
               ACE_TEXT(repository.c_str())));

    return 0;
  } // end IF

  // retrieve all relevant files and sort them alphabetically...
  ACE_Dirent_Selector entries;
  if (entries.open(repository.c_str(),
                   selector,
                   &::dirent_comparator) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Dirent_Selector::open(\"%s\"): \"%m\", aborting\n"),
               ACE_TEXT(repository.c_str())));

    return 0;
  } // end IF

  // clear existing entries
  // *WARNING* triggers the "changed" signal of the combobox...
  gtk_list_store_clear(listStore_in);

  // iterate over entries
  std::string entry;
  GtkTreeIter iter;
  size_t position = -1;
  for (unsigned int i = 0;
       i < static_cast<unsigned int>(entries.length());
       i++)
  {
    // sanitize name (chop off extension)
    entry = entries[i]->d_name;
    position = entry.rfind(extension,
                           std::string::npos);
    ACE_ASSERT(position != std::string::npos);
    entry.erase(position,
                std::string::npos);

    // append new (text) entry
    gtk_list_store_append(listStore_in, &iter);
    gtk_list_store_set(listStore_in, &iter,
                       0, ACE_TEXT(entry.c_str()), // column 0
                       -1);

    return_value++;
  } // end FOR

  // clean up
  if (entries.close() == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Dirent_Selector::close: \"%m\", continuing\n")));

  // debug info
  GValue value;
  ACE_OS::memset(&value, 0, sizeof(value));
  const gchar* text = NULL;
  if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(listStore_in),
                                     &iter))
    return 0;
  gtk_tree_model_get_value(GTK_TREE_MODEL(listStore_in), &iter,
                           0, &value);
  text = g_value_get_string(&value);
  extension.erase(0, 1);
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("%s[0]: %s\n"),
             ACE_TEXT(extension.c_str()),
             ACE_TEXT(text)));
  g_value_unset(&value);
  for (unsigned int i = 1;
       gtk_tree_model_iter_next(GTK_TREE_MODEL(listStore_in),
                                &iter);
       i++)
  {
    ACE_OS::memset(&value, 0, sizeof(value));
    text = NULL;

    gtk_tree_model_get_value(GTK_TREE_MODEL(listStore_in), &iter,
                             0, &value);
    text = g_value_get_string(&value);
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("%s[%u]: %s\n"),
               ACE_TEXT(extension.c_str()),
               i,
               ACE_TEXT(text)));

    // clean up
    g_value_unset(&value);
  } // end FOR

  return return_value;
}

gint
combobox_sort_function(GtkTreeModel* model_in,
											 GtkTreeIter*  iterator1_in,
											 GtkTreeIter*  iterator2_in,
											 gpointer      userData_in)
{
	RPG_TRACE(ACE_TEXT("::combobox_sort_function"));

	gint sort_column = GPOINTER_TO_INT(userData_in);
	gint result = 0; // -1: row1 < row2, 0: equal, 1: row1 > row2

	switch (sort_column)
	{
		case 0:
		{
			gchar* row1, *row2;
			gtk_tree_model_get(model_in, iterator1_in, sort_column, &row1, -1);
			gtk_tree_model_get(model_in, iterator2_in, sort_column, &row2, -1);
			if ((row1 == NULL) ||
					(row2 == NULL))
			{
				if ((row1 == NULL) &&
						(row2 == NULL))
					break; // --> equal

				result = ((row1 == NULL) ? -1 : 1);

				break;
			} // end IF

			// compare (case-insensitive)
			gchar* row1_lower, *row2_lower;
			row1_lower = g_utf8_strdown(row1, -1);
			row2_lower = g_utf8_strdown(row2, -1);
			g_free(row1);
			g_free(row2);

			result = g_utf8_collate(row1_lower, row2_lower);
			g_free(row1_lower);
			g_free(row2_lower);

			break;
		}
		default:
			g_return_val_if_reached(0);
	} // end SWITCH

	return result;
}

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT gboolean
idle_init_UI_cb(gpointer act_in)
{
  RPG_TRACE(ACE_TEXT("::idle_init_UI_cb"));

  RPG_Client_GTK_CBData_t* data = static_cast<RPG_Client_GTK_CBData_t*>(act_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->XML);

	// activate first repository entry (if any)
	GtkComboBox* combobox =
			GTK_COMBO_BOX(glade_xml_get_widget(data->XML,
																				 ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
  ACE_ASSERT(combobox);
  if (gtk_widget_is_sensitive(GTK_WIDGET(combobox)))
    gtk_combo_box_set_active(combobox, 0);
  combobox =
      GTK_COMBO_BOX(glade_xml_get_widget(data->XML,
                                         ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_MAP_NAME)));
  ACE_ASSERT(combobox);
  if (gtk_widget_is_sensitive(GTK_WIDGET(combobox)))
    gtk_combo_box_set_active(combobox, 0);

	// one-shot action
	return FALSE; // G_SOURCE_REMOVE
}

G_MODULE_EXPORT gboolean
idle_fini_UI_cb(gpointer act_in)
{
  RPG_TRACE(ACE_TEXT("::idle_fini_UI_cb"));

  RPG_Client_GTK_CBData_t* data = static_cast<RPG_Client_GTK_CBData_t*>(act_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->XML);

  GtkWidget* widget =
      glade_xml_get_widget(data->XML,
                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_SERVER_PART_NAME));
  ACE_ASSERT(widget);
  // raise dialog window
  GdkWindow* toplevel = gtk_widget_get_parent_window(widget);
  ACE_ASSERT(toplevel);
  gdk_window_deiconify(toplevel);
  // emit a signal...
  gtk_button_clicked(GTK_BUTTON(widget));

	// one-shot action
	return FALSE; // G_SOURCE_REMOVE
}

G_MODULE_EXPORT gint
about_clicked_GTK_cb(GtkWidget* widget_in,
                     gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::about_clicked_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  RPG_Client_GTK_CBData_t* data = static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->XML);

  // retrieve about dialog handle
  GtkWidget* about_dialog =
      GTK_WIDGET(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_ABOUT_NAME)));
  ACE_ASSERT(about_dialog);
  if (!about_dialog)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
               ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_ABOUT_NAME)));

    return TRUE; // propagate
  } // end IF

  // draw it
  if (!GTK_WIDGET_VISIBLE(about_dialog))
    gtk_widget_show_all(about_dialog);

  return FALSE;
}

G_MODULE_EXPORT gint
quit_clicked_GTK_cb(GtkWidget* widget_in,
                    gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::quit_clicked_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
//   ACE_UNUSED_ARG(userData_in);
  RPG_Client_GTK_CBData_t* data =
      static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);

	// trigger SDL event loop
	SDL_Event sdl_event;
  sdl_event.type = SDL_QUIT;
	if (SDL_PushEvent(&sdl_event))
		ACE_DEBUG((LM_ERROR,
		           ACE_TEXT("failed to SDL_PushEvent(): \"%s\", continuing\n"),
               ACE_TEXT(SDL_GetError())));

  //// synch access
  //{
  //  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(data->lock);

  //  ACE_ASSERT(!(data->GTK_done));
  //  data->GTK_done = true;
  //} // end lock scope

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("leaving GTK...\n")));

  // this is the "delete-event" handler
  // --> destroy the main dialog widget
  return TRUE; // --> propagate
}

G_MODULE_EXPORT gint
create_character_clicked_GTK_cb(GtkWidget* widget_in,
                                gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::create_character_clicked_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  RPG_Client_GTK_CBData_t* data =
      static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);

  // clean up
  if (data->entity.character)
  {
    // *TODO*: save existing character first ?
    delete data->entity.character;
    data->entity.character = NULL;
  } // end IF
  data->entity.position =
      std::make_pair(std::numeric_limits<unsigned int>::max(),
                     std::numeric_limits<unsigned int>::max());
  data->entity.modes.clear();
  data->entity.actions.clear();
  data->entity.is_spawned = false;

  data->entity = RPG_Engine_Common_Tools::createEntity();
  ACE_ASSERT(data->entity.character);

  // update entity profile widgets
  ::update_entity_profile(data->entity,
                          data->XML);

  // make character display frame sensitive (if it's not already)
  GtkFrame* character_frame =
      GTK_FRAME(glade_xml_get_widget(data->XML,
                                     ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_FRAME_CHARACTER_NAME)));
  ACE_ASSERT(character_frame);
  gtk_widget_set_sensitive(GTK_WIDGET(character_frame), TRUE);

  // make drop button sensitive (if it's not already)
  GtkButton* button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_DROP_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make save button sensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_STORE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make load button insensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_LOAD_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make join button sensitive (if appropriate)
  GtkComboBox* repository_combobox =
      GTK_COMBO_BOX(glade_xml_get_widget(data->XML,
                                         ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_MAP_NAME)));
  ACE_ASSERT(repository_combobox);
  if (gtk_combo_box_get_active(repository_combobox) != -1)
  {
    button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                             ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_SERVER_JOIN_NAME)));
    ACE_ASSERT(button);
    gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
  } // end IF

  // make this insensitive
  gtk_widget_set_sensitive(widget_in, FALSE);

  return FALSE;
}

G_MODULE_EXPORT gint
drop_character_clicked_GTK_cb(GtkWidget* widget_in,
                              gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::drop_character_clicked_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  //   ACE_UNUSED_ARG(userData_in);
  RPG_Client_GTK_CBData_t* data =
      static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);

  // clean up
  if (data->entity.character)
  {
    delete data->entity.character;
    data->entity.character = NULL;
    data->entity.position =
        std::make_pair(std::numeric_limits<unsigned int>::max(),
                       std::numeric_limits<unsigned int>::max());
    data->entity.modes.clear();
    data->entity.actions.clear();
    data->entity.is_spawned = false;
  } // end IF

  // reset profile widgets
  ::reset_character_profile(data->XML);
	GtkImage* image =
		GTK_IMAGE(glade_xml_get_widget(data->XML,
		                               ACE_TEXT_ALWAYS_CHAR("image_sprite")));
	ACE_ASSERT(image);
	gtk_image_clear(image);

  // make character display frame insensitive (if it's not already)
  GtkFrame* character_frame =
      GTK_FRAME(glade_xml_get_widget(data->XML,
                                     ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_FRAME_CHARACTER_NAME)));
  ACE_ASSERT(character_frame);
  gtk_widget_set_sensitive(GTK_WIDGET(character_frame), FALSE);

  // make create button sensitive (if it's not already)
  GtkButton* button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_CREATE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make save button insensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_STORE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make load button sensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_LOAD_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make join button insensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_SERVER_JOIN_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make this insensitive
  gtk_widget_set_sensitive(widget_in, FALSE);

  // make equip button insensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_EQUIP_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make rest button insensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_REST_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  return FALSE;
}

G_MODULE_EXPORT gint
load_character_clicked_GTK_cb(GtkWidget* widget_in,
                              gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::load_character_clicked_GTK_cb"));

  RPG_Client_GTK_CBData_t* data =
      static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);
  ACE_ASSERT(data->XML);
  ACE_ASSERT(data->entity_filter);
  ACE_ASSERT(data->level_engine);

  // step1a: retrieve file chooser dialog handle
  GtkFileChooserDialog* filechooser_dialog =
      GTK_FILE_CHOOSER_DIALOG(glade_xml_get_widget(data->XML,
                                                   ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_FILECHOOSER_NAME)));
  ACE_ASSERT(filechooser_dialog);

  // step1b: setup chooser dialog
  std::string profiles_directory =
      RPG_Player_Common_Tools::getPlayerProfilesDirectory();
  gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooser_dialog),
                                      ACE_TEXT(profiles_directory.c_str()));
	// *TODO*: this crashes on WIN
  gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(filechooser_dialog),
                              data->entity_filter);

  // step1c: run chooser dialog
  gint response_id = gtk_dialog_run(GTK_DIALOG(filechooser_dialog));
  if (response_id == -1)
    return FALSE;

  // retrieve selected filename
  std::string filename =
      gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooser_dialog));

  // clean up
  if (data->entity.character)
  {
    delete data->entity.character;
    data->entity.character = NULL;
    data->entity.position =
        std::make_pair(std::numeric_limits<unsigned int>::max(),
                       std::numeric_limits<unsigned int>::max());
    data->entity.modes.clear();
    data->entity.actions.clear();
    data->entity.is_spawned = false;
  } // end IF

  // load player profile
  RPG_Character_Conditions_t condition;
  condition.insert(CONDITION_NORMAL);
  short int hitpoints = std::numeric_limits<short int>::max();
  RPG_Magic_Spells_t spells;
  data->entity.character = RPG_Player::load(filename,
                                            data->schema_repository,
                                            condition,
                                            hitpoints,
                                            spells);
  ACE_ASSERT(data->entity.character);

  // update entity profile widgets
  ::update_entity_profile(data->entity,
                          data->XML);

  // if necessary, update starting position
  if (data->entity.position ==
      std::make_pair(std::numeric_limits<unsigned int>::max(),
                     std::numeric_limits<unsigned int>::max()))
    data->entity.position = data->level_engine->getStartPosition();

  // make character display frame sensitive (if it's not already)
  GtkFrame* character_frame =
      GTK_FRAME(glade_xml_get_widget(data->XML,
                                     ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_FRAME_CHARACTER_NAME)));
  ACE_ASSERT(character_frame);
  gtk_widget_set_sensitive(GTK_WIDGET(character_frame), TRUE);

  // make create button insensitive (if it's not already)
  GtkButton* button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_CREATE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make drop button sensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_DROP_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make load button insensitive (if it's not already)
  gtk_widget_set_sensitive(widget_in, FALSE);

  // make save button insensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_STORE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make join button sensitive (if appropriate)
  GtkComboBox* repository_combobox =
      GTK_COMBO_BOX(glade_xml_get_widget(data->XML,
                                         ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_MAP_NAME)));
  ACE_ASSERT(repository_combobox);
  if (gtk_combo_box_get_active(repository_combobox) != -1)
  {
    button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                             ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_SERVER_JOIN_NAME)));
    ACE_ASSERT(button);
    gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
  } // end IF

  return FALSE;
}

G_MODULE_EXPORT gint
save_character_clicked_GTK_cb(GtkWidget* widget_in,
                              gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::save_character_clicked_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  RPG_Client_GTK_CBData_t* data =
      static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->entity.character);

  // assemble target filename
  std::string profiles_directory =
      RPG_Player_Common_Tools::getPlayerProfilesDirectory();
  std::string filename = profiles_directory;
  filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  filename += data->entity.character->getName();
  filename += ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_PROFILE_EXT);

  RPG_Player* player_p = NULL;
  try
  {
    player_p = dynamic_cast<RPG_Player*>(data->entity.character);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in dynamic_cast<RPG_Player*>(%@), aborting\n"),
               data->entity.character));

    return FALSE;
  }
  if (!player_p)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to dynamic_cast<RPG_Player*>(%@), aborting\n"),
               data->entity.character));

    return FALSE;
  }
  if (!player_p->save(filename))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Player::save(\"%s\"), continuing\n"),
               ACE_TEXT(filename.c_str())));

  // make create button sensitive (if it's not already)
  GtkButton* button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_CREATE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make drop button insensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_DROP_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make save button insensitive
  gtk_widget_set_sensitive(widget_in, FALSE);

  return FALSE;
}

G_MODULE_EXPORT gint
character_repository_combobox_changed_GTK_cb(GtkWidget* widget_in,
                                             gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::character_repository_combobox_changed_GTK_cb"));

  RPG_Client_GTK_CBData_t* data =
      static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(widget_in);
  ACE_ASSERT(data->XML);

  // retrieve active item
  std::string active_item;
  GtkTreeIter selected;
  GtkTreeModel* model = NULL;
  GValue value;
  const gchar* text = NULL;
	GtkVBox* vbox =
		GTK_VBOX(glade_xml_get_widget(data->XML,
		                              ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_VBOX_TOOLS_NAME)));
	ACE_ASSERT(vbox);
	GtkFrame* frame =
		GTK_FRAME(glade_xml_get_widget(data->XML,
		                               ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_FRAME_CHARACTER_NAME)));
	ACE_ASSERT(frame);
  if (!gtk_combo_box_get_active_iter(GTK_COMBO_BOX(widget_in), &selected))
  {
    // *WARNING*: refreshing the combobox triggers removal of items
    // which also generates this signal...

		// clean up
		::reset_character_profile(data->XML);
		GtkImage* image =
			GTK_IMAGE(glade_xml_get_widget(data->XML,
			                               ACE_TEXT_ALWAYS_CHAR("image_sprite")));
		ACE_ASSERT(image);
		gtk_image_clear(image);
		// desensitize tools vbox
		gtk_widget_set_sensitive(GTK_WIDGET(vbox), FALSE);
		// remove character frame widget
		gtk_widget_set_sensitive(GTK_WIDGET(frame), FALSE);
		gtk_widget_hide(GTK_WIDGET(frame));

    return FALSE;
  } // end IF
  model = gtk_combo_box_get_model(GTK_COMBO_BOX(widget_in));
  ACE_ASSERT(model);
  ACE_OS::memset(&value, 0, sizeof(value));
  gtk_tree_model_get_value(model, &selected,
                           0, &value);
  text = g_value_get_string(&value);
  // sanity check
  ACE_ASSERT(text);
  active_item = text;
  g_value_unset(&value);

  // clean up
  if (data->entity.character)
  {
    delete data->entity.character;
    data->entity.character = NULL;
    data->entity.position =
        std::make_pair(std::numeric_limits<unsigned int>::max(),
                       std::numeric_limits<unsigned int>::max());
    data->entity.modes.clear();
    data->entity.actions.clear();
    data->entity.is_spawned = false;
  } // end IF

  // construct filename
  std::string profiles_directory =
      RPG_Player_Common_Tools::getPlayerProfilesDirectory();
  std::string filename = profiles_directory;
  filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  filename += active_item;
  filename += ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_PROFILE_EXT);

  // load player profile
  RPG_Character_Conditions_t condition;
  condition.insert(CONDITION_NORMAL);
  short int hitpoints = std::numeric_limits<short int>::max();
  RPG_Magic_Spells_t spells;
  data->entity.character = RPG_Player::load(filename,
                                            data->schema_repository,
                                            condition,
                                            hitpoints,
                                            spells);
  if (!data->entity.character)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Player::load(\"%s\"), aborting\n"),
               ACE_TEXT(filename.c_str())));

    return FALSE;
  } // end IF

  // update entity profile widgets
  ::update_entity_profile(data->entity,
                          data->XML);

	// sensitize tools vbox
	gtk_widget_set_sensitive(GTK_WIDGET(vbox), TRUE);
  // make character frame visible/sensitive (if it's not already)
	gtk_widget_show(GTK_WIDGET(frame));
	gtk_widget_set_sensitive(GTK_WIDGET(frame), TRUE);

  // make join button sensitive IFF player is not disabled
  GtkButton* button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_SERVER_JOIN_NAME)));
  ACE_ASSERT(button);
  if (!RPG_Engine_Common_Tools::isCharacterDisabled(data->entity.character))
    gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make equip button sensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_EQUIP_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  return FALSE;
}

G_MODULE_EXPORT gint
character_repository_button_clicked_GTK_cb(GtkWidget* widget_in,
                                           gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::character_repository_button_clicked_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  RPG_Client_GTK_CBData_t* data =
      static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->XML);

  // retrieve tree model
  GtkComboBox* repository_combobox =
      GTK_COMBO_BOX(glade_xml_get_widget(data->XML,
                                         ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
  ACE_ASSERT(repository_combobox);
  GtkTreeModel* model = gtk_combo_box_get_model(repository_combobox);
  ACE_ASSERT(model);

  // re-load profile data
  unsigned int num_entries =
      ::load_files(REPOSITORY_PROFILES,
                   GTK_LIST_STORE(model));

  // set sensitive as appropriate
  GtkFrame* character_frame =
      GTK_FRAME(glade_xml_get_widget(data->XML,
                                     ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_FRAME_CHARACTER_NAME)));
  ACE_ASSERT(character_frame);

  // ... sensitize/activate widgets as appropriate
  if (num_entries)
  {
    gtk_widget_set_sensitive(GTK_WIDGET(repository_combobox), TRUE);
    gtk_combo_box_set_active(repository_combobox, 0);
  } // end IF
  else
  {
    gtk_widget_set_sensitive(GTK_WIDGET(repository_combobox), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(character_frame), FALSE);

    // make create button sensitive (if it's not already)
    GtkButton* button =
        GTK_BUTTON(glade_xml_get_widget(data->XML,
                                        ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_CREATE_NAME)));
    ACE_ASSERT(button);
    gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

    // make load button sensitive (if it's not already)
    button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                             ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_LOAD_NAME)));
    ACE_ASSERT(button);
    gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
  } // end ELSE

  return FALSE;
}

G_MODULE_EXPORT gint
create_map_clicked_GTK_cb(GtkWidget* widget_in,
                          gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::create_map_clicked_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  RPG_Client_GTK_CBData_t* data =
		static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);
  ACE_ASSERT(data->XML);
  ACE_ASSERT(data->level_engine);
  ACE_ASSERT(data->client_engine);

  // step1: get map name...
  // step1a: setup entry dialog
  GtkEntry* main_entry_dialog_entry =
		GTK_ENTRY(glade_xml_get_widget(data->XML,
																	 ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_ENTRY_NAME)));
  ACE_ASSERT(main_entry_dialog_entry);
  gtk_entry_buffer_delete_text(gtk_entry_get_buffer(main_entry_dialog_entry),
                               0, -1);
  // enforce sane values
  gtk_entry_set_max_length(main_entry_dialog_entry,
                           RPG_ENGINE_LEVEL_NAME_MAX_LENGTH);
//   gtk_entry_set_width_chars(main_entry_dialog_entry,
//                             -1); // reset to default
  gtk_entry_set_text(main_entry_dialog_entry,
                     ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_DEF_NAME));
  gtk_editable_select_region(GTK_EDITABLE(main_entry_dialog_entry),
                             0, -1);
  // step1b: retrieve entry dialog handle
  GtkDialog* main_entry_dialog =
		GTK_DIALOG(glade_xml_get_widget(data->XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_ENTRY_NAME)));
  ACE_ASSERT(main_entry_dialog);
  // step1c: run entry dialog
  gint response_id = gtk_dialog_run(main_entry_dialog);
  if (response_id == -1)
    return FALSE;

  // step1c: convert UTF8 --> locale
  const gchar* text = gtk_entry_get_text(main_entry_dialog_entry);
  ACE_ASSERT(text);
  gchar* converted_text = NULL;
  GError* conversion_error = NULL;
  converted_text = g_locale_from_utf8(text,               // text
                                      -1,                 // length in bytes (-1: \0-terminated)
                                      NULL,               // bytes read (don't care)
                                      NULL,               // bytes written (don't care)
                                      &conversion_error); // return value: error
  if (conversion_error)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert string: \"%s\", continuing\n"),
               ACE_TEXT(conversion_error->message)));

    // clean up
    g_error_free(conversion_error);
  } // end IF
  else
   data->level_metadata.name = converted_text;
  // clean up
  g_free(converted_text);

  // step2: create new (random) map
  RPG_Engine_Level_t level;
  RPG_Engine_Level::random(data->level_metadata,
                           data->map_configuration,
                           level);

  // step3: assign new map to level engine
  if (data->level_engine->isRunning())
    data->level_engine->stop();
	data->level_engine->set(level);
  data->level_engine->start();

  // make "this" insensitive
  gtk_widget_set_sensitive(widget_in, FALSE);

  // make drop button sensitive (if it's not already)
  GtkButton* button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_DROP_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make save button sensitive (if it's not already)
  button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_STORE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make load button insensitive (if it's not already)
  button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_LOAD_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make join button sensitive (if appropriate)
  GtkComboBox* repository_combobox =
		GTK_COMBO_BOX(glade_xml_get_widget(data->XML,
																			 ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
  ACE_ASSERT(repository_combobox);
  if (gtk_combo_box_get_active(repository_combobox) != -1)
  {
    button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                             ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_SERVER_JOIN_NAME)));
    ACE_ASSERT(button);
    gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
  } // end IF

  return FALSE;
}

G_MODULE_EXPORT gint
drop_map_clicked_GTK_cb(GtkWidget* widget_in,
                        gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::drop_map_clicked_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  //   ACE_UNUSED_ARG(userData_in);
  RPG_Client_GTK_CBData_t* data =
		static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);
  ACE_ASSERT(data->level_engine);
  ACE_ASSERT(data->client_engine);

  RPG_Engine_Level_t level;
  level.metadata = data->level_metadata;
  level.map.start = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                   std::numeric_limits<unsigned int>::max());
  level.map.seeds.clear();
  level.map.plan.size_x = 0;
  level.map.plan.size_y = 0;
  level.map.plan.unmapped.clear();
  level.map.plan.walls.clear();
  level.map.plan.doors.clear();
  level.map.plan.rooms_are_square = false;

  data->level_engine->stop();
  // assign empty map to level engine
  data->level_engine->set(level);

  // make "this" insensitive
  gtk_widget_set_sensitive(widget_in, FALSE);

  // make create_map button sensitive (if it's not already)
  GtkButton* button =
		GTK_BUTTON(glade_xml_get_widget(data->XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_CREATE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make map_load button sensitive (if it's not already)
  button =
		GTK_BUTTON(glade_xml_get_widget(data->XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_LOAD_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make join button insensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_SERVER_JOIN_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  return FALSE;
}

G_MODULE_EXPORT gint
load_map_clicked_GTK_cb(GtkWidget* widget_in,
                        gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::load_map_clicked_GTK_cb"));

  RPG_Client_GTK_CBData_t* data =
		static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);
  ACE_ASSERT(data->XML);
  ACE_ASSERT(data->entity_filter);
  ACE_ASSERT(data->level_engine);

  // step1a: retrieve file chooser dialog handle
  GtkFileChooserDialog* filechooser_dialog =
		GTK_FILE_CHOOSER_DIALOG(glade_xml_get_widget(data->XML,
																								 ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_FILECHOOSER_NAME)));
  ACE_ASSERT(filechooser_dialog);

  // step1b: setup chooser dialog
	std::string maps_directory = RPG_Map_Common_Tools::getMapsDirectory();
  gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooser_dialog),
                                      ACE_TEXT(maps_directory.c_str()));
  gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(filechooser_dialog),
                              data->map_filter);

  // step1c: run chooser dialog
  gint response_id = gtk_dialog_run(GTK_DIALOG(filechooser_dialog));
  if (response_id == -1)
    return FALSE;

  // retrieve selected filename
  std::string filename =
		gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooser_dialog));

  // load level
  RPG_Engine_Level_t level;
  if (!RPG_Engine_Level::load(filename,
                              data->schema_repository,
                              level))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Engine_Level::load(\"%s\"), aborting"),
               ACE_TEXT(filename.c_str())));

    return FALSE;
  } // end IF
  if (data->level_engine->isRunning())
    data->level_engine->stop();
  data->level_engine->set(level);
  data->level_engine->start();

  // make create button insensitive (if it's not already)
  GtkButton* button =
		GTK_BUTTON(glade_xml_get_widget(data->XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_CREATE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make map_drop button sensitive (if it's not already)
  button =
		GTK_BUTTON(glade_xml_get_widget(data->XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_DROP_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make load button insensitive (if it's not already)
  gtk_widget_set_sensitive(widget_in, FALSE);

  // make map_save button insensitive (if it's not already)
  button =
		GTK_BUTTON(glade_xml_get_widget(data->XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_STORE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make join button sensitive (if appropriate)
  GtkComboBox* repository_combobox =
		GTK_COMBO_BOX(glade_xml_get_widget(data->XML,
																			 ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
  ACE_ASSERT(repository_combobox);
  if (gtk_combo_box_get_active(repository_combobox) != -1)
  {
    button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                             ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_SERVER_JOIN_NAME)));
    ACE_ASSERT(button);
    gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
  } // end IF

  return FALSE;
}

G_MODULE_EXPORT gint
save_map_clicked_GTK_cb(GtkWidget* widget_in,
                        gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::save_map_clicked_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  RPG_Client_GTK_CBData_t* data =
		static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);
  ACE_ASSERT(data->XML);

  std::string filename = RPG_Map_Common_Tools::getMapsDirectory();
  filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  filename += data->level_engine->getMetaData(true).name;
  filename += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT);
  data->level_engine->save(filename);

  // make create button sensitive (if it's not already)
  GtkButton* button =
		GTK_BUTTON(glade_xml_get_widget(data->XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_CREATE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make drop button insensitive (if it's not already)
  button =
		GTK_BUTTON(glade_xml_get_widget(data->XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_DROP_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make save button insensitive
  gtk_widget_set_sensitive(widget_in, FALSE);

  return FALSE;
}

G_MODULE_EXPORT gint
map_repository_combobox_changed_GTK_cb(GtkWidget* widget_in,
                                       gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::map_repository_combobox_changed_GTK_cb"));

  RPG_Client_GTK_CBData_t* data =
		static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(widget_in);
  ACE_ASSERT(data->XML);
  ACE_ASSERT(data->level_engine);
  ACE_ASSERT(data->client_engine);

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
  ACE_OS::memset(&value, 0, sizeof(value));
  gtk_tree_model_get_value(model, &selected,
                           0, &value);
  text = g_value_get_string(&value);
  // sanity check
  ACE_ASSERT(text);
  active_item = text;
  g_value_unset(&value);

  // construct filename
  std::string filename = RPG_Map_Common_Tools::getMapsDirectory();
  filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  filename += active_item;
  filename += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT);

  // load level
  RPG_Engine_Level_t level;
  if (!RPG_Engine_Level::load(filename,
                              data->schema_repository,
                              level))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Engine_Level::load(\"%s\"), aborting"),
               ACE_TEXT(filename.c_str())));

    return FALSE;
  } // end IF
  bool engine_was_running = data->level_engine->isRunning();
  if (engine_was_running)
    data->level_engine->stop();
  data->level_engine->set(level);
  if (engine_was_running)
    data->level_engine->start();

  // make map_save button insensitive (if it's not already)
  GtkButton* button =
		GTK_BUTTON(glade_xml_get_widget(data->XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_STORE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  return FALSE;
}

G_MODULE_EXPORT gint
map_repository_button_clicked_GTK_cb(GtkWidget* widget_in,
                                     gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::map_repository_button_clicked_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  RPG_Client_GTK_CBData_t* data =
      static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);
  ACE_ASSERT(data->XML);

  // retrieve tree model
  GtkComboBox* repository_combobox =
      GTK_COMBO_BOX(glade_xml_get_widget(data->XML,
                                         ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_MAP_NAME)));
  ACE_ASSERT(repository_combobox);
  GtkTreeModel* model = gtk_combo_box_get_model(repository_combobox);
  ACE_ASSERT(model);

  // re-load maps data
  unsigned int num_entries =
      ::load_files(REPOSITORY_MAPS,
                   GTK_LIST_STORE(model));

  // ... sensitize/activate widgets as appropriate
  if (num_entries)
  {
    gtk_widget_set_sensitive(GTK_WIDGET(repository_combobox), TRUE);
    gtk_combo_box_set_active(repository_combobox, 0);
  } // end IF
  else
  {
    // make join button insensitive (if it's not already)
    GtkButton* button =
        GTK_BUTTON(glade_xml_get_widget(data->XML,
                                        ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_SERVER_JOIN_NAME)));
    ACE_ASSERT(button);
    gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
  } // end ELSE

  return FALSE;
}

G_MODULE_EXPORT gint
load_state_clicked_GTK_cb(GtkWidget* widget_in,
                          gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::load_state_clicked_GTK_cb"));

	RPG_Client_GTK_CBData_t* data =
		static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
	ACE_ASSERT(data);
	ACE_ASSERT(data->XML);
	ACE_ASSERT(data->entity_filter);
	ACE_ASSERT(data->level_engine);

  // step1a: retrieve file chooser dialog handle
  GtkFileChooserDialog* filechooser_dialog =
    GTK_FILE_CHOOSER_DIALOG(glade_xml_get_widget(data->XML,
                                                 ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_FILECHOOSER_NAME)));
  ACE_ASSERT(filechooser_dialog);

	// step1b: setup chooser dialog
	std::string savedstate_directory =
			RPG_Engine_Common_Tools::getEngineStateDirectory();
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooser_dialog),
																			ACE_TEXT(savedstate_directory.c_str()));
	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(filechooser_dialog),
															data->savedstate_filter);

  // step1c: run chooser dialog
  gint response_id = gtk_dialog_run(GTK_DIALOG(filechooser_dialog));
  gtk_widget_hide(GTK_WIDGET(filechooser_dialog));
  if (response_id == -1)
    return FALSE;

  // retrieve selected filename
  std::string filename =
    gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooser_dialog));

  // load saved state
  if (!data->level_engine->load(filename,
                                data->schema_repository))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Engine::load(\"%s\"), aborting"),
               ACE_TEXT(filename.c_str())));

    return FALSE;
  } // end IF

  // make state_save button sensitive (if it is not already)
  GtkButton* button =
    GTK_BUTTON(glade_xml_get_widget(data->XML,
                                    ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_ENGINESTATE_STORE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make join button in-sensitive (if it is not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_SERVER_JOIN_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
  // make part button sensitive (if it is not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_SERVER_PART_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  return FALSE;
}

G_MODULE_EXPORT gint
save_state_clicked_GTK_cb(GtkWidget* widget_in,
                          gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::save_state_clicked_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  RPG_Client_GTK_CBData_t* data =
    static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);
  ACE_ASSERT(data->XML);

  // get descriptor...
  // step1a: setup entry dialog
  GtkEntry* entry =
      GTK_ENTRY(glade_xml_get_widget(data->XML,
                                     ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_ENTRY_NAME)));
  ACE_ASSERT(entry);
  gtk_entry_buffer_delete_text(gtk_entry_get_buffer(entry),
                               0, -1);
  // enforce sane values
  gtk_entry_set_max_length(entry,
                           RPG_ENGINE_LEVEL_NAME_MAX_LENGTH);
//   gtk_entry_set_width_chars(entry,
//                             -1); // reset to default
  gtk_entry_set_text(entry,
                     ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_STATE_DEF_DESCRIPTOR));
  gtk_editable_select_region(GTK_EDITABLE(entry),
                             0, -1);
  // step1b: retrieve entry dialog handle
  GtkDialog* entry_dialog =
    GTK_DIALOG(glade_xml_get_widget(data->XML,
                                    ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_ENTRY_NAME)));
  ACE_ASSERT(entry_dialog);
  // step1b: run entry dialog
  gint response_id = gtk_dialog_run(entry_dialog);
  gtk_widget_hide(GTK_WIDGET(entry_dialog));
  if (response_id == -1)
    return FALSE;
  // step1c: convert UTF8 --> locale
  const gchar* text = gtk_entry_get_text(entry);
  ACE_ASSERT(text);
  gchar* converted_text = NULL;
  GError* conversion_error = NULL;
  converted_text = g_locale_from_utf8(text,               // text
                                      -1,                 // length in bytes (-1: \0-terminated)
                                      NULL,               // bytes read (don't care)
                                      NULL,               // bytes written (don't care)
                                      &conversion_error); // return value: error
  if (conversion_error)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert string: \"%s\", continuing\n"),
               ACE_TEXT(conversion_error->message)));

    // clean up
    g_error_free(conversion_error);
  } // end IF
  std::string descriptor = converted_text;
  g_free(converted_text);
  if (!data->level_engine->save(descriptor))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Engine::save(), continuing\n")));

  return FALSE;
}

G_MODULE_EXPORT gint
state_repository_combobox_changed_GTK_cb(GtkWidget* widget_in,
                                         gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::state_repository_combobox_changed_GTK_cb"));

	RPG_Client_GTK_CBData_t* data =
		static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
	ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(widget_in);
  ACE_ASSERT(data->XML);
  ACE_ASSERT(data->level_engine);
  ACE_ASSERT(data->client_engine);

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
  ACE_OS::memset(&value, 0, sizeof(value));
  gtk_tree_model_get_value(model, &selected,
                           0, &value);
  text = g_value_get_string(&value);
  // sanity check
  ACE_ASSERT(text);
  active_item = text;
  g_value_unset(&value);

  // construct filename
  std::string filename = RPG_Engine_Common_Tools::getEngineStateDirectory();
  filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  filename += active_item;
  filename += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_STATE_EXT);

  // load state
  if (!data->level_engine->load(filename,
                                data->schema_repository))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Engine::load(\"%s\"), aborting"),
               ACE_TEXT(filename.c_str())));

    return FALSE;
  } // end IF

  // make state_save button sensitive (if it is not already)
  GtkButton* button =
    GTK_BUTTON(glade_xml_get_widget(data->XML,
                                    ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_ENGINESTATE_STORE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make join button in-sensitive (if it is not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_SERVER_JOIN_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
  // make part button sensitive (if it is not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_SERVER_PART_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  return FALSE;
}

G_MODULE_EXPORT gint
state_repository_button_clicked_GTK_cb(GtkWidget* widget_in,
                                       gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::state_repository_button_clicked_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  RPG_Client_GTK_CBData_t* data =
      static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);
  ACE_ASSERT(data->XML);

  // retrieve tree model
  GtkComboBox* repository_combobox =
      GTK_COMBO_BOX(glade_xml_get_widget(data->XML,
                                         ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_ENGINESTATE_NAME)));
  ACE_ASSERT(repository_combobox);
  GtkTreeModel* model = gtk_combo_box_get_model(repository_combobox);
  ACE_ASSERT(model);

  // re-load savedstates data
  unsigned int num_entries =
      ::load_files(REPOSITORY_ENGINESTATE,
                   GTK_LIST_STORE(model));

  // ... sensitize/activate widgets
  gtk_widget_set_sensitive(GTK_WIDGET(repository_combobox),
                           (num_entries > 0));
  gtk_combo_box_set_active(repository_combobox, -1);

  return FALSE;
}

G_MODULE_EXPORT gint
join_game_clicked_GTK_cb(GtkWidget* widget_in,
                         gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::join_game_clicked_GTK_cb"));

  ACE_ASSERT(widget_in);
  RPG_Client_GTK_CBData_t* data =
      static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->client_engine);
  ACE_ASSERT(data->entity.character);
  ACE_ASSERT(data->level_engine);
  if (data->level_engine->isRunning())
    data->level_engine->stop();

  // set start position, if necessary
  if (data->entity.position ==
      std::make_pair(std::numeric_limits<unsigned int>::max(),
                     std::numeric_limits<unsigned int>::max()))
    data->entity.position = data->level_engine->getStartPosition(true);

  // update the level state

  // activate the current character
  RPG_Engine_EntityID_t id = data->level_engine->add(&(data->entity));
  data->level_engine->setActive(id);

  // center on character
  data->client_engine->setView(data->entity.position);

  // play ambient sound
  RPG_SOUND_EVENT_MANAGER_SINGLETON::instance()->start();

  // make join button INsensitive
  gtk_widget_set_sensitive(widget_in, FALSE);

  // make part button sensitive
  GtkButton* button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_SERVER_PART_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make create button insensitive
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_CREATE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make drop button insensitive
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_DROP_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make load button insensitive
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_LOAD_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make save button insensitive
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_STORE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make create_map button insensitive
  button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_CREATE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make drop_map button insensitive
  button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_DROP_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make load_map button insensitive
  button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_LOAD_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make save_map button insensitive
  button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_STORE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make character combox insensitive
  GtkComboBox* combo_box =
      GTK_COMBO_BOX(glade_xml_get_widget(data->XML,
                                         ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
  ACE_ASSERT(combo_box);
  gtk_widget_set_sensitive(GTK_WIDGET(combo_box), FALSE);

  // make map combobox insensitive
  combo_box =
      GTK_COMBO_BOX(glade_xml_get_widget(data->XML,
                                         ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_MAP_NAME)));
  ACE_ASSERT(combo_box);
  gtk_widget_set_sensitive(GTK_WIDGET(combo_box), FALSE);

  // make quit button insensitive
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_QUIT_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // minimize dialog window
  GdkWindow* toplevel = gtk_widget_get_parent_window(widget_in);
  ACE_ASSERT(toplevel);
  gdk_window_iconify(toplevel);

  return FALSE;
}

G_MODULE_EXPORT gint
part_game_clicked_GTK_cb(GtkWidget* widget_in,
                         gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::part_game_clicked_GTK_cb"));

  ACE_ASSERT(widget_in);
  RPG_Client_GTK_CBData_t* data =
      static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->level_engine);

  // deactivate the current character
  RPG_Engine_EntityID_t id = data->level_engine->getActive();
  if (id)
    data->level_engine->remove(id);

  // stop ambient sound
  RPG_SOUND_EVENT_MANAGER_SINGLETON::instance()->stop();

  // update entity profile widgets
  ::update_entity_profile(data->entity,
                          data->XML);

  // make part button insensitive
  gtk_widget_set_sensitive(widget_in, FALSE);

  // make join button sensitive IFF player is not disabled
  GtkButton* button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_SERVER_JOIN_NAME)));
  ACE_ASSERT(button);
  if (!RPG_Engine_Common_Tools::isCharacterDisabled(data->entity.character))
    gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make drop button sensitive
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_DROP_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make save button sensitive
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_STORE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make map create button sensitive
  button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_CREATE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make map load button sensitive
  button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_LOAD_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make character combox sensitive
  GtkComboBox* combo_box =
      GTK_COMBO_BOX(glade_xml_get_widget(data->XML,
                                         ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
  ACE_ASSERT(combo_box);
  gtk_widget_set_sensitive(GTK_WIDGET(combo_box), TRUE);

  // make map combobox sensitive
  combo_box =
      GTK_COMBO_BOX(glade_xml_get_widget(data->XML,
                                         ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_MAP_NAME)));
  ACE_ASSERT(combo_box);
  gtk_widget_set_sensitive(GTK_WIDGET(combo_box), TRUE);

  // make quit button insensitive
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_QUIT_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  return FALSE;
}

G_MODULE_EXPORT gint
equip_clicked_GTK_cb(GtkWidget* widget_in,
                     gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::equip_clicked_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  RPG_Client_GTK_CBData_t* data =
      static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->entity.character);
  ACE_ASSERT(data->entity.character->isPlayerCharacter());
  RPG_Player* player = dynamic_cast<RPG_Player*>(data->entity.character);
  ACE_ASSERT(player);
  ACE_ASSERT(data->XML);

  // retrieve about dialog handle
  GtkWidget* equipment_dialog =
      GTK_WIDGET(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_EQUIPMENT_NAME)));
  ACE_ASSERT(equipment_dialog);
  if (!equipment_dialog)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
               ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_EQUIPMENT_NAME)));

    return TRUE; // propagate
  } // end IF

  ::update_equipment(*data);

  // draw it
  if (!GTK_WIDGET_VISIBLE(equipment_dialog))
    gtk_widget_show_all(equipment_dialog);

  return FALSE;
}

G_MODULE_EXPORT gint
item_toggled_GTK_cb(GtkWidget* widget_in,
                    gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::item_toggled_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  RPG_Client_GTK_CBData_t* data =
      static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->entity.character);
  ACE_ASSERT(data->entity.character->isPlayerCharacter());
	RPG_Player* player = NULL;
	try
	{
		player = dynamic_cast<RPG_Player*>(data->entity.character);
	}
	catch (...)
	{
		player = NULL;
	}
	if (!player)
	{
		ACE_DEBUG((LM_ERROR,
			         ACE_TEXT("failed to dynamic_cast<RPG_Player*>(%@), aborting\n"),
							 data->entity.character));

		return TRUE; // propagate
	} // end IF
  ACE_ASSERT(data->level_engine);
  ACE_ASSERT(data->client_engine);

  // retrieve item id
  std::string widget_name = gtk_widget_get_name(widget_in);
  std::istringstream input(widget_name);
  RPG_Item_ID_t item_id = 0;
  if (!(input >> item_id))
  {
    ACE_DEBUG((LM_ERROR,
              ACE_TEXT("failed to retrieve item id (was: \"%s\"), aborting\n"),
							ACE_TEXT(widget_name.c_str())));

    return TRUE; // propagate
  } // end IF

	data->level_engine->lock();
  RPG_Engine_EntityID_t active_entity = data->level_engine->getActive(false);
  unsigned char visible_radius_before = 0;
  if (active_entity)
    visible_radius_before = data->level_engine->getVisibleRadius(active_entity,
		                                                             false);

  // *TODO*: where ?
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget_in)))
    player->getEquipment().equip(item_id,
                                 player->getOffHand(),
                                 EQUIPMENTSLOT_ANY);
  else
    player->getEquipment().unequip(item_id);

  // equipped light source --> update vision ?
  if (active_entity)
  {
		RPG_Engine_ClientNotificationParameters_t parameters;
		parameters.visible_radius =
			data->level_engine->getVisibleRadius(active_entity, false);
		if (visible_radius_before != parameters.visible_radius)
    {
      // notify client window
			parameters.entity_id = active_entity;
			parameters.condition = RPG_COMMON_CONDITION_INVALID;
			data->level_engine->getVisiblePositions(active_entity,
																							parameters.positions,
																							false);
			parameters.previous_position =
				std::make_pair(std::numeric_limits<unsigned int>::max(),
											 std::numeric_limits<unsigned int>::max());			
			data->level_engine->unlock();
      try
      {
        data->client_engine->notify(COMMAND_E2C_ENTITY_VISION, parameters);
      }
      catch (...)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("caught exception in RPG_Engine_IWindow::notify(\"%s\"), continuing\n"),
                   ACE_TEXT(RPG_Engine_CommandHelper::RPG_Engine_CommandToString(COMMAND_E2C_ENTITY_VISION).c_str())));
      }
			data->level_engine->lock();
    } // end IF
  } // end IF
	data->level_engine->unlock();

  ::update_equipment(*data);

  return FALSE;
}

G_MODULE_EXPORT gint
rest_clicked_GTK_cb(GtkWidget* widget_in,
                    gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::rest_clicked_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  RPG_Client_GTK_CBData_t* data =
      static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);

  // *TODO*
  ACE_ASSERT(false);

  return FALSE;
}

G_MODULE_EXPORT gint
server_repository_combobox_changed_GTK_cb(GtkWidget* widget_in,
                                          gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::server_repository_combobox_changed_GTK_cb"));

//   RPG_Client_GTK_CBData_t* data = static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
//   ACE_ASSERT(data);
//
//   // sanity check(s)
//   ACE_ASSERT(widget_in);
//   ACE_ASSERT(data->XML);
//
//   // retrieve active item
//   std::string active_item;
//   GtkTreeIter selected;
//   GtkTreeModel* model = NULL;
//   GValue value;
//   const gchar* text = NULL;
//   if (!gtk_combo_box_get_active_iter(GTK_COMBO_BOX(widget_in), &selected))
//   {
//     // *WARNING*: refreshing the combobox triggers removal of items
//     // which also generates this signal...
//     return FALSE;
//   } // end IF
//   model = gtk_combo_box_get_model(GTK_COMBO_BOX(widget_in));
//   ACE_ASSERT(model);
//   ACE_OS::memset(&value,
//                  0,
//                  sizeof(value));
//   gtk_tree_model_get_value(model, &selected,
//                            0, &value);
//   text = g_value_get_string(&value);
//   // sanity check
//   ACE_ASSERT(text);
//   active_item = text;
//   g_value_unset(&value);
//
//   // construct filename
//   std::string filename = RPG_CLIENT_DEF_CHARACTER_REPOSITORY;
//   filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
//   filename += active_item;
//   filename += RPG_CHARACTER_PROFILE_EXT;
//
//   // load player profile
//   data->entity = RPG_Engine_Common_Tools::loadEntity(filename,
//                                                      data->schema_repository);
//   ACE_ASSERT(data->entity.character);
//
//   // update entity profile widgets
//   ::update_entity_profile(data->entity,
//                           data->XML);
//
//   // make character display frame sensitive (if it's not already)
//   GtkFrame* character_frame = GTK_FRAME(glade_xml_get_widget(data->XML,
//                                                              ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_FRAME_CHARACTER_NAME)));
//   ACE_ASSERT(character_frame);
//   gtk_widget_set_sensitive(GTK_WIDGET(character_frame), TRUE);
//
//   // make join button sensitive (if it's not already)
//   GtkButton* join_game = GTK_BUTTON(glade_xml_get_widget(data->XML,
//                                                          ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_JOIN_NAME)));
//   ACE_ASSERT(join_game);
//   gtk_widget_set_sensitive(GTK_WIDGET(join_game), TRUE);

  return FALSE;
}

G_MODULE_EXPORT gint
server_repository_button_clicked_GTK_cb(GtkWidget* widget_in,
                                        gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::server_repository_button_clicked_GTK_cb"));

//   ACE_UNUSED_ARG(widget_in);
//   RPG_Client_GTK_CBData_t* data = static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
//   ACE_ASSERT(data);
//
//   // sanity check(s)
//   ACE_ASSERT(data->XML);
//
//   // retrieve tree model
//   GtkComboBox* repository_combobox = GTK_COMBO_BOX(glade_xml_get_widget(data->XML,
//                                                                          ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
//   ACE_ASSERT(repository_combobox);
//   GtkTreeModel* model = gtk_combo_box_get_model(repository_combobox);
//   ACE_ASSERT(model);
//
//   // re-load profile data
//   ::load_profiles(RPG_CLIENT_DEF_CHARACTER_REPOSITORY,
//                   GTK_LIST_STORE(model));
//
//   // set sensitive as appropriate
//   GtkFrame* character_frame = GTK_FRAME(glade_xml_get_widget(data->XML,
//                                                              ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_FRAME_CHARACTER_NAME)));
//   ACE_ASSERT(character_frame);
//   if (g_list_length(gtk_container_get_children(GTK_CONTAINER(repository_combobox))))
//   {
//     gtk_widget_set_sensitive(GTK_WIDGET(repository_combobox), TRUE);
//     gtk_widget_set_sensitive(GTK_WIDGET(character_frame), TRUE);
//   } // end IF
//   else
//   {
//     gtk_widget_set_sensitive(GTK_WIDGET(repository_combobox), FALSE);
//     gtk_widget_set_sensitive(GTK_WIDGET(character_frame), FALSE);
//   } // end ELSE
//
//   // ... activate first entry as appropriate
//   if (gtk_widget_is_sensitive(GTK_WIDGET(repository_combobox)))
//     gtk_combo_box_set_active(repository_combobox, 0);

  return FALSE;
}

// G_MODULE_EXPORT gint
// do_SDLEventLoop_GTK_cb(gpointer userData_in)
// {
//   RPG_TRACE(ACE_TEXT("::do_SDLEventLoop_GTK_cb"));
//
//   RPG_Client_GTK_CBData_t* data = static_cast<RPG_Client_GTK_CBData_t*> (userData_in);
//   ACE_ASSERT(data);
//
//   SDL_Event event;
//   RPG_Graphics_Position_t mouse_position = std::make_pair(0, 0);
//   RPG_Graphics_IWindow* window = NULL;
//   bool need_redraw = false;
//   bool done = false;
// //   while (SDL_WaitEvent(&event) > 0)
//   if (SDL_PollEvent(&event))
//   {
//     // if necessary, reset hover_time
//     if (event.type != RPG_GRAPHICS_SDL_HOVEREVENT)
//     {
//       // synch access
//       ACE_Guard<ACE_Thread_Mutex> aGuard(data->hover_quit_lock);
//
//       data->hover_time = 0;
//     } // end IF
//
//     switch (event.type)
//     {
//       case SDL_KEYDOWN:
//       {
//         switch (event.key.keysym.sym)
//         {
//           case SDLK_m:
//           {
//             std::string dump_path = RPG_MAP_DUMP_DIR;
//             dump_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
//             dump_path += ACE_TEXT("map.txt");
//             if (!RPG_Map_Common_Tools::save(dump_path,         // file
//                                             data->seed_points, // seed points
//                                             data->plan))       // plan
//             {
//               ACE_DEBUG((LM_ERROR,
//                          ACE_TEXT("failed to RPG_Map_Common_Tools::save(\"%s\"), aborting\n"),
//                          dump_path.c_str()));
//
//               done = true;
//             } // end IF
//
//             break;
//           }
//           case SDLK_q:
//           {
//             // finished event processing
//             done = true;
//
//             break;
//           }
//           default:
//           {
//             break;
//           }
//         } // end SWITCH
//
//         if (done)
//           break; // leave
//         // *WARNING*: falls through !
//       }
//       case SDL_ACTIVEEVENT:
//       case SDL_MOUSEMOTION:
//       case SDL_MOUSEBUTTONDOWN:
//       case RPG_GRAPHICS_SDL_HOVEREVENT: // hovering...
//       {
//         // find window
//         switch (event.type)
//         {
//           case SDL_MOUSEMOTION:
//             mouse_position = std::make_pair(event.motion.x,
//                                             event.motion.y); break;
//           case SDL_MOUSEBUTTONDOWN:
//             mouse_position = std::make_pair(event.button.x,
//                                             event.button.y); break;
//           default:
//           {
//             int x,y;
//             SDL_GetMouseState(&x, &y);
//             mouse_position = std::make_pair(x, y);
//
//             break;
//           }
//         } // end SWITCH
//
//         window = data->main_window->getWindow(mouse_position);
//         ACE_ASSERT(window);
//
//         // notify previously "active" window upon losing "focus"
//         if (event.type == SDL_MOUSEMOTION)
//         {
//           if (data->previous_window &&
//               // (data->previous_window != mainWindow)
//               (data->previous_window != window))
//           {
//             event.type = RPG_GRAPHICS_SDL_MOUSEMOVEOUT;
//             try
//             {
//               data->previous_window->handleEvent(event,
//                                                  data->previous_window,
//                                                  need_redraw);
//             }
//             catch (...)
//             {
//               ACE_DEBUG((LM_ERROR,
//                          ACE_TEXT("caught exception in RPG_Graphics_IWindow::handleEvent(), continuing\n")));
//             }
//             event.type = SDL_MOUSEMOTION;
//           } // end IF
//         } // end IF
//         // remember last "active" window
//         data->previous_window = window;
//
//         // notify "active" window
//         try
//         {
//           window->handleEvent(event,
//                               window,
//                               need_redraw);
//         }
//         catch (...)
//         {
//           ACE_DEBUG((LM_ERROR,
//                      ACE_TEXT("caught exception in RPG_Graphics_IWindow::handleEvent(), continuing\n")));
//         }
//
//         break;
//       }
//       case SDL_QUIT:
//       {
//         // finished event processing
//         done = true;
//
//         break;
//       }
//       case SDL_KEYUP:
//       case SDL_MOUSEBUTTONUP:
//       case SDL_JOYAXISMOTION:
//       case SDL_JOYBALLMOTION:
//       case SDL_JOYHATMOTION:
//       case SDL_JOYBUTTONDOWN:
//       case SDL_JOYBUTTONUP:
//       case SDL_SYSWMEVENT:
//       case SDL_VIDEORESIZE:
//       case SDL_VIDEOEXPOSE:
//       case RPG_CLIENT_SDL_TIMEREVENT:
//       default:
//       {
//
//         break;
//       }
//     } // end SWITCH
//
//         // redraw map ?
//     if (need_redraw)
//     {
//       try
//       {
//         data->map_window->draw();
//         data->map_window->refresh();
//       }
//       catch (...)
//       {
//         ACE_DEBUG((LM_ERROR,
//                    ACE_TEXT("caught exception in RPG_Graphics_IWindow::draw()/refresh(), continuing\n")));
//       }
//     } // end IF
//
//     // redraw cursor ?
//     switch (event.type)
//     {
//       case SDL_KEYDOWN:
//       case SDL_MOUSEBUTTONDOWN:
//       {
//         // map hasn't changed --> no need to redraw
//         if (!need_redraw)
//           break;
//
//         // *WARNING*: falls through !
//       }
//       case SDL_MOUSEMOTION:
//       case RPG_GRAPHICS_SDL_HOVEREVENT:
//       {
//         // map has changed, cursor MAY have been drawn over...
//         // --> redraw cursor
//         SDL_Rect dirtyRegion;
//         RPG_GRAPHICS_CURSOR_SINGLETON::instance()->put(mouse_position.first,
//                                                        mouse_position.second,
//                                                        data->screen,
//                                                        dirtyRegion);
//         RPG_Graphics_Surface::update(dirtyRegion,
//                                      data->screen);
//
//         break;
//       }
//       default:
//       {
//         break;
//       }
//     } // end SWITCH
//
//     if (done)
//     {
//       if (!SDL_RemoveTimer(data->event_timer))
//         ACE_DEBUG((LM_ERROR,
//                    ACE_TEXT("failed to SDL_RemoveTimer(): \"%s\", continuing\n"),
//                    SDL_GetError()));
//
//       // leave GTK
//       gtk_main_quit();
//
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("leaving...\n")));
//
//       // quit idle task
//       return 0;
//     } // end IF
//   } // end IF
//
//   // continue idle task
//   return 1;
// }

// G_MODULE_EXPORT gboolean
// gtk_quit_handler_cb(gpointer userData_in)
// {
//   RPG_TRACE(ACE_TEXT("::gtk_quit_handler_cb"));
//
//   RPG_Client_GTK_CBData_t* data = static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
//   ACE_ASSERT(data);
//
//   // synch access
//   {
//     ACE_Guard<ACE_Thread_Mutex> aGuard(data->hover_quit_lock);
//
//     ACE_ASSERT(!data->GTK_done);
//     data->GTK_done = true;
//   } // end lock scope
//
//   // de-register this hook
//   return FALSE;
// }

#ifdef __cplusplus
}
#endif /* __cplusplus */

bool
init_UI_client(const std::string& UIFile_in,
               RPG_Client_GTK_CBData_t& userData_in)
{
  RPG_TRACE(ACE_TEXT("::init_UI_client"));

  // sanity check(s)
  if (!RPG_Common_File_Tools::isReadable(UIFile_in.c_str()))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("UI definition file \"%s\" doesn't exist, aborting\n"),
               ACE_TEXT(UIFile_in.c_str())));

    return false;
  } // end IF

  // step1: load widget tree
  ACE_ASSERT(userData_in.XML == NULL);
  userData_in.XML = glade_xml_new(UIFile_in.c_str(), // definition file
                                  NULL,              // root widget --> construct all
                                  NULL);             // domain
  if (!userData_in.XML)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_new(): \"%m\", aborting\n")));

    return false;
  } // end IF

  // step2: init dialog windows
  GtkFileChooserDialog* filechooser_dialog =
		GTK_FILE_CHOOSER_DIALOG(glade_xml_get_widget(userData_in.XML,
																								 ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_FILECHOOSER_NAME)));
  ACE_ASSERT(filechooser_dialog);
  userData_in.map_filter = gtk_file_filter_new();
  if (!userData_in.map_filter)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_file_filter_new(): \"%m\", aborting\n")));

    // clean up
    g_object_unref(G_OBJECT(userData_in.XML));

    return false;
  } // end IF
  gtk_file_filter_set_name(userData_in.map_filter,
                           ACE_TEXT(RPG_ENGINE_LEVEL_FILE_EXT));
  std::string pattern = ACE_TEXT_ALWAYS_CHAR("*");
  pattern += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT);
  gtk_file_filter_add_pattern(userData_in.map_filter,
                              ACE_TEXT(pattern.c_str()));
  //gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(filechooser_dialog), userData_in.map_filter);
  //g_object_ref(G_OBJECT(userData_in.map_filter));
  userData_in.entity_filter = gtk_file_filter_new();
  if (!userData_in.entity_filter)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_file_filter_new(): \"%m\", aborting\n")));

    // clean up
    g_object_unref(G_OBJECT(userData_in.map_filter));
    g_object_unref(G_OBJECT(userData_in.XML));

    return false;
  } // end IF
	gtk_file_filter_set_name(userData_in.entity_filter,
													 ACE_TEXT(RPG_PLAYER_PROFILE_EXT));
  pattern = ACE_TEXT_ALWAYS_CHAR("*");
  pattern += ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_PROFILE_EXT);
  gtk_file_filter_add_pattern(userData_in.entity_filter,
		                          ACE_TEXT(pattern.c_str()));
  //gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(filechooser_dialog),
	//                            userData_in.entity_filter);
  //g_object_ref(G_OBJECT(userData_in.entity_filter));
  userData_in.savedstate_filter = gtk_file_filter_new();
  if (!userData_in.savedstate_filter)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_file_filter_new(): \"%m\", aborting\n")));

    // clean up
    g_object_unref(G_OBJECT(userData_in.entity_filter));
    g_object_unref(G_OBJECT(userData_in.map_filter));
    g_object_unref(G_OBJECT(userData_in.XML));

    return false;
  } // end IF
  gtk_file_filter_set_name(userData_in.savedstate_filter,
                           ACE_TEXT(RPG_ENGINE_STATE_EXT));
  pattern = ACE_TEXT_ALWAYS_CHAR("*");
  pattern += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_STATE_EXT);
  gtk_file_filter_add_pattern(userData_in.savedstate_filter,
                              ACE_TEXT(pattern.c_str()));
  //gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(filechooser_dialog), userData_in.savedstate_filter);
  //g_object_ref(G_OBJECT(userData_in.savedstate_filter));

  GtkWidget* about_dialog =
		GTK_WIDGET(glade_xml_get_widget(userData_in.XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_ABOUT_NAME)));
  ACE_ASSERT(about_dialog);

  GtkWidget* equipment_dialog =
		GTK_WIDGET(glade_xml_get_widget(userData_in.XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_EQUIPMENT_NAME)));
  ACE_ASSERT(equipment_dialog);

	// set window icon
  GtkWidget* main_dialog =
		GTK_WIDGET(glade_xml_get_widget(userData_in.XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_MAIN_NAME)));
  ACE_ASSERT(main_dialog);
	RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::IMAGE;
  type.image = IMAGE_WM_ICON;
  RPG_Graphics_t icon_graphic =
		RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->get(type);
  ACE_ASSERT(icon_graphic.type.image == IMAGE_WM_ICON);
  std::string path = RPG_Graphics_Common_Tools::getGraphicsDirectory();
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_TILE_IMAGES_SUB);
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += icon_graphic.file;
  GtkWidget* image_icon = gtk_image_new_from_file(path.c_str());
  ACE_ASSERT(image_icon);
  gtk_window_set_icon(GTK_WINDOW(main_dialog),
                      gtk_image_get_pixbuf(GTK_IMAGE(image_icon)));
  //GdkWindow* main_dialog_window = gtk_widget_get_window(main_dialog);
  //gtk_window_set_title(,
  //                     caption.c_str());

  GtkWidget* main_entry_dialog =
		GTK_WIDGET(glade_xml_get_widget(userData_in.XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_ENTRY_NAME)));
  ACE_ASSERT(main_entry_dialog);
  GtkEntry* entry =
		GTK_ENTRY(glade_xml_get_widget(userData_in.XML,
																	 ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_ENTRY_NAME)));
  ACE_ASSERT(entry);
  GtkEntryBuffer* entry_buffer =
    gtk_entry_buffer_new(ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_DEF_NAME), // text
                         -1);                                             // length in bytes (-1: \0-terminated)
  ACE_ASSERT(entry_buffer);
  if (!entry_buffer)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_entry_buffer_new(): \"%m\", aborting\n")));

    // clean up
    g_object_unref(G_OBJECT(userData_in.savedstate_filter));
    g_object_unref(G_OBJECT(userData_in.entity_filter));
    g_object_unref(G_OBJECT(userData_in.map_filter));
    g_object_unref(G_OBJECT(userData_in.XML));

    return false;
  } // end IF
  gtk_entry_set_buffer(entry, entry_buffer);
//   g_object_unref(G_OBJECT(entry_buffer));

  // step3: populate comboboxes
  // step3a: character repository
  GtkComboBox* combobox =
		GTK_COMBO_BOX(glade_xml_get_widget(userData_in.XML,
																			 ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
  ACE_ASSERT(combobox);
  gtk_cell_layout_clear(GTK_CELL_LAYOUT(combobox));
  GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
  if (!renderer)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_cell_renderer_text_new(): \"%m\", aborting\n")));

    // clean up
    g_object_unref(G_OBJECT(userData_in.savedstate_filter));
    g_object_unref(G_OBJECT(userData_in.entity_filter));
    g_object_unref(G_OBJECT(userData_in.map_filter));
    g_object_unref(G_OBJECT(userData_in.XML));

    return false;
  } // end IF
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combobox), renderer,
                             TRUE); // expand ?
//   gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(repository_combobox), renderer,
//                                 ACE_TEXT_ALWAYS_CHAR("text"), 0);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combobox), renderer,
                                 ACE_TEXT_ALWAYS_CHAR("text"), 0,
                                 NULL);
  GtkListStore* list = gtk_list_store_new(1,
                                          G_TYPE_STRING);
  if (!list)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_list_store_new(): \"%m\", aborting\n")));

    // clean up
    g_object_unref(G_OBJECT(userData_in.savedstate_filter));
    g_object_unref(G_OBJECT(userData_in.entity_filter));
    g_object_unref(G_OBJECT(userData_in.map_filter));
    g_object_unref(G_OBJECT(userData_in.XML));

    return false;
  } // end IF
  gtk_combo_box_set_model(combobox,
                          GTK_TREE_MODEL(list));
  g_object_unref(G_OBJECT(list));
  if (::load_files(REPOSITORY_PROFILES,
                   list))
    gtk_widget_set_sensitive(GTK_WIDGET(combobox),
                             TRUE);

  // step3b: maps repository
  combobox =
      GTK_COMBO_BOX(glade_xml_get_widget(userData_in.XML,
                                         ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_MAP_NAME)));
  ACE_ASSERT(combobox);
  gtk_cell_layout_clear(GTK_CELL_LAYOUT(combobox));
  renderer = gtk_cell_renderer_text_new();
  if (!renderer)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_cell_renderer_text_new(): \"%m\", aborting\n")));

    // clean up
    g_object_unref(G_OBJECT(userData_in.savedstate_filter));
    g_object_unref(G_OBJECT(userData_in.entity_filter));
    g_object_unref(G_OBJECT(userData_in.map_filter));
    g_object_unref(G_OBJECT(userData_in.XML));

    return false;
  } // end IF
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combobox), renderer,
                             TRUE); // expand ?
//   gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(combobox), renderer,
//                                 ACE_TEXT_ALWAYS_CHAR("text"), 0);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combobox), renderer,
                                 ACE_TEXT_ALWAYS_CHAR("text"), 0,
                                 NULL);
  list = gtk_list_store_new(1,
                            G_TYPE_STRING);
  if (!list)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_list_store_new(): \"%m\", aborting\n")));

    // clean up
    g_object_unref(G_OBJECT(userData_in.savedstate_filter));
    g_object_unref(G_OBJECT(userData_in.entity_filter));
    g_object_unref(G_OBJECT(userData_in.map_filter));
    g_object_unref(G_OBJECT(userData_in.XML));

    return false;
  } // end IF
  gtk_combo_box_set_model(combobox,
                          GTK_TREE_MODEL(list));
  g_object_unref(G_OBJECT(list));
  if (::load_files(REPOSITORY_MAPS,
                   list))
    gtk_widget_set_sensitive(GTK_WIDGET(combobox),
                             TRUE);

  // step3c: savedstate repository
  combobox =
      GTK_COMBO_BOX(glade_xml_get_widget(userData_in.XML,
                                         ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_ENGINESTATE_NAME)));
  ACE_ASSERT(combobox);
  gtk_cell_layout_clear(GTK_CELL_LAYOUT(combobox));
  renderer = gtk_cell_renderer_text_new();
  if (!renderer)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_cell_renderer_text_new(): \"%m\", aborting\n")));

    // clean up
    g_object_unref(G_OBJECT(userData_in.savedstate_filter));
    g_object_unref(G_OBJECT(userData_in.entity_filter));
    g_object_unref(G_OBJECT(userData_in.map_filter));
    g_object_unref(G_OBJECT(userData_in.XML));

    return false;
  } // end IF
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combobox), renderer,
                             TRUE); // expand ?
//   gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(combobox), renderer,
//                                 ACE_TEXT_ALWAYS_CHAR("text"), 0);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combobox), renderer,
                                 ACE_TEXT_ALWAYS_CHAR("text"), 0,
                                 NULL);
  list = gtk_list_store_new(1,
                            G_TYPE_STRING);
  if (!list)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_list_store_new(): \"%m\", aborting\n")));

    // clean up
    g_object_unref(G_OBJECT(userData_in.savedstate_filter));
    g_object_unref(G_OBJECT(userData_in.entity_filter));
    g_object_unref(G_OBJECT(userData_in.map_filter));
    g_object_unref(G_OBJECT(userData_in.XML));

    return false;
  } // end IF
	gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(list),
																	0,
																	(GtkTreeIterCompareFunc)combobox_sort_function,
																	GINT_TO_POINTER(0),
																	(GDestroyNotify)NULL);
	gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(list),
																			 0,
																			 GTK_SORT_ASCENDING);
  gtk_combo_box_set_model(combobox,
                          GTK_TREE_MODEL(list));
  g_object_unref(G_OBJECT(list));
  if (::load_files(REPOSITORY_ENGINESTATE,
                   list))
    gtk_widget_set_sensitive(GTK_WIDGET(combobox),
                             TRUE);

	GtkHBox* hbox =
		GTK_HBOX(glade_xml_get_widget(userData_in.XML,
		                              ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_HBOX_MAIN_NAME)));
	ACE_ASSERT(hbox);
	GtkFrame* frame =
		GTK_FRAME(glade_xml_get_widget(userData_in.XML,
		                               ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_FRAME_CHARACTER_NAME)));
	ACE_ASSERT(frame);
	gtk_widget_ref(GTK_WIDGET(frame));
	gtk_widget_unparent(GTK_WIDGET(frame));
	gtk_box_pack_start(GTK_BOX(hbox),
										 GTK_WIDGET(frame),
										 FALSE, // expand
										 FALSE, // fill
										 0);    // padding

  // step4: (auto-)connect signals/slots
	// *NOTE*: glade_xml_signal_autoconnect doesn't work reliably
	//glade_xml_signal_autoconnect(xml);
  // step4a: connect default signals
	// *NOTE*: glade_xml_signal_connect_data doesn't work reliably
	//   glade_xml_signal_connect_data(xml,
//                                 ACE_TEXT_ALWAYS_CHAR("properties_activated_GTK_cb"),
//                                 G_CALLBACK(properties_activated_GTK_cb),
//                                 userData_p);
  g_signal_connect(filechooser_dialog,
                   ACE_TEXT_ALWAYS_CHAR("response"),
                   G_CALLBACK(gtk_widget_hide),
                   &filechooser_dialog);
  g_signal_connect(about_dialog,
                   ACE_TEXT_ALWAYS_CHAR("response"),
                   G_CALLBACK(gtk_widget_hide),
                   &about_dialog);
  g_signal_connect(equipment_dialog,
                   ACE_TEXT_ALWAYS_CHAR("response"),
                   G_CALLBACK(gtk_widget_hide),
                   &equipment_dialog);
  g_signal_connect(main_entry_dialog,
                   ACE_TEXT_ALWAYS_CHAR("response"),
                   G_CALLBACK(gtk_widget_hide),
                   &main_entry_dialog);
  gpointer userData_p = const_cast<RPG_Client_GTK_CBData_t*>(&userData_in);
  g_signal_connect(main_dialog,
                   ACE_TEXT_ALWAYS_CHAR("destroy"),
                   G_CALLBACK(quit_clicked_GTK_cb),
                   //                    G_CALLBACK(gtk_widget_destroyed),
                   //                    &main_dialog,
                   userData_p);

   // step4b: connect custom signals
  GtkButton* button = NULL;
  button = GTK_BUTTON(glade_xml_get_widget(userData_in.XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_CREATE_NAME)));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(create_character_clicked_GTK_cb),
                   userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(userData_in.XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_DROP_NAME)));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(drop_character_clicked_GTK_cb),
                   userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(userData_in.XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_LOAD_NAME)));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(load_character_clicked_GTK_cb),
                   userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(userData_in.XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_STORE_NAME)));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(save_character_clicked_GTK_cb),
                   userData_p);

  combobox =
		GTK_COMBO_BOX(glade_xml_get_widget(userData_in.XML,
																			 ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
  ACE_ASSERT(combobox);
  g_signal_connect(combobox,
                   ACE_TEXT_ALWAYS_CHAR("changed"),
                   G_CALLBACK(character_repository_combobox_changed_GTK_cb),
                   userData_p);

  button =
		GTK_BUTTON(glade_xml_get_widget(userData_in.XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_REFRESH_NAME)));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(character_repository_button_clicked_GTK_cb),
                   userData_p);

  button =
		GTK_BUTTON(glade_xml_get_widget(userData_in.XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_CREATE_NAME)));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(create_map_clicked_GTK_cb),
                   userData_p);

  button =
		GTK_BUTTON(glade_xml_get_widget(userData_in.XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_DROP_NAME)));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(drop_map_clicked_GTK_cb),
                   userData_p);

  button =
		GTK_BUTTON(glade_xml_get_widget(userData_in.XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_LOAD_NAME)));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(load_map_clicked_GTK_cb),
                   userData_p);

  button =
		GTK_BUTTON(glade_xml_get_widget(userData_in.XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_STORE_NAME)));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(save_map_clicked_GTK_cb),
                   userData_p);

  combobox =
		GTK_COMBO_BOX(glade_xml_get_widget(userData_in.XML,
																			 ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_MAP_NAME)));
  ACE_ASSERT(combobox);
  g_signal_connect(combobox,
                   ACE_TEXT_ALWAYS_CHAR("changed"),
                   G_CALLBACK(map_repository_combobox_changed_GTK_cb),
                   userData_p);

  button =
		GTK_BUTTON(glade_xml_get_widget(userData_in.XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_REFRESH_NAME)));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(map_repository_button_clicked_GTK_cb),
                   userData_p);

	button =
		GTK_BUTTON(glade_xml_get_widget(userData_in.XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_ENGINESTATE_LOAD_NAME)));
	ACE_ASSERT(button);
	g_signal_connect(button,
									 ACE_TEXT_ALWAYS_CHAR("clicked"),
									 G_CALLBACK(load_state_clicked_GTK_cb),
									 userData_p);

	button =
		GTK_BUTTON(glade_xml_get_widget(userData_in.XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_ENGINESTATE_STORE_NAME)));
	ACE_ASSERT(button);
	g_signal_connect(button,
									 ACE_TEXT_ALWAYS_CHAR("clicked"),
									 G_CALLBACK(save_state_clicked_GTK_cb),
									 userData_p);

	combobox =
		GTK_COMBO_BOX(glade_xml_get_widget(userData_in.XML,
																			 ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_ENGINESTATE_NAME)));
	ACE_ASSERT(combobox);
	g_signal_connect(combobox,
									 ACE_TEXT_ALWAYS_CHAR("changed"),
									 G_CALLBACK(state_repository_combobox_changed_GTK_cb),
									 userData_p);

	button =
		GTK_BUTTON(glade_xml_get_widget(userData_in.XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_ENGINESTATE_REFRESH_NAME)));
	ACE_ASSERT(button);
	g_signal_connect(button,
									 ACE_TEXT_ALWAYS_CHAR("clicked"),
									 G_CALLBACK(state_repository_button_clicked_GTK_cb),
									 userData_p);

  button =
		GTK_BUTTON(glade_xml_get_widget(userData_in.XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_SERVER_JOIN_NAME)));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(join_game_clicked_GTK_cb),
                   userData_p);

  button =
		GTK_BUTTON(glade_xml_get_widget(userData_in.XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_SERVER_PART_NAME)));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(part_game_clicked_GTK_cb),
                   userData_p);

  button =
		GTK_BUTTON(glade_xml_get_widget(userData_in.XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_EQUIP_NAME)));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(equip_clicked_GTK_cb),
                   userData_p);

  button =
		GTK_BUTTON(glade_xml_get_widget(userData_in.XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_REST_NAME)));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(rest_clicked_GTK_cb),
                   userData_p);

  combobox =
		GTK_COMBO_BOX(glade_xml_get_widget(userData_in.XML,
																			 ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_SERVER_NAME)));
  ACE_ASSERT(combobox);
  g_signal_connect(combobox,
                   ACE_TEXT_ALWAYS_CHAR("changed"),
                   G_CALLBACK(server_repository_combobox_changed_GTK_cb),
                   userData_p);

  button =
		GTK_BUTTON(glade_xml_get_widget(userData_in.XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_SERVER_REFRESH_NAME)));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(server_repository_button_clicked_GTK_cb),
                   userData_p);

  button =
		GTK_BUTTON(glade_xml_get_widget(userData_in.XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_ABOUT_NAME)));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(about_clicked_GTK_cb),
                   userData_p);

  button =
		GTK_BUTTON(glade_xml_get_widget(userData_in.XML,
																		ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_QUIT_NAME)));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(quit_clicked_GTK_cb),
                   userData_p);

//   // step5: use correct screen
//   if (parentWidget_in)
//     gtk_window_set_screen(GTK_WINDOW(dialog),
//                           gtk_widget_get_screen(const_cast<GtkWidget*>(//parentWidget_in)));

  // step6: draw main dialog
  gtk_widget_show_all(main_dialog);

	// step7: schedule UI initialization
	guint event_source_id = g_idle_add(idle_init_UI_cb,
                                     userData_p);
  if (event_source_id == 0)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to g_idle_add(): \"%m\", aborting\n")));

    return false;
  } // end ELSE

  return true;
}

void
fini_UI_client(RPG_Client_GTK_CBData_t& userData_in)
{
	RPG_TRACE(ACE_TEXT("::fini_UI_client"));

	// schedule UI finalization
	gpointer userData_p = const_cast<RPG_Client_GTK_CBData_t*>(&userData_in);
	guint event_source_id = g_idle_add(idle_fini_UI_cb,
																		 userData_p);
	if (event_source_id == 0)
		ACE_DEBUG((LM_ERROR,
							 ACE_TEXT("failed to g_idle_add(): \"%m\", continuing\n")));
}

RPG_Client_GTKUIDefinition::RPG_Client_GTKUIDefinition(RPG_Client_GTK_CBData_t* GTKCBData_in)
: myGTKCBData(GTKCBData_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_GTKUIDefinition::RPG_Client_GTKUIDefinition"));

	ACE_ASSERT(myGTKCBData);
}

RPG_Client_GTKUIDefinition::~RPG_Client_GTKUIDefinition()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_GTKUIDefinition::~RPG_Client_GTKUIDefinition"));

}

bool
RPG_Client_GTKUIDefinition::init(const std::string& filename_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_GTKUIDefinition::init"));

	return init_UI_client(filename_in,
			                  *myGTKCBData);
}

void
RPG_Client_GTKUIDefinition::fini()
{
	RPG_TRACE(ACE_TEXT("RPG_Client_GTKUIDefinition::fini"));

	fini_UI_client(*myGTKCBData);
}
