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

#include "rpg_client_callbacks.h"

#include "rpg_client_defines.h"
#include "rpg_client_common.h"
#include "rpg_client_engine.h"

#include <rpg_engine_common.h>
#include <rpg_engine_common_tools.h>

#include <rpg_graphics_defines.h>
#include <rpg_graphics_surface.h>
#include <rpg_graphics_cursor.h>
#include <rpg_graphics_dictionary.h>
#include <rpg_graphics_common_tools.h>

#include <rpg_map_defines.h>
#include <rpg_map_common_tools.h>

#include <rpg_character_defines.h>
#include <rpg_character_class_common_tools.h>

#include <rpg_item_instance_manager.h>
#include <rpg_item_weapon.h>
#include <rpg_item_armor.h>

#include <rpg_common_macros.h>
#include <rpg_common_defines.h>
#include <rpg_common_tools.h>
#include <rpg_common_file_tools.h>

#include <gmodule.h>

#include <ace/Reactor.h>
#include <ace/Dirent_Selector.h>
#include <ace/Log_Msg.h>

#include <sstream>

void
update_character_profile(const RPG_Character_Player& player_in,
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
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
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
        switch (static_cast<RPG_Character_Race> (race_index))
        {
          case RPG_CHARACTER_RACE_MAX:
          case RPG_CHARACTER_RACE_INVALID:
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("invalid race (was: \"%s\"), aborting\n"),
                       RPG_Character_RaceHelper::RPG_Character_RaceToString(static_cast<RPG_Character_Race> (race_index)).c_str()));

            break;
          }
          default:
          {
            text += RPG_Common_Tools::enumToString(RPG_Character_RaceHelper::RPG_Character_RaceToString(static_cast<RPG_Character_Race> (race_index)));
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
    {
      text = RPG_Common_Tools::enumToString(RPG_Character_MetaClassHelper::RPG_Character_MetaClassToString(player_class.metaClass));

      break;
    }
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
      {
        text += RPG_Common_Tools::enumToString(RPG_Common_SubClassHelper::RPG_Common_SubClassToString(*iterator));

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
      {
        text += RPG_Common_Tools::enumToString(RPG_Character_AlignmentCivicHelper::RPG_Character_AlignmentCivicToString(player_alignment.civic));

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
                   RPG_Character_AlignmentEthicHelper::RPG_Character_AlignmentEthicToString(player_alignment.ethic).c_str()));

        break;
      }
      default:
      {
        text += RPG_Common_Tools::enumToString(RPG_Character_AlignmentEthicHelper::RPG_Character_AlignmentEthicToString(player_alignment.ethic));

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
  unsigned short int total_hp = player_in.getNumTotalHitPoints();
  short int hp = player_in.getNumHitPoints();
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
  converter << hp;
  converter << ACE_TEXT_ALWAYS_CHAR(" / ");
  converter << total_hp;
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("hitpoints")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());

  // step7: AC
  signed char armor_class_normal = player_in.getArmorClass(DEFENSE_NORMAL);
  signed char armor_class_touch = player_in.getArmorClass(DEFENSE_TOUCH);
  signed char armor_class_flatfooted = player_in.getArmorClass(DEFENSE_FLATFOOTED);
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
  converter << static_cast<int> (armor_class_normal);
  converter << ACE_TEXT_ALWAYS_CHAR(" / ");
  converter << static_cast<int> (armor_class_touch);
  converter << ACE_TEXT_ALWAYS_CHAR(" / ");
  converter << static_cast<int> (armor_class_flatfooted);
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("armorclass")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());

  // step8: XP
  unsigned int experience = player_in.getExperience();
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
  converter << experience;
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
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
  {
    converter << static_cast<unsigned int> (level);
  } // end IF
  else
  {
    for (RPG_Character_SubClassesIterator_t iterator = player_class.subClasses.begin();
         iterator != player_class.subClasses.end();
         iterator++)
    {
      level = player_in.getLevel(*iterator);
      converter << static_cast<unsigned int> (level);
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
  unsigned int gold = player_in.getWealth();
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
  converter << gold;
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("gold")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());

  // step11: condition
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
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
  GtkWidget* label = NULL;
  RPG_Character_Conditions_t player_condition = player_in.getCondition();
  for (RPG_Character_ConditionsIterator_t iterator = player_condition.begin();
       iterator != player_condition.end();
       iterator++)
  {
    text = RPG_Common_Tools::enumToString(RPG_Common_ConditionHelper::RPG_Common_ConditionToString(*iterator));
    label = NULL;
    label = gtk_label_new(text.c_str());
    ACE_ASSERT(label);
//     gtk_container_add(GTK_CONTAINER(current), label);
    gtk_box_pack_start(GTK_BOX(current), label,
                       TRUE, // expand
                       TRUE, // fill
                       0);   // padding
  } // end FOR
  gtk_widget_show_all(current);

  // step12: attributes
  unsigned int attribute = 0;
  attribute = player_in.getAttribute(ATTRIBUTE_STRENGTH);
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
  converter << attribute;
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("strength")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());
  attribute = player_in.getAttribute(ATTRIBUTE_DEXTERITY);
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
  converter << attribute;
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("dexterity")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());
  attribute = player_in.getAttribute(ATTRIBUTE_CONSTITUTION);
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
  converter << attribute;
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("constitution")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());
  attribute = player_in.getAttribute(ATTRIBUTE_INTELLIGENCE);
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
  converter << attribute;
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("intelligence")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());
  attribute = player_in.getAttribute(ATTRIBUTE_WISDOM);
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
  converter << attribute;
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("wisdom")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());
  attribute = player_in.getAttribute(ATTRIBUTE_CHARISMA);
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter.clear();
  converter << attribute;
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("charisma")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     converter.str().c_str());

  // step13: feats
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("feats_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
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
  label = NULL;
  RPG_Character_Feats_t player_feats = player_in.getFeats();
  for (RPG_Character_FeatsConstIterator_t iterator = player_feats.begin();
       iterator != player_feats.end();
       iterator++)
  {
    text = RPG_Common_Tools::enumToString(RPG_Character_FeatHelper::RPG_Character_FeatToString(*iterator));
    label = NULL;
    label = gtk_label_new(text.c_str());
    ACE_ASSERT(label);
//     gtk_container_add(GTK_CONTAINER(current), label);
    gtk_box_pack_start(GTK_BOX(current), label,
                       TRUE, // expand
                       TRUE, // fill
                       0);   // padding
  } // end FOR
  gtk_widget_show_all(current);

  // step14: abilities
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("abilities_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
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
  RPG_Character_Abilities_t player_abilities = player_in.getAbilities();
  for (RPG_Character_AbilitiesConstIterator_t iterator = player_abilities.begin();
       iterator != player_abilities.end();
       iterator++)
  {
    text = RPG_Common_Tools::enumToString(RPG_Character_AbilityHelper::RPG_Character_AbilityToString(*iterator));
    label = NULL;
    label = gtk_label_new(text.c_str());
    ACE_ASSERT(label);
//     gtk_container_add(GTK_CONTAINER(current), label);
    gtk_box_pack_start(GTK_BOX(current), label,
                       TRUE, // expand
                       TRUE, // fill
                       0);   // padding
  } // end FOR
  gtk_widget_show_all(current);

  // step15: skills
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("skills_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
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
  GtkWidget* current_box = NULL;
  RPG_Character_Skills_t player_skills = player_in.getSkills();
  for (RPG_Character_SkillsConstIterator_t iterator = player_skills.begin();
       iterator != player_skills.end();
       iterator++)
  {
    text = RPG_Common_Tools::enumToString(RPG_Common_SkillHelper::RPG_Common_SkillToString((*iterator).first));
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

    converter.str(ACE_TEXT(""));
    converter.clear();
    converter << static_cast<int>((*iterator).second);
    text = converter.str();
    label = NULL;
    label = gtk_label_new(text.c_str());
    ACE_ASSERT(label);
    gtk_box_pack_start(GTK_BOX(current_box), label,
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

  // step16: spells
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("spells_frame")));
  ACE_ASSERT(current);
  if (!RPG_Character_Class_Common_Tools::hasCasterClass(player_class))
    gtk_widget_hide_all(current);
  else
    gtk_widget_show_all(current);

  // divine casters know ALL spells from the levels they can cast...
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("known_spells_frame")));
  ACE_ASSERT(current);
  if (!RPG_Character_Class_Common_Tools::hasArcaneCasterClass(player_class))
    gtk_widget_hide_all(current);
  else
    gtk_widget_show_all(current);

  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("known_spells_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
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
  RPG_Magic_SpellTypes_t player_known_spells = player_in.getKnownSpells();
  for (RPG_Magic_SpellTypesIterator_t iterator = player_known_spells.begin();
       iterator != player_known_spells.end();
       iterator++)
  {
    text = RPG_Common_Tools::enumToString(RPG_Magic_SpellTypeHelper::RPG_Magic_SpellTypeToString(*iterator));

    label = NULL;
    label = gtk_label_new(text.c_str());
    ACE_ASSERT(label);
//     gtk_container_add(GTK_CONTAINER(current), label);
    gtk_box_pack_start(GTK_BOX(current), label,
                       TRUE, // expand
                       TRUE, // fill
                       0);   // padding
  } // end FOR
  gtk_widget_show_all(current);
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("prepared_spells_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
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
  RPG_Magic_Spells_t player_spells = player_in.getSpells();
  unsigned int count = 0;
  RPG_Magic_SpellTypes_t processsed_types;
  for (RPG_Magic_SpellsIterator_t iterator = player_spells.begin();
       iterator != player_spells.end();
       iterator++)
  {
    if (processsed_types.find(*iterator) != processsed_types.end())
      continue;

    // count instances
    count = 0;
    for (RPG_Magic_SpellsIterator_t iterator2 = player_spells.begin();
         iterator2 != player_spells.end();
         iterator2++)
      if (*iterator2 == *iterator)
        count++;

    text = RPG_Common_Tools::enumToString(RPG_Magic_SpellTypeHelper::RPG_Magic_SpellTypeToString(*iterator));
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

    converter.str(ACE_TEXT(""));
    converter.clear();
    converter << count;
    text = converter.str();
    label = NULL;
    label = gtk_label_new(text.c_str());
    ACE_ASSERT(label);
    gtk_box_pack_start(GTK_BOX(current_box), label,
                       TRUE, // expand
                       TRUE, // fill
                       0);   // padding

    //     gtk_container_add(GTK_CONTAINER(current), label);
    gtk_box_pack_start(GTK_BOX(current), current_box,
                       TRUE, // expand
                       TRUE, // fill
                       0);   // padding

    processsed_types.insert(*iterator);
  } // end FOR
  gtk_widget_show_all(current);

  // step17: inventory
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("inventory_items_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
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
  RPG_Character_Inventory player_inventory = player_in.getInventory();
//   RPG_Character_Equipment player_equipment = player_in.getEquipment();
  RPG_Item_Base* item = NULL;
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
      case ITEM_WEAPON:
      {
        RPG_Item_Weapon* weapon = dynamic_cast<RPG_Item_Weapon*>(item);
        ACE_ASSERT(weapon);

        // *TODO*: pretty-print this string
        text = RPG_Common_Tools::enumToString(RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString(weapon->getWeaponType()), false); // don't strip leading "xxx_"

        break;
      }
      case ITEM_ARMOR:
      {
        RPG_Item_Armor* armor = dynamic_cast<RPG_Item_Armor*>(item);
        ACE_ASSERT(armor);

        text = RPG_Common_Tools::enumToString(RPG_Item_ArmorTypeHelper::RPG_Item_ArmorTypeToString(armor->getArmorType()));

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
  gtk_widget_show_all(current);
}

void
update_entity_profile(const RPG_Engine_Entity& entity_in,
                      GladeXML* xml_in)
{
  RPG_TRACE(ACE_TEXT("::update_entity_profile"));

  // sanity checks
  ACE_ASSERT(entity_in.character);
  ACE_ASSERT(entity_in.character->isPlayerCharacter());
  RPG_Character_Player* player = dynamic_cast<RPG_Character_Player*>(entity_in.character);
  ACE_ASSERT(player);
  ACE_ASSERT(xml_in);

  // step1: update character profile
  ::update_character_profile(*player, xml_in);

  // step2: update image (if available)
  std::string filename;
  // retrieve image widget
  GtkImage* image = GTK_IMAGE(glade_xml_get_widget(xml_in,
                                                   ACE_TEXT_ALWAYS_CHAR("image_sprite")));
  ACE_ASSERT(image);
  if (entity_in.sprite != RPG_GRAPHICS_SPRITE_INVALID)
  {
    RPG_Graphics_GraphicTypeUnion type;
    type.discriminator = RPG_Graphics_GraphicTypeUnion::SPRITE;
    type.sprite = entity_in.sprite;
    RPG_Graphics_t graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->get(type);
    ACE_ASSERT((graphic.type.discriminator == type.discriminator) &&
               (graphic.type.sprite == type.sprite));
    // assemble path
    RPG_Graphics_Common_Tools::graphicToFile(graphic, filename);
  } // end IF
  if (!filename.empty())
    gtk_image_set_from_file(image, filename.c_str());
  else
    gtk_image_clear(image);
}

void
reset_entity_profile(GladeXML* xml_in)
{
  RPG_TRACE(ACE_TEXT("::reset_entity_profile"));

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
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("armorclass")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     NULL);

  // step1h: XP
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
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
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
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
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("constitution")));
  ACE_ASSERT(current);
  gtk_label_set_text(GTK_LABEL(current),
                     NULL);
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
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
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("feats_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
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

  // step1n: abilities
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("abilities_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
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

  // step1o: skills
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("skills_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
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

  // step1p: spells
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("spells_frame")));
  ACE_ASSERT(current);
  gtk_widget_show_all(current);
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("known_spells_frame")));
  ACE_ASSERT(current);
  gtk_widget_show_all(current);
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("known_spells_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
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
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("prepared_spells_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
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

  // step1q: inventory
  current = GTK_WIDGET(glade_xml_get_widget(xml_in,
                                            ACE_TEXT_ALWAYS_CHAR("inventory_items_vbox")));
  ACE_ASSERT(current);
  entries = gtk_container_get_children(GTK_CONTAINER(current));
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

  // step2: reset image (if available)
  GtkImage* image = GTK_IMAGE(glade_xml_get_widget(xml_in,
                                                   ACE_TEXT_ALWAYS_CHAR("image_sprite")));
  ACE_ASSERT(image);
  gtk_image_clear(image);
}

// callbacks used by ::scandir...
int
dirent_selector(const dirent* entry_in)
{
  RPG_TRACE(ACE_TEXT("::dirent_selector"));

  // *NOTE*: select *.xml files
  std::string filename(entry_in->d_name);
  std::string extension(RPG_CHARACTER_PROFILE_EXT);
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

int
dirent_comparator(const dirent** entry1_in,
                  const dirent** entry2_in)
{
  RPG_TRACE(ACE_TEXT("::dirent_comparator"));

  return ACE_OS::strcmp((*entry1_in)->d_name,
                        (*entry2_in)->d_name);
}

const unsigned long
load_profiles(const std::string& repository_in,
              GtkListStore* listStore_in)
//                         GtkComboBox* comboBox_in)
{
  RPG_TRACE(ACE_TEXT("::load_profiles"));

  unsigned long return_value = 0;

  // sanity check(s)
  ACE_ASSERT(listStore_in);
  if (!RPG_Common_File_Tools::isDirectory(repository_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to load_character_profiles(\"%s\"), not a directory, aborting\n"),
               repository_in.c_str()));

    return 0;
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

    return 0;
  } // end IF

  // clear existing entries
  // *WARNING* triggers the "changed" signal of the combobox...
  gtk_list_store_clear(listStore_in);

  // iterate over entries
  std::string entry;
  std::string extension(RPG_CHARACTER_PROFILE_EXT);
  GtkTreeIter iter;
  for (unsigned int i = 0;
       i < static_cast<unsigned int>(entries.length());
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

    return_value++;
  } // end FOR

  // clean up
  entries.close();

  // debug info
  GValue value;
  ACE_OS::memset(&value,
                  0,
                  sizeof(value));
  const gchar* text = NULL;
  if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(listStore_in),
                                     &iter))
    return 0;
  gtk_tree_model_get_value(GTK_TREE_MODEL(listStore_in), &iter,
                           0, &value);
  text = g_value_get_string(&value);
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("profile[0]: %s\n"),
             std::string(text).c_str()));
  g_value_unset(&value);
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
               ACE_TEXT("profile[%u]: %s\n"),
               i,
               std::string(text).c_str()));

    g_value_unset(&value);
  } // end FOR

  return return_value;
}

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT gint
about_activated_GTK_cb(GtkWidget* widget_in,
                       gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::about_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  RPG_Client_GTK_CBData_t* data = static_cast<RPG_Client_GTK_CBData_t*> (userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->xml);

  // retrieve about dialog handle
  GtkWidget* about_dialog = GTK_WIDGET(glade_xml_get_widget(data->xml,
                                                            RPG_CLIENT_DEF_GNOME_ABOUTDIALOG_NAME));
  ACE_ASSERT(about_dialog);
  if (!about_dialog)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
               RPG_CLIENT_DEF_GNOME_ABOUTDIALOG_NAME));

    return TRUE; // propagate
  } // end IF

  // draw it
  if (!GTK_WIDGET_VISIBLE(about_dialog))
    gtk_widget_show_all(about_dialog);

  return FALSE;
}

G_MODULE_EXPORT gint
properties_activated_GTK_cb(GtkWidget* widget_in,
                            gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::properties_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  ACE_UNUSED_ARG(userData_in);

  return FALSE;
}

G_MODULE_EXPORT gint
quit_activated_GTK_cb(GtkWidget* widget_in,
                      gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::quit_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
//   ACE_UNUSED_ARG(userData_in);
  RPG_Client_GTK_CBData_t* data = static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);

//   // leave GTK
//   gtk_main_quit();

  // synch access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(data->lock);

    ACE_ASSERT(!data->gtk_main_quit_invoked);
    data->gtk_main_quit_invoked = true;
  } // end lock scope

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("leaving GTK...\n")));

  // this is the "delete-event" handler
  // --> destroy the main dialog widget
  return TRUE; // --> propagate
}

G_MODULE_EXPORT gint
create_character_activated_GTK_cb(GtkWidget* widget_in,
                                  gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::create_character_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  //   ACE_UNUSED_ARG(userData_in);
  RPG_Client_GTK_CBData_t* data = static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);
  ACE_ASSERT(data->entity.character == NULL);

  data->entity = RPG_Engine_Common_Tools::createEntity(true);
  ACE_ASSERT(data->entity.character);

  // update entity profile widgets
  ::update_entity_profile(data->entity,
                          data->xml);

  // if necessary, update starting position
  if ((data->entity.position.first == 0) &&
      (data->entity.position.second == 0))
    data->entity.position = data->level_engine->getStartPosition();

  // make character display frame sensitive (if it's not already)
  GtkFrame* character_frame = GTK_FRAME(glade_xml_get_widget(data->xml,
                                                             RPG_CLIENT_DEF_GNOME_CHARFRAME_NAME));
  ACE_ASSERT(character_frame);
  gtk_widget_set_sensitive(GTK_WIDGET(character_frame), TRUE);

  // make drop button sensitive (if it's not already)
  GtkButton* button = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                                      ACE_TEXT_ALWAYS_CHAR("drop")));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make save button sensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                           ACE_TEXT_ALWAYS_CHAR("save")));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make load button insensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                           ACE_TEXT_ALWAYS_CHAR("load")));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make join button sensitive (if it's not already)
  GtkButton* join_game = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                                         ACE_TEXT_ALWAYS_CHAR("join")));
  ACE_ASSERT(join_game);
  gtk_widget_set_sensitive(GTK_WIDGET(join_game), TRUE);

  // make this insensitive
  gtk_widget_set_sensitive(widget_in, FALSE);

  return FALSE;
}

G_MODULE_EXPORT gint
drop_character_activated_GTK_cb(GtkWidget* widget_in,
                                gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::drop_character_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  //   ACE_UNUSED_ARG(userData_in);
  RPG_Client_GTK_CBData_t* data = static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);

  // clean up
  if (data->entity.character)
  {
    data->entity.actions.clear();
    delete data->entity.character;
    data->entity.character = NULL;
    SDL_FreeSurface(data->entity.graphic);
    data->entity.graphic = NULL;
    data->entity.position = std::make_pair(0, 0);
    data->entity.sprite = RPG_GRAPHICS_SPRITE_INVALID;
  } // end IF

  // reset profile widgets
 ::reset_entity_profile(data->xml);

  // make character display frame insensitive (if it's not already)
  GtkFrame* character_frame = GTK_FRAME(glade_xml_get_widget(data->xml,
                                                             RPG_CLIENT_DEF_GNOME_CHARFRAME_NAME));
  ACE_ASSERT(character_frame);
  gtk_widget_set_sensitive(GTK_WIDGET(character_frame), FALSE);

  // make create button sensitive (if it's not already)
  GtkButton* button = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                                      ACE_TEXT_ALWAYS_CHAR("create")));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make save button insensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                           ACE_TEXT_ALWAYS_CHAR("save")));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make load button sensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                           ACE_TEXT_ALWAYS_CHAR("load")));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make join button insensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                           ACE_TEXT_ALWAYS_CHAR("join")));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make this insensitive
  gtk_widget_set_sensitive(widget_in, FALSE);

  return FALSE;
}

G_MODULE_EXPORT gint
load_character_activated_GTK_cb(GtkWidget* widget_in,
                                gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::load_character_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  RPG_Client_GTK_CBData_t* data = static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->xml);

  // *NOTE*: this callback just presents the file picker
  // --> business logic happens in character_file_actived_GTK_cb

  // retrieve file chooser dialog handle
  GtkFileChooserDialog* filechooser_dialog = GTK_FILE_CHOOSER_DIALOG(glade_xml_get_widget(data->xml,
                                                                                          RPG_CLIENT_DEF_GNOME_FILECHOOSERDIALOG_NAME));
  ACE_ASSERT(filechooser_dialog);
  if (!filechooser_dialog)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
               RPG_CLIENT_DEF_GNOME_FILECHOOSERDIALOG_NAME));

    return FALSE;
  } // end IF

  // draw it
  if (!GTK_WIDGET_VISIBLE(filechooser_dialog))
    gtk_widget_show_all(GTK_WIDGET(filechooser_dialog));

  return FALSE;
}

G_MODULE_EXPORT gint
character_file_activated_GTK_cb(GtkWidget* widget_in,
                                gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::character_file_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  RPG_Client_GTK_CBData_t* data = static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->xml);
  ACE_ASSERT(data->level_engine);

  // retrieve file chooser dialog handle
  GtkFileChooserDialog* filechooser_dialog = GTK_FILE_CHOOSER_DIALOG(glade_xml_get_widget(data->xml,
                                                                                          RPG_CLIENT_DEF_GNOME_FILECHOOSERDIALOG_NAME));
  ACE_ASSERT(filechooser_dialog);

  // hide widget
  gtk_widget_hide(GTK_WIDGET(filechooser_dialog));

  // clean up
  if (data->entity.character)
  {
    data->entity.actions.clear();
    delete data->entity.character;
    data->entity.character = NULL;
    SDL_FreeSurface(data->entity.graphic);
    data->entity.graphic = NULL;
    data->entity.position = std::make_pair(0, 0);
    data->entity.sprite = RPG_GRAPHICS_SPRITE_INVALID;
  } // end IF

  // retrieve selected filename
  std::string filename(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooser_dialog)));

  // load player profile
  data->entity = RPG_Engine_Common_Tools::loadEntity(filename,
                                                             data->schemaRepository,
                                                             true);
  ACE_ASSERT(data->entity.character);

  // update entity profile widgets
  ::update_entity_profile(data->entity,
                          data->xml);

  // if necessary, update starting position
  if ((data->entity.position.first == 0) &&
      (data->entity.position.second == 0))
    data->entity.position = data->level_engine->getStartPosition();

  // make character display frame sensitive (if it's not already)
  GtkFrame* character_frame = GTK_FRAME(glade_xml_get_widget(data->xml,
                                                             RPG_CLIENT_DEF_GNOME_CHARFRAME_NAME));
  ACE_ASSERT(character_frame);
  gtk_widget_set_sensitive(GTK_WIDGET(character_frame), TRUE);

  // make create button insensitive (if it's not already)
  GtkButton* button = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                                      ACE_TEXT_ALWAYS_CHAR("create")));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make drop button sensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                           ACE_TEXT_ALWAYS_CHAR("drop")));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make save button insensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                           ACE_TEXT_ALWAYS_CHAR("save")));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make load button insensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                           ACE_TEXT_ALWAYS_CHAR("load")));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make join button sensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                           ACE_TEXT_ALWAYS_CHAR("join")));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  return FALSE;
}

G_MODULE_EXPORT gint
save_character_activated_GTK_cb(GtkWidget* widget_in,
                                gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::save_character_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  RPG_Client_GTK_CBData_t* data = static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->entity.character);

  // assemble target filename
  std::string filename = RPG_COMMON_DUMP_DIR;
  filename += ACE_DIRECTORY_SEPARATOR_STR;
  filename += data->entity.character->getName();
  filename += RPG_CHARACTER_PROFILE_EXT;

  if (!RPG_Engine_Common_Tools::saveEntity(data->entity,
                                           filename))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Engine_Common_Tools::saveEntity(\"%s\"), continuing\n"),
               filename.c_str()));

  // make save button INsensitive
  gtk_widget_set_sensitive(widget_in, FALSE);

  return FALSE;
}

G_MODULE_EXPORT gint
character_repository_combobox_changed_GTK_cb(GtkWidget* widget_in,
                                             gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::character_repository_combobox_changed_GTK_cb"));

  RPG_Client_GTK_CBData_t* data = static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(widget_in);
  ACE_ASSERT(data->xml);

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

  // clean up
  if (data->entity.character)
  {
    data->entity.actions.clear();
    delete data->entity.character;
    data->entity.character = NULL;
    SDL_FreeSurface(data->entity.graphic);
    data->entity.graphic = NULL;
    data->entity.position = std::make_pair(0, 0);
    data->entity.sprite = RPG_GRAPHICS_SPRITE_INVALID;
  } // end IF

  // construct filename
  std::string filename = RPG_CLIENT_DEF_CHARACTER_REPOSITORY;
  filename += ACE_DIRECTORY_SEPARATOR_STR;
  filename += active_item;
  filename += RPG_CHARACTER_PROFILE_EXT;

  // load player profile
  data->entity = RPG_Engine_Common_Tools::loadEntity(filename,
                                                             data->schemaRepository,
                                                             true);
  ACE_ASSERT(data->entity.character);

  // update entity profile widgets
  ::update_entity_profile(data->entity,
                          data->xml);

  // make character display frame sensitive (if it's not already)
  GtkFrame* character_frame = GTK_FRAME(glade_xml_get_widget(data->xml,
                                                             RPG_CLIENT_DEF_GNOME_CHARFRAME_NAME));
  ACE_ASSERT(character_frame);
  gtk_widget_set_sensitive(GTK_WIDGET(character_frame), TRUE);

  // make create button insensitive (if it's not already)
  GtkButton* button = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                                      ACE_TEXT_ALWAYS_CHAR("create")));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make drop button sensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                           ACE_TEXT_ALWAYS_CHAR("drop")));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make save button insensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                           ACE_TEXT_ALWAYS_CHAR("save")));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make load button insensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                           ACE_TEXT_ALWAYS_CHAR("load")));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // make join button sensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                           ACE_TEXT_ALWAYS_CHAR("join")));
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
  RPG_Client_GTK_CBData_t* data = static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->xml);

  // retrieve tree model
  GtkComboBox* repository_combobox = GTK_COMBO_BOX(glade_xml_get_widget(data->xml,
                                                                        RPG_CLIENT_DEF_GNOME_CHARBOX_NAME));
  ACE_ASSERT(repository_combobox);
  GtkTreeModel* model = gtk_combo_box_get_model(repository_combobox);
  ACE_ASSERT(model);

  // re-load profile data
  unsigned long num_entries = ::load_profiles(RPG_CLIENT_DEF_CHARACTER_REPOSITORY,
                                              GTK_LIST_STORE(model));

  // set sensitive as appropriate
  GtkFrame* character_frame = GTK_FRAME(glade_xml_get_widget(data->xml,
                                                             RPG_CLIENT_DEF_GNOME_CHARFRAME_NAME));
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
    GtkButton* button = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                                        ACE_TEXT_ALWAYS_CHAR("create")));
    ACE_ASSERT(button);
    gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

    // make load button sensitive (if it's not already)
    button = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                             ACE_TEXT_ALWAYS_CHAR("load")));
    ACE_ASSERT(button);
    gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
  } // end ELSE

  return FALSE;
}

G_MODULE_EXPORT gint
join_game_activated_GTK_cb(GtkWidget* widget_in,
                           gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::join_game_activated_GTK_cb"));

  ACE_ASSERT(widget_in);
  RPG_Client_GTK_CBData_t* data = static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->client_engine);
  ACE_ASSERT(data->entity.character);
  ACE_ASSERT(data->level_engine);

  // set start position, if necessary
  if ((data->entity.position.first == 0) &&
      (data->entity.position.second == 0))
    data->entity.position = data->level_engine->getStartPosition();

  // activate the current character
  RPG_Engine_EntityID_t id = data->level_engine->add(&(data->entity));
  data->client_engine->setPlayer(id);

  // make join button INsensitive
  gtk_widget_set_sensitive(widget_in, FALSE);

  // make part button sensitive
  GtkButton* part_game = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                                         ACE_TEXT_ALWAYS_CHAR("part")));
  ACE_ASSERT(part_game);
  gtk_widget_set_sensitive(GTK_WIDGET(part_game), TRUE);

  // minimize dialog window
  GdkWindow* toplevel = gtk_widget_get_parent_window(widget_in);
  ACE_ASSERT(toplevel);
  gdk_window_iconify(toplevel);

  return FALSE;
}

G_MODULE_EXPORT gint
part_game_activated_GTK_cb(GtkWidget* widget_in,
                           gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::part_game_activated_GTK_cb"));

  ACE_ASSERT(widget_in);
  RPG_Client_GTK_CBData_t* data = static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->client_engine);
  ACE_ASSERT(data->level_engine);

  // activate the current character
  RPG_Engine_EntityID_t id = data->client_engine->getPlayer();
  data->level_engine->remove(id);
  data->client_engine->setPlayer(0);

  // make join button INsensitive
  gtk_widget_set_sensitive(widget_in, FALSE);

  // make join button sensitive
  GtkButton* join_game = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                                         ACE_TEXT_ALWAYS_CHAR("join")));
  ACE_ASSERT(join_game);
  gtk_widget_set_sensitive(GTK_WIDGET(join_game), TRUE);

  // make save button sensitive
  GtkButton* save = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                                    ACE_TEXT_ALWAYS_CHAR("save")));
  ACE_ASSERT(save);
  gtk_widget_set_sensitive(GTK_WIDGET(save), TRUE);

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
//   ACE_ASSERT(data->xml);
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
//   filename += ACE_DIRECTORY_SEPARATOR_STR;
//   filename += active_item;
//   filename += RPG_CHARACTER_PROFILE_EXT;
//
//   // load player profile
//   data->entity = RPG_Engine_Common_Tools::loadEntity(filename,
//                                                              data->schemaRepository,
//                                                              true);
//   ACE_ASSERT(data->entity.character);
//
//   // update entity profile widgets
//   ::update_entity_profile(data->entity,
//                           data->xml);
//
//   // make character display frame sensitive (if it's not already)
//   GtkFrame* character_frame = GTK_FRAME(glade_xml_get_widget(data->xml,
//                                                              RPG_CLIENT_DEF_GNOME_CHARFRAME_NAME));
//   ACE_ASSERT(character_frame);
//   gtk_widget_set_sensitive(GTK_WIDGET(character_frame), TRUE);
//
//   // make join button sensitive (if it's not already)
//   GtkButton* join_game = GTK_BUTTON(glade_xml_get_widget(data->xml,
//                                                          ACE_TEXT_ALWAYS_CHAR("join")));
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
//   ACE_ASSERT(data->xml);
//
//   // retrieve tree model
//   GtkComboBox* repository_combobox = GTK_COMBO_BOX(glade_xml_get_widget(data->xml,
//                                                                          RPG_CLIENT_DEF_GNOME_CHARBOX_NAME));
//   ACE_ASSERT(repository_combobox);
//   GtkTreeModel* model = gtk_combo_box_get_model(repository_combobox);
//   ACE_ASSERT(model);
//
//   // re-load profile data
//   ::load_profiles(RPG_CLIENT_DEF_CHARACTER_REPOSITORY,
//                   GTK_LIST_STORE(model));
//
//   // set sensitive as appropriate
//   GtkFrame* character_frame = GTK_FRAME(glade_xml_get_widget(data->xml,
//                                                              RPG_CLIENT_DEF_GNOME_CHARFRAME_NAME));
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
//             dump_path += ACE_DIRECTORY_SEPARATOR_STR;
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
//   RPG_Client_GTK_CBData_t* data = static_cast<RPG_Client_GTK_CBData_t*> (userData_in);
//   ACE_ASSERT(data);
//
//   // synch access
//   {
//     ACE_Guard<ACE_Thread_Mutex> aGuard(data->hover_quit_lock);
//
//     ACE_ASSERT(!data->gtk_main_quit_invoked);
//     data->gtk_main_quit_invoked = true;
//   } // end lock scope
//
//   // de-register this hook
//   return FALSE;
// }

#ifdef __cplusplus
}
#endif /* __cplusplus */
