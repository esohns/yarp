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

#include "rpg_client_gui_callbacks.h"

#include <algorithm>
#include <sstream>

#include "gmodule.h"

#include "ace/Dirent_Selector.h"
#include "ace/Log_Msg.h"

#include "common_file_tools.h"

#include "common_ui_gtk_tools.h"

#include "rpg_common_defines.h"
#include "rpg_common_macros.h"
#include "rpg_common_tools.h"

#include "rpg_dice.h"
#include "rpg_dice_common.h"
#include "rpg_dice_common_tools.h"

#include "rpg_magic_common_tools.h"

#include "rpg_item_armor.h"
#include "rpg_item_commodity.h"
#include "rpg_item_common.h"
#include "rpg_item_dictionary.h"
#include "rpg_item_instance_manager.h"
#include "rpg_item_weapon.h"

#include "rpg_character_defines.h"

#include "rpg_player_common_tools.h"
#include "rpg_player_defines.h"

#include "rpg_map_common_tools.h"
#include "rpg_map_defines.h"

#include "rpg_engine_common.h"
#include "rpg_engine_common_tools.h"
#include "rpg_engine_defines.h"

#include "rpg_sound_event.h"
#include "rpg_sound_event_manager.h"

#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_cursor.h"
#include "rpg_graphics_defines.h"
#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_surface.h"

#include "rpg_client_common.h"
#include "rpg_client_common_tools.h"
#include "rpg_client_defines.h"
#include "rpg_client_engine.h"

// global variables
bool untoggling_server_join_button = false;
// bool setting_up_levelup_attributes = false;
struct RPG_Dice_Roll current_dice_roll;
std::string current_path;

void
update_equipment (const struct RPG_Client_GTK_CBData& data_in)
{
  RPG_TRACE (ACE_TEXT ("::update_equipment"));

  // sanity check(s)
  ACE_ASSERT (data_in.UIState);
  ACE_ASSERT (data_in.entity.character);
  ACE_ASSERT (data_in.entity.character->isPlayerCharacter ());
  RPG_Player* player = static_cast<RPG_Player*> (data_in.entity.character);
  ACE_ASSERT (player);
  Common_UI_GTK_BuildersConstIterator_t iterator =
    data_in.UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_in.UIState->builders.end ());

  std::string text;
  std::stringstream converter;
#if defined (GTK3_USE)
  GtkGrid* current = NULL;
#else
  GtkTable* current = NULL;
#endif // GTK3_USE
  // step1: list equipment
  current =
#if defined (GTK3_USE)
    GTK_GRID (gtk_builder_get_object ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_TABLE_EQUIPENT_NAME)));
#else
      GTK_TABLE (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_TABLE_EQUIPENT_NAME)));
#endif // GTK3_USE
  ACE_ASSERT (current);
  GList* entries = gtk_container_get_children (GTK_CONTAINER (current));
  if (entries)
  {
    for (GList* iterator_2 = entries;
         iterator_2;
         iterator_2 = g_list_next (iterator_2))
    {
      // *NOTE*: effectively removes the widget from the container...
      gtk_widget_destroy (GTK_WIDGET (iterator_2->data));
    } // end FOR
    g_list_free (entries);
  } // end IF
  GtkWidget* check_button, *label = NULL;
  RPG_Player_Equipment& player_equipment = player->getEquipment ();
  RPG_Item_Base* item = NULL;
  RPG_Character_EquipmentSlot equipment_slot =
    RPG_CHARACTER_EQUIPMENTSLOT_INVALID;
  const RPG_Player_Inventory& player_inventory = player->getInventory ();
  std::string widget_name;
  int i = 0;
  for (RPG_Item_ListIterator_t iterator_2 = player_inventory.myItems.begin ();
       iterator_2 != player_inventory.myItems.end ();
       iterator_2++, ++i)
  {
    item = NULL;
    // retrieve item handle
    if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->get (*iterator_2,
                                                                item))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid item (ID: %d), returning\n"),
                  *iterator_2));
      return;
    } // end IF
    ACE_ASSERT (item);
    switch (item->type ())
    {
      case ITEM_ARMOR:
      {
        RPG_Item_Armor* armor = dynamic_cast<RPG_Item_Armor*> (item);
        ACE_ASSERT (armor);

        text =
          RPG_Common_Tools::enumToString (RPG_Item_ArmorTypeHelper::RPG_Item_ArmorTypeToString (armor->armorType ()));

        break;
      }
      case ITEM_COMMODITY:
      {
        RPG_Item_Commodity* commodity = dynamic_cast<RPG_Item_Commodity*> (item);
        ACE_ASSERT (commodity);

        RPG_Item_CommodityUnion commodity_type = commodity->subtype ();
        switch (commodity_type.discriminator)
        {
          case RPG_Item_CommodityUnion::COMMODITYBEVERAGE:
          {
            text =
              RPG_Common_Tools::enumToString (RPG_Item_CommodityBeverageHelper::RPG_Item_CommodityBeverageToString (commodity_type.commoditybeverage));
            break;
          }
          case RPG_Item_CommodityUnion::COMMODITYLIGHT:
          {
            text =
              RPG_Common_Tools::enumToString (RPG_Item_CommodityLightHelper::RPG_Item_CommodityLightToString (commodity_type.commoditylight));
            break;
          }
          default:
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("invalid commodity subtype (was: %d), returning\n"),
                        commodity_type.discriminator));
            return;
          }
        } // end SWITCH

        break;
      }
      case ITEM_WEAPON:
      {
        RPG_Item_Weapon* weapon = dynamic_cast<RPG_Item_Weapon*> (item);
        ACE_ASSERT (weapon);

        // *TODO*: pretty-print this string
        text =
          RPG_Common_Tools::enumToString (RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString (weapon->weaponType_), false); // don't strip leading "xxx_"

        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid item type (was: \"%s\"), returning\n"),
                    ACE_TEXT (RPG_Item_TypeHelper::RPG_Item_TypeToString (item->type ()).c_str ())));
        return;
      }
    } // end SWITCH

    label = NULL;
    label = gtk_label_new (text.c_str ());
    ACE_ASSERT (label);
#if defined (GTK3_USE)
    gtk_grid_attach (current,
                     label,
                     0, i,
                     1,1);
#else
    gtk_table_attach_defaults (current,
                               label,
                               0, 1,
                               i, i + 1);
#endif // GTK3_USE
    check_button = NULL;
    check_button = gtk_check_button_new ();
    ACE_ASSERT (check_button);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_button),
                                  player_equipment.isEquipped (*iterator_2, equipment_slot));
    converter.str (ACE_TEXT (""));
    if (!(converter << *iterator_2))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to convert integer (was: %u), returning\n"),
                  *iterator_2));
      return;
    } // end IF
    widget_name = converter.str ();
    gtk_widget_set_name (check_button,
                         widget_name.c_str ());
    g_signal_connect (check_button,
                      ACE_TEXT_ALWAYS_CHAR ("toggled"),
                      G_CALLBACK (item_toggled_GTK_cb),
                      &const_cast<struct RPG_Client_GTK_CBData&> (data_in));
#if defined (GTK3_USE)
    gtk_grid_attach (current,
                     check_button,
                     1, i,
                     1, 1);
#else
    gtk_table_attach_defaults (current,
                               check_button,
                               1, 2,
                               i, i + 1);
#endif // GTK3_USE
  } // end FOR

  gtk_widget_show_all (GTK_WIDGET (current));
}

void
update_levelup (const struct RPG_Client_GTK_CBData& data_in)
{
  RPG_TRACE (ACE_TEXT ("::update_levelup"));

  // sanity check(s)
  ACE_ASSERT (data_in.UIState);
  ACE_ASSERT (data_in.entity.character);
  ACE_ASSERT (data_in.entity.character->isPlayerCharacter ());
  RPG_Player* player = static_cast<RPG_Player*> (data_in.entity.character);
  Common_UI_GTK_BuildersConstIterator_t iterator =
      data_in.UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_in.UIState->builders.end ());
  ACE_ASSERT (data_in.subClass != RPG_COMMON_SUBCLASS_INVALID);

  std::stringstream converter;
  GtkLabel* label_p = NULL;
  GtkSpinButton* spinbutton_p = NULL;
  GtkListStore* list_store_p = NULL;
  GtkTreeIter tree_iter;

  // step1: set (max) HP to current value
  label_p =
      GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_LABEL_HITPOINTS_NAME)));
  ACE_ASSERT (label_p);
  converter << player->getNumTotalHitPoints ();
  gtk_label_set_text (label_p,
                      converter.str ().c_str ());
  label_p =
      GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_LABEL_HITDICE_NAME)));
  ACE_ASSERT (label_p);
  current_dice_roll = player->getHitDicePerLevel (data_in.subClass);
  gtk_label_set_text (label_p,
                      RPG_Dice_Common_Tools::toString (current_dice_roll).c_str ());

  // step2: set attributes to current values
//  setting_up_levelup_attributes = true;
  spinbutton_p =
      GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_STRENGTH_NAME)));
  ACE_ASSERT (spinbutton_p);
  g_signal_handlers_block_by_func (G_OBJECT (spinbutton_p),
                                   (gpointer)spinbutton_attribute_value_changed_cb,
                                   (gpointer)&data_in);
  gtk_spin_button_set_value (spinbutton_p,
                             static_cast<gdouble> (player->getAttribute (ATTRIBUTE_STRENGTH)));
  g_signal_handlers_unblock_by_func (G_OBJECT (spinbutton_p),
                                     (gpointer)spinbutton_attribute_value_changed_cb,
                                     (gpointer)&data_in);
  spinbutton_p =
      GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_DEXTERITY_NAME)));
  ACE_ASSERT (spinbutton_p);
  g_signal_handlers_block_by_func (G_OBJECT (spinbutton_p),
                                   (gpointer)spinbutton_attribute_value_changed_cb,
                                   (gpointer)&data_in);
  gtk_spin_button_set_value (spinbutton_p,
                             static_cast<gdouble> (player->getAttribute (ATTRIBUTE_DEXTERITY)));
  g_signal_handlers_unblock_by_func (G_OBJECT (spinbutton_p),
                                     (gpointer)spinbutton_attribute_value_changed_cb,
                                     (gpointer)&data_in);
  spinbutton_p =
      GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_CONSTITUTION_NAME)));
  ACE_ASSERT (spinbutton_p);
  g_signal_handlers_block_by_func (G_OBJECT (spinbutton_p),
                                   (gpointer)spinbutton_attribute_value_changed_cb,
                                   (gpointer)&data_in);
  gtk_spin_button_set_value (spinbutton_p,
                             static_cast<gdouble> (player->getAttribute (ATTRIBUTE_CONSTITUTION)));
  g_signal_handlers_unblock_by_func (G_OBJECT (spinbutton_p),
                                     (gpointer)spinbutton_attribute_value_changed_cb,
                                     (gpointer)&data_in);
  spinbutton_p =
      GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_INTELLIGENCE_NAME)));
  ACE_ASSERT (spinbutton_p);
  g_signal_handlers_block_by_func (G_OBJECT (spinbutton_p),
                                   (gpointer)spinbutton_attribute_value_changed_cb,
                                   (gpointer)&data_in);
  gtk_spin_button_set_value (spinbutton_p,
                             static_cast<gdouble> (player->getAttribute (ATTRIBUTE_INTELLIGENCE)));
  g_signal_handlers_unblock_by_func (G_OBJECT (spinbutton_p),
                                     (gpointer)spinbutton_attribute_value_changed_cb,
                                     (gpointer)&data_in);
  spinbutton_p =
      GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_WISDOM_NAME)));
  ACE_ASSERT (spinbutton_p);
  g_signal_handlers_block_by_func (G_OBJECT (spinbutton_p),
                                   (gpointer)spinbutton_attribute_value_changed_cb,
                                   (gpointer)&data_in);
  gtk_spin_button_set_value (spinbutton_p,
                             static_cast<gdouble> (player->getAttribute (ATTRIBUTE_WISDOM)));
  g_signal_handlers_unblock_by_func (G_OBJECT (spinbutton_p),
                                     (gpointer)spinbutton_attribute_value_changed_cb,
                                     (gpointer)&data_in);
  spinbutton_p =
      GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_CHARISMA_NAME)));
  ACE_ASSERT (spinbutton_p);
  g_signal_handlers_block_by_func (G_OBJECT (spinbutton_p),
                                   (gpointer)spinbutton_attribute_value_changed_cb,
                                   (gpointer)&data_in);
  gtk_spin_button_set_value (spinbutton_p,
                             static_cast<gdouble> (player->getAttribute (ATTRIBUTE_CHARISMA)));
  g_signal_handlers_unblock_by_func (G_OBJECT (spinbutton_p),
                                     (gpointer)spinbutton_attribute_value_changed_cb,
                                     (gpointer)&data_in);
//  setting_up_levelup_attributes = false;

  label_p =
      GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_LABEL_POINTSREMAINING_NAME)));
  ACE_ASSERT (label_p);
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << RPG_CHARACTER_ATTRIBUTE_LEVELUP_POINTS;
  gtk_label_set_text (label_p,
                      converter.str ().c_str ());

  // step3: populate list of (new) feats
  list_store_p =
      GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_LISTSTORE_FEATS_NAME)));
  ACE_ASSERT (list_store_p);
  gtk_list_store_clear (list_store_p);
  const RPG_Character_Feats_t& feats_a = player->getFeats ();
  for (int i = FEAT_BLIND_FIGHT;
       i < RPG_CHARACTER_FEAT_MAX;
       ++i)
  {
    if (feats_a.find (static_cast<enum RPG_Character_Feat> (i)) != feats_a.end ())
      continue;
    gtk_list_store_append (list_store_p, &tree_iter);
    gtk_list_store_set (list_store_p, &tree_iter,
                        0, ACE_TEXT (RPG_Common_Tools::enumToString (RPG_Character_FeatHelper::RPG_Character_FeatToString (static_cast<enum RPG_Character_Feat> (i))).c_str ()), // column 0
                        1, i,
                        -1);
  } // end FOR

  label_p =
      GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                       ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_LABEL_FEATSREMAINING_NAME)));
  ACE_ASSERT (label_p);
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << static_cast<unsigned int> (player->getFeatsPerLevel (data_in.subClass));
  gtk_label_set_text (label_p,
                      converter.str ().c_str ());

  // step4: populate list of (new) skills
  list_store_p =
      GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_LISTSTORE_SKILLS_NAME)));
  ACE_ASSERT (list_store_p);
  gtk_list_store_clear (list_store_p);
  const RPG_Character_Skills_t& skills_a = player->getSkills ();
  RPG_Character_SkillsConstIterator_t iterator_2;
  for (int i = SKILL_APPRAISE;
       i < RPG_COMMON_SKILL_MAX;
       ++i)
  {
    iterator_2 = skills_a.find (static_cast<enum RPG_Common_Skill> (i));
    gtk_list_store_append (list_store_p, &tree_iter);
    gtk_list_store_set (list_store_p, &tree_iter,
                        0, ACE_TEXT (RPG_Common_Tools::enumToString (RPG_Common_SkillHelper::RPG_Common_SkillToString (static_cast<enum RPG_Common_Skill> (i))).c_str ()), // column 0
                        1, i,
                        2, (iterator_2 == skills_a.end () ? 0 : (*iterator_2).second),
                        -1);
  } // end FOR

  label_p =
      GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_LABEL_SKILLSREMAINING_NAME)));
  ACE_ASSERT (label_p);
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << static_cast<unsigned int> (player->getSkillsPerLevel (data_in.subClass));
  gtk_label_set_text (label_p,
                      converter.str ().c_str ());

  // step5: populate list of (new) (known) spells
  list_store_p =
      GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_LISTSTORE_SPELLS_NAME)));
  ACE_ASSERT (list_store_p);
  gtk_list_store_clear (list_store_p);
  RPG_Magic_SpellTypes_t known_spells_a = player->getKnownSpells ();
  RPG_Magic_SpellTypes_t available;
  struct RPG_Magic_CasterClassUnion caster_class_s;
  caster_class_s.discriminator = RPG_Magic_CasterClassUnion::SUBCLASS;
  ACE_UINT16 num_new_known_spells_i = 0;
  RPG_Magic_SpellTypes_t possible_new_spells_a;
  gchar* utf8_string_p = NULL;
  if (!RPG_Magic_Common_Tools::isCasterClass (data_in.subClass))
    goto continue_;
  caster_class_s.subclass = data_in.subClass;
  for (ACE_UINT8 i = 0;
       i <= RPG_COMMON_MAX_SPELL_LEVEL;
       i++)
  {
    // step5a: get list of available spells
    RPG_Magic_SpellTypes_t available_per_level_a =
        RPG_MAGIC_DICTIONARY_SINGLETON::instance ()->getSpells (caster_class_s,
                                                                i);

    // step5b: compute # known spells
    ACE_UINT16 num_known_spells_per_level_i =
      player->getKnownSpellsPerLevel (data_in.subClass,
                                      i);
    if (num_known_spells_per_level_i == 0)
      break; // done
    else if (num_known_spells_per_level_i == std::numeric_limits<ACE_UINT16>::max ())
      num_new_known_spells_i = std::numeric_limits<ACE_UINT16>::max ();
    else
      num_new_known_spells_i += num_known_spells_per_level_i;

    available.insert (available_per_level_a.begin (),
                      available_per_level_a.end ());
  } // end FOR
  if (num_new_known_spells_i != std::numeric_limits<ACE_UINT16>::max ())
    num_new_known_spells_i -= static_cast<ACE_UINT16> (known_spells_a.size ());
  std::set_difference (available.begin (), available.end (),
                       known_spells_a.begin (), known_spells_a.end (),
                       std::inserter (possible_new_spells_a, possible_new_spells_a.end ()));
  for (RPG_Magic_SpellTypesIterator_t iterator_3 = possible_new_spells_a.begin ();
       iterator_3 != possible_new_spells_a.end ();
       ++iterator_3)
  {
    gtk_list_store_append (list_store_p, &tree_iter);
    utf8_string_p =
      Common_UI_GTK_Tools::localeToUTF8 (RPG_Common_Tools::enumToString (RPG_Magic_SpellTypeHelper::RPG_Magic_SpellTypeToString (*iterator_3), true));
    gtk_list_store_set (list_store_p, &tree_iter,
                        0, utf8_string_p, // column 0
                        1, *iterator_3,
                        -1);
    g_free (utf8_string_p); utf8_string_p = NULL;
  } // end FOR

continue_:
  label_p =
      GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_LABEL_SPELLSREMAINING_NAME)));
  ACE_ASSERT (label_p);
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << (num_new_known_spells_i == std::numeric_limits<ACE_UINT16>::max () ? 0 : num_new_known_spells_i);
  gtk_label_set_text (label_p,
                      converter.str ().c_str ());
}

void
update_inventory (const RPG_Player& player_in,
                  GtkBuilder* xml_in)
{
  RPG_TRACE (ACE_TEXT ("::update_inventory"));

  // sanity check(s)
  ACE_ASSERT (xml_in);

  // step17: inventory
  GtkWidget* widget_p =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                      ACE_TEXT_ALWAYS_CHAR("inventory_items_vbox")));
  ACE_ASSERT (widget_p);
  GList* entries = gtk_container_get_children (GTK_CONTAINER (widget_p));
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
    g_list_free_full (entries,
                      (GDestroyNotify)gtk_widget_destroy);
  } // end IF
  const RPG_Player_Inventory& player_inventory = player_in.getInventory ();
  RPG_Player_Equipment& player_equipment =
    const_cast<RPG_Player&> (player_in).getEquipment ();
  std::string text;
  GtkWidget* label_p = NULL;
  RPG_Item_Base* item_p = NULL;
  RPG_Character_EquipmentSlot equipment_slot =
    RPG_CHARACTER_EQUIPMENTSLOT_INVALID;
  for (RPG_Item_ListIterator_t iterator = player_inventory.myItems.begin ();
       iterator != player_inventory.myItems.end ();
       iterator++)
  {
    item_p = NULL;

    // retrieve item handle
    if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->get (*iterator,
                                                                item_p))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid item (ID: %d), returning\n"),
                  *iterator));
      return;
    } // end IF
    ACE_ASSERT (item_p);

    switch (item_p->type ())
    {
      case ITEM_ARMOR:
      {
        RPG_Item_Armor* armor_p = NULL;
        try {
          armor_p = dynamic_cast<RPG_Item_Armor*> (item_p);
        } catch (...) {
          armor_p = NULL;
        }
        if (!armor_p)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to dynamic_cast<RPG_Item_Armor*>(%@), returning\n"),
                      item_p));
          return;
        } // end IF
        text =
          RPG_Common_Tools::enumToString (RPG_Item_ArmorTypeHelper::RPG_Item_ArmorTypeToString (armor_p->armorType ()));

        break;
      }
      case ITEM_COMMODITY:
      {
        RPG_Item_Commodity* commodity_p = NULL;
        try {
          commodity_p = dynamic_cast<RPG_Item_Commodity*> (item_p);
        } catch (...) {
          commodity_p = NULL;
        }
        if (!commodity_p)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to dynamic_cast<RPG_Item_Commodity*>(%@), returning\n"),
                      item_p));
          return;
        } // end IF

        RPG_Item_CommodityUnion commodity_type = commodity_p->subtype ();
        switch (commodity_type.discriminator)
        {
          case RPG_Item_CommodityUnion::COMMODITYBEVERAGE:
          {
            text =
              RPG_Common_Tools::enumToString (RPG_Item_CommodityBeverageHelper::RPG_Item_CommodityBeverageToString (commodity_type.commoditybeverage));
            break;
          }
          case RPG_Item_CommodityUnion::COMMODITYLIGHT:
          {
            text =
              RPG_Common_Tools::enumToString (RPG_Item_CommodityLightHelper::RPG_Item_CommodityLightToString (commodity_type.commoditylight));
            break;
          }
          default:
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("invalid commodity subtype (was: %d), returning\n"),
                        commodity_type.discriminator));
            return;
          }
        } // end SWITCH

        break;
      }
      case ITEM_WEAPON:
      {
        RPG_Item_Weapon* weapon_p = NULL;
        try {
          weapon_p = dynamic_cast<RPG_Item_Weapon*> (item_p);
        } catch (...) {
          weapon_p = NULL;
        }
        if (!weapon_p)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to dynamic_cast<RPG_Item_Weapon*>(%@), returning\n"),
                      item_p));
          return;
        } // end IF

        // *TODO*: pretty-print this string
        text =
          RPG_Common_Tools::enumToString (RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString (weapon_p->weaponType_), false); // don't strip leading "xxx_"

        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid item type (was: \"%s\"), returning\n"),
                    ACE_TEXT (RPG_Item_TypeHelper::RPG_Item_TypeToString (item_p->type ()).c_str ())));
        return;
      }
    } // end SWITCH

    // equipped ? --> add '*'
    if (player_equipment.isEquipped (*iterator, equipment_slot))
      text += ACE_TEXT_ALWAYS_CHAR ('*');

    label_p = NULL;
    label_p = gtk_label_new (text.c_str ());
    ACE_ASSERT (label_p);
//     gtk_container_add (GTK_CONTAINER (widget_p), label_p);
    gtk_box_pack_start (GTK_BOX (widget_p), label_p,
                        FALSE, // expand
                        FALSE, // fill
                        0);    // padding
  } // end FOR
  gtk_widget_show_all (widget_p);
}

void
update_character_profile (const RPG_Player& player_in,
                          GtkBuilder* xml_in)
{
  RPG_TRACE (ACE_TEXT ("::update_character_profile"));

  // sanity check(s)
  ACE_ASSERT (xml_in);

  std::string text;
  std::stringstream converter;
  GtkWidget* widget_p = NULL;

  // step1: name
  widget_p = GTK_WIDGET (gtk_builder_get_object (xml_in,
                                                 ACE_TEXT_ALWAYS_CHAR ("name")));
  ACE_ASSERT(widget_p);
  gchar* text_p = Common_UI_GTK_Tools::localeToUTF8 (player_in.getName ());
  if (!text_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_UI_Tools::Locale2UTF8(\"%s\"), returning\n"),
                ACE_TEXT (player_in.getName ().c_str ())));
    return;
  } // end IF
  gtk_label_set_text (GTK_LABEL (widget_p),
                      text_p);
  g_free (text_p);

  // step2: gender
  const RPG_Character_Gender& player_gender = player_in.getGender ();
  switch (player_gender)
  {
    case RPG_CHARACTER_GENDER_MAX:
    case RPG_CHARACTER_GENDER_INVALID:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid gender (was: \"%s\"), returning\n"),
                  ACE_TEXT (RPG_Character_GenderHelper::RPG_Character_GenderToString (player_gender).c_str ())));
      return;
    }
    case GENDER_NONE:
      text = ACE_TEXT_ALWAYS_CHAR ("N/A"); break;
    default:
    {
      text =
        RPG_Common_Tools::enumToString (RPG_Character_GenderHelper::RPG_Character_GenderToString (player_gender));

      break;
    }
  } // end SWITCH
  widget_p = GTK_WIDGET (gtk_builder_get_object (xml_in,
                                                 ACE_TEXT_ALWAYS_CHAR ("gender")));
  ACE_ASSERT (widget_p);
  gtk_label_set_text (GTK_LABEL (widget_p),
                      text.c_str ());

  // step3: race
  text.clear ();
  const RPG_Character_Race_t& player_race = player_in.getRace ();
  if (player_race.count () == 0)
    text = ACE_TEXT_ALWAYS_CHAR ("N/A");
  else
  {
    unsigned int race_index = 1;
    for (unsigned int index = 0;
         index < player_race.size ();
         index++, race_index++)
    {
      if (player_race.test (index))
      {
        switch (static_cast<RPG_Character_Race> (race_index))
        {
          case RPG_CHARACTER_RACE_MAX:
          case RPG_CHARACTER_RACE_INVALID:
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("invalid race (was: \"%s\"), returning\n"),
                        ACE_TEXT (RPG_Character_RaceHelper::RPG_Character_RaceToString (static_cast<RPG_Character_Race> (race_index)).c_str ())));
            return;
          }
          default:
          {
            text +=
              RPG_Common_Tools::enumToString (RPG_Character_RaceHelper::RPG_Character_RaceToString (static_cast<RPG_Character_Race> (race_index)));

            break;
          }
        } // end SWITCH
        text += ACE_TEXT_ALWAYS_CHAR (",");
      } // end IF
    } // end FOR
    text.erase (--text.end ());
  } // end ELSE
  widget_p = GTK_WIDGET (gtk_builder_get_object (xml_in,
                                                 ACE_TEXT_ALWAYS_CHAR ("race")));
  ACE_ASSERT (widget_p);
  gtk_label_set_text (GTK_LABEL (widget_p),
                      text.c_str ());

  // step4: class
  text.clear ();
  const RPG_Character_Class& player_class = player_in.getClass ();
  switch (player_class.metaClass)
  {
    case RPG_CHARACTER_METACLASS_MAX:
    case RPG_CHARACTER_METACLASS_INVALID:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid metaclass (was: \"%s\"), returning\n"),
                  ACE_TEXT (RPG_Character_MetaClassHelper::RPG_Character_MetaClassToString (player_class.metaClass).c_str ())));
      return;
    }
    case METACLASS_NONE:
      text = ACE_TEXT_ALWAYS_CHAR ("N/A"); break;
    default:
    {
      text =
        RPG_Common_Tools::enumToString (RPG_Character_MetaClassHelper::RPG_Character_MetaClassToString (player_class.metaClass));

      break;
    }
  } // end SWITCH
  if (!player_class.subClasses.empty ())
    text += ACE_TEXT_ALWAYS_CHAR (" (");
  for (RPG_Character_SubClassesIterator_t iterator = player_class.subClasses.begin ();
       iterator != player_class.subClasses.end ();
       iterator++)
  {
    switch (*iterator)
    {
      case RPG_COMMON_SUBCLASS_MAX:
      case RPG_COMMON_SUBCLASS_INVALID:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid subclass (was: \"%s\"), returning\n"),
                    ACE_TEXT (RPG_Common_SubClassHelper::RPG_Common_SubClassToString (*iterator).c_str ())));
        return;
      }
      case SUBCLASS_NONE:
        text += ACE_TEXT_ALWAYS_CHAR ("N/A"); break;
      default:
      {
        text +=
          RPG_Common_Tools::enumToString (RPG_Common_SubClassHelper::RPG_Common_SubClassToString (*iterator));

        break;
      }
    } // end SWITCH
    text += ACE_TEXT_ALWAYS_CHAR (",");
  } // end FOR
  if (!player_class.subClasses.empty ())
  {
    text.erase (--text.end ());
    text += ACE_TEXT_ALWAYS_CHAR (")");
  } // end IF
  widget_p = GTK_WIDGET (gtk_builder_get_object (xml_in,
                                                 ACE_TEXT_ALWAYS_CHAR ("class")));
  ACE_ASSERT (widget_p);
  gtk_label_set_text (GTK_LABEL (widget_p),
                      text.c_str ());

  // step5: alignment
  text.clear ();
  const RPG_Character_Alignment& player_alignment = player_in.getAlignment ();
  // "Neutral" "Neutral" --> "True Neutral"
  if ((player_alignment.civic == ALIGNMENTCIVIC_NEUTRAL) &&
      (player_alignment.ethic == ALIGNMENTETHIC_NEUTRAL))
    text = ACE_TEXT_ALWAYS_CHAR ("True Neutral");
  else
  {
    switch (player_alignment.civic)
    {
      case RPG_CHARACTER_ALIGNMENTCIVIC_MAX:
      case RPG_CHARACTER_ALIGNMENTCIVIC_INVALID:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid alignment (civic) (was: \"%s\"), returning\n"),
                    ACE_TEXT (RPG_Character_AlignmentCivicHelper::RPG_Character_AlignmentCivicToString (player_alignment.civic).c_str ())));
        return;
      }
      default:
      {
        text +=
          RPG_Common_Tools::enumToString (RPG_Character_AlignmentCivicHelper::RPG_Character_AlignmentCivicToString (player_alignment.civic));

        break;
      }
    } // end SWITCH
    text += ACE_TEXT_ALWAYS_CHAR (" ");
    switch (player_alignment.ethic)
    {
      case RPG_CHARACTER_ALIGNMENTETHIC_MAX:
      case RPG_CHARACTER_ALIGNMENTETHIC_INVALID:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid alignment (ethic) (was: \"%s\"), returning\n"),
                    ACE_TEXT (RPG_Character_AlignmentEthicHelper::RPG_Character_AlignmentEthicToString (player_alignment.ethic).c_str ())));
        return;
      }
      default:
      {
        text +=
          RPG_Common_Tools::enumToString (RPG_Character_AlignmentEthicHelper::RPG_Character_AlignmentEthicToString (player_alignment.ethic));

        break;
      }
    } // end SWITCH
  } // end ELSE
  widget_p = GTK_WIDGET (gtk_builder_get_object (xml_in,
                                                 ACE_TEXT_ALWAYS_CHAR ("alignment")));
  ACE_ASSERT (widget_p);
  gtk_label_set_text (GTK_LABEL (widget_p),
                      text.c_str ());

  // step6: HP
  //converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  //converter.clear();
  converter << player_in.getNumHitPoints ();
  converter << ACE_TEXT_ALWAYS_CHAR (" / ");
  converter << player_in.getNumTotalHitPoints ();
  widget_p = GTK_WIDGET (gtk_builder_get_object (xml_in,
                                                 ACE_TEXT_ALWAYS_CHAR ("hitpoints")));
  ACE_ASSERT (widget_p);
  gtk_label_set_text (GTK_LABEL (widget_p),
                      converter.str ().c_str ());

  // step8: XP
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << player_in.getExperience ();
  widget_p =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("experience")));
  ACE_ASSERT (widget_p);
  gtk_label_set_text (GTK_LABEL (widget_p),
                      converter.str ().c_str ());

  // step9: level(s)
  text.clear ();
  unsigned char level = 0;
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  if (player_class.subClasses.empty ())
    converter << static_cast<unsigned int> (level);
  else
  {
    for (RPG_Character_SubClassesIterator_t iterator = player_class.subClasses.begin ();
         iterator != player_class.subClasses.end ();
         iterator++)
    {
      level = player_in.getLevel (*iterator);
      converter << static_cast<unsigned int> (level);
      converter << ACE_TEXT_ALWAYS_CHAR (" / ");
    } // end FOR
  } // end ELSE
  text = converter.str ();
  if (!player_class.subClasses.empty ())
  {
    // trim tailing " / "
    std::string::iterator iterator = text.end ();
    std::advance (iterator, -3);
    text.erase (iterator, text.end ());
  } // end IF
  widget_p = GTK_WIDGET (gtk_builder_get_object (xml_in,
                                                 ACE_TEXT_ALWAYS_CHAR ("level")));
  ACE_ASSERT (widget_p);
  gtk_label_set_text (GTK_LABEL (widget_p),
                      text.c_str ());

  // step10: gold
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << player_in.getWealth ();
  widget_p = GTK_WIDGET (gtk_builder_get_object (xml_in,
                                                 ACE_TEXT_ALWAYS_CHAR ("gold")));
  ACE_ASSERT (widget_p);
  gtk_label_set_text (GTK_LABEL (widget_p),
                      converter.str ().c_str ());

  // step11: condition
  widget_p =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("condition_vbox")));
  ACE_ASSERT (widget_p);
  GList* entries = gtk_container_get_children (GTK_CONTAINER (widget_p));
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
    g_list_free_full (entries,
                      (GDestroyNotify)gtk_widget_destroy);
  } // end IF
  GtkWidget* label_p = NULL;
  const RPG_Character_Conditions_t& player_condition = player_in.getCondition ();
  for (RPG_Character_ConditionsIterator_t iterator = player_condition.begin ();
       iterator != player_condition.end ();
       iterator++)
  {
    text =
      RPG_Common_Tools::enumToString (RPG_Common_ConditionHelper::RPG_Common_ConditionToString (*iterator));
    label_p = NULL;
    label_p = gtk_label_new (text.c_str ());
    ACE_ASSERT (label_p);
//     gtk_container_add(GTK_CONTAINER(current), label);
    gtk_box_pack_start (GTK_BOX (widget_p), label_p,
                        FALSE, // expand
                        FALSE, // fill
                        0);    // padding
  } // end FOR
  gtk_widget_show_all (widget_p);

  // step12: attributes
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << static_cast<unsigned int> (player_in.getAttribute (ATTRIBUTE_STRENGTH));
  widget_p = GTK_WIDGET (gtk_builder_get_object (xml_in,
                                                 ACE_TEXT_ALWAYS_CHAR ("strength")));
  ACE_ASSERT (widget_p);
  gtk_label_set_text (GTK_LABEL (widget_p),
                      converter.str ().c_str ());
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << static_cast<unsigned int> (player_in.getAttribute (ATTRIBUTE_DEXTERITY));
  widget_p =
      GTK_WIDGET (gtk_builder_get_object (xml_in,
                                          ACE_TEXT_ALWAYS_CHAR ("dexterity")));
  ACE_ASSERT (widget_p);
  gtk_label_set_text (GTK_LABEL (widget_p),
                      converter.str ().c_str ());
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << static_cast<unsigned int> (player_in.getAttribute (ATTRIBUTE_CONSTITUTION));
  widget_p =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("constitution")));
  ACE_ASSERT (widget_p);
  gtk_label_set_text (GTK_LABEL (widget_p),
                      converter.str ().c_str ());
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << static_cast<unsigned int> (player_in.getAttribute (ATTRIBUTE_INTELLIGENCE));
  widget_p =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("intelligence")));
  ACE_ASSERT (widget_p);
  gtk_label_set_text (GTK_LABEL (widget_p),
                      converter.str ().c_str ());
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << static_cast<unsigned int> (player_in.getAttribute (ATTRIBUTE_WISDOM));
  widget_p =
      GTK_WIDGET (gtk_builder_get_object (xml_in,
                                          ACE_TEXT_ALWAYS_CHAR ("wisdom")));
  ACE_ASSERT (widget_p);
  gtk_label_set_text (GTK_LABEL (widget_p),
                      converter.str ().c_str ());
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << static_cast<unsigned int> (player_in.getAttribute (ATTRIBUTE_CHARISMA));
  widget_p =
      GTK_WIDGET (gtk_builder_get_object (xml_in,
                                          ACE_TEXT_ALWAYS_CHAR ("charisma")));
  ACE_ASSERT (widget_p);
  gtk_label_set_text (GTK_LABEL (widget_p),
                      converter.str ().c_str ());

  // step7: Attack
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  enum RPG_Common_Attribute attribute_e =
      (player_in.getAttribute (ATTRIBUTE_STRENGTH) > player_in.getAttribute (ATTRIBUTE_DEXTERITY) ? ATTRIBUTE_STRENGTH
                                                                                                  : ATTRIBUTE_DEXTERITY);
  RPG_Character_BaseAttackBonus_t attack_bonusses =
      player_in.getAttackBonus (attribute_e,
                                ATTACK_NORMAL);
  converter << static_cast<int> (attack_bonusses.front ());
  converter << ACE_TEXT_ALWAYS_CHAR (" / ");
  attack_bonusses = player_in.getAttackBonus (attribute_e,
                                              ATTACK_BACKSTAB);
  converter << static_cast<int> (attack_bonusses.front ());
  converter << ACE_TEXT_ALWAYS_CHAR (" / ");
  attack_bonusses = player_in.getAttackBonus (attribute_e,
                                              ATTACK_SURPRISE);
  converter << static_cast<int> (attack_bonusses.front ());
  widget_p =
      GTK_WIDGET (gtk_builder_get_object (xml_in,
                                          ACE_TEXT_ALWAYS_CHAR ("attack")));
  ACE_ASSERT (widget_p);
  gtk_label_set_text (GTK_LABEL (widget_p),
                      converter.str ().c_str ());

  // step8: Damage
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  RPG_Item_WeaponType weapon_type =
      const_cast<RPG_Player&> (player_in).getEquipment ().getPrimaryWeapon (player_in.getOffHand ());
  if (weapon_type == RPG_ITEM_WEAPONTYPE_INVALID)
    weapon_type =
        const_cast<RPG_Player&> (player_in).getEquipment().getSecondaryWeapon (player_in.getOffHand ());
  RPG_Item_WeaponProperties weapon_properties =
      RPG_ITEM_DICTIONARY_SINGLETON::instance ()->getWeaponProperties (weapon_type);
  converter << RPG_Dice_Common_Tools::toString (weapon_properties.baseDamage);
  widget_p =
      GTK_WIDGET (gtk_builder_get_object (xml_in,
                                          ACE_TEXT_ALWAYS_CHAR ("damage")));
  ACE_ASSERT (widget_p);
  gtk_label_set_text (GTK_LABEL (widget_p),
                      converter.str ().c_str ());

  // step9: AC
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << static_cast<int> (player_in.getArmorClass (DEFENSE_NORMAL));
  converter << ACE_TEXT_ALWAYS_CHAR(" / ");
  converter << static_cast<int> (player_in.getArmorClass (DEFENSE_TOUCH));
  converter << ACE_TEXT_ALWAYS_CHAR(" / ");
  converter << static_cast<int> (player_in.getArmorClass (DEFENSE_FLATFOOTED));
  widget_p =
      GTK_WIDGET (gtk_builder_get_object (xml_in,
                                          ACE_TEXT_ALWAYS_CHAR ("armorclass")));
  ACE_ASSERT (widget_p);
  gtk_label_set_text (GTK_LABEL (widget_p),
                      converter.str ().c_str ());

  // step13: feats
  widget_p =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("feats_vbox")));
  ACE_ASSERT (widget_p);
  entries = gtk_container_get_children (GTK_CONTAINER (widget_p));
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
    g_list_free_full (entries,
                      (GDestroyNotify)gtk_widget_destroy);
  } // end IF
  const RPG_Character_Feats_t& player_feats = player_in.getFeats ();
  for (RPG_Character_FeatsConstIterator_t iterator = player_feats.begin ();
       iterator != player_feats.end ();
       iterator++)
  {
    text =
      RPG_Common_Tools::enumToString (RPG_Character_FeatHelper::RPG_Character_FeatToString (*iterator));
    label_p = NULL;
    label_p = gtk_label_new (text.c_str ());
    ACE_ASSERT (label_p);
//     gtk_container_add(GTK_CONTAINER(current), label_p);
    gtk_box_pack_start (GTK_BOX (widget_p), label_p,
                        FALSE, // expand
                        FALSE, // fill
                        0);    // padding
  } // end FOR
  gtk_widget_show_all (widget_p);

  // step14: abilities
  widget_p =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("abilities_vbox")));
  ACE_ASSERT (widget_p);
  entries = gtk_container_get_children (GTK_CONTAINER (widget_p));
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
    g_list_free_full (entries,
                      (GDestroyNotify)gtk_widget_destroy);
  } // end IF
  const RPG_Character_Abilities_t& player_abilities = player_in.getAbilities ();
  for (RPG_Character_AbilitiesConstIterator_t iterator = player_abilities.begin ();
       iterator != player_abilities.end ();
       iterator++)
  {
    text =
      RPG_Common_Tools::enumToString (RPG_Character_AbilityHelper::RPG_Character_AbilityToString (*iterator));
    label_p = NULL;
    label_p = gtk_label_new (text.c_str ());
    ACE_ASSERT (label_p);
//     gtk_container_add(GTK_CONTAINER(widget_p), label_p);
    gtk_box_pack_start (GTK_BOX (widget_p), label_p,
                        FALSE, // expand
                        FALSE, // fill
                        0);    // padding
  } // end FOR
  gtk_widget_show_all (widget_p);

  // step15: skills
  widget_p =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("skills_table")));
  ACE_ASSERT (widget_p);
  entries = gtk_container_get_children (GTK_CONTAINER (widget_p));
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
    g_list_free_full (entries,
                      (GDestroyNotify)gtk_widget_destroy);
  } // end IF
  //GtkWidget* current_box = NULL;
  const RPG_Character_Skills_t& player_skills = player_in.getSkills ();
  gtk_table_resize (GTK_TABLE (widget_p),
                    static_cast<guint> (player_skills.size ()), 2);
  unsigned int index = 0;
  GtkAttachOptions attach_options = GTK_SHRINK;
  for (RPG_Character_SkillsConstIterator_t iterator = player_skills.begin ();
       iterator != player_skills.end ();
       iterator++, index++)
  {
    text =
      RPG_Common_Tools::enumToString (RPG_Common_SkillHelper::RPG_Common_SkillToString ((*iterator).first));
    //current_box = NULL;
    //current_box = gtk_hbox_new(TRUE, // homogeneous
    //                           0);   // spacing
    //ACE_ASSERT(current_box);
    label_p = NULL;
    label_p = gtk_label_new (text.c_str ());
    ACE_ASSERT (label_p);
    //gtk_box_pack_start(GTK_BOX(current_box), label_p,
    //                   FALSE, // expand
    //                   FALSE, // fill
    //                   0);    // padding
    gtk_table_attach_defaults (GTK_TABLE (widget_p),
                               label_p,
                               0, 1,
                               index, index + 1);

    converter.str (ACE_TEXT (""));
    converter.clear ();
    converter << static_cast<unsigned int> ((*iterator).second);
    text = converter.str ();
    label_p = NULL;
    label_p = gtk_label_new (text.c_str ());
    ACE_ASSERT (label_p);
    //gtk_box_pack_start(GTK_BOX(current_box), label_p,
    //                   FALSE, // expand
    //                   FALSE, // fill
    //                   0);    // padding
    gtk_table_attach (GTK_TABLE (widget_p),
                      label_p,
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
  gtk_widget_show_all (widget_p);

  // step16: spells
  GtkNotebook* notebook_p =
    GTK_NOTEBOOK (gtk_builder_get_object (xml_in,
                                          ACE_TEXT_ALWAYS_CHAR ("notebook")));
  ACE_ASSERT (notebook_p);
  widget_p = gtk_notebook_get_nth_page (notebook_p,
                                        3);
  //widget_p =
  //    GTK_WIDGET(gtk_builder_get_object(xml_in,
  //                                    ACE_TEXT_ALWAYS_CHAR("spells_frame")));
  ACE_ASSERT (widget_p);
  gtk_widget_set_sensitive (widget_p,
                            RPG_Magic_Common_Tools::hasCasterClass (player_class));
  //if (!RPG_Magic_Common_Tools::hasCasterClass (player_class))
  //  gtk_widget_hide_all (widget_p);
  //else
  //  gtk_widget_show_all (widget_p);

  // divine casters know ALL spells from the levels they can cast...
  widget_p =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("known_spells_frame")));
  ACE_ASSERT (widget_p);
  if (!RPG_Magic_Common_Tools::hasArcaneCasterClass (player_class))
    gtk_widget_hide (widget_p);
  else
    gtk_widget_show_all (widget_p);

  widget_p =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("known_spells_vbox")));
  ACE_ASSERT (widget_p);
  entries = gtk_container_get_children (GTK_CONTAINER (widget_p));
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
    g_list_free_full (entries,
                      (GDestroyNotify)gtk_widget_destroy);
  } // end IF
  const RPG_Magic_SpellTypes_t& player_known_spells =
    player_in.getKnownSpells ();
  for (RPG_Magic_SpellTypesIterator_t iterator = player_known_spells.begin ();
       iterator != player_known_spells.end ();
       iterator++)
  {
    text =
      RPG_Common_Tools::enumToString (RPG_Magic_SpellTypeHelper::RPG_Magic_SpellTypeToString (*iterator));

    label_p = NULL;
    label_p = gtk_label_new (text.c_str ());
    ACE_ASSERT (label_p);
//     gtk_container_add(GTK_CONTAINER (widget_p), label_p);
    gtk_box_pack_start (GTK_BOX (widget_p), label_p,
                        FALSE, // expand
                        FALSE, // fill
                        0);    // padding
  } // end FOR
  gtk_widget_show_all (widget_p);
  widget_p =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                      ACE_TEXT_ALWAYS_CHAR ("prepared_spells_table")));
  ACE_ASSERT (widget_p);
  entries = gtk_container_get_children (GTK_CONTAINER (widget_p));
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
    g_list_free_full (entries,
                      (GDestroyNotify)gtk_widget_destroy);
  } // end IF
  const RPG_Magic_Spells_t& player_spells = player_in.getSpells ();
  unsigned int count = 0;
  RPG_Magic_SpellTypes_t processsed_types;
  std::map<RPG_Magic_SpellType, unsigned int> spells;
  for (RPG_Magic_SpellsIterator_t iterator = player_spells.begin ();
       iterator != player_spells.end ();
       iterator++)
  {
    if (processsed_types.find (*iterator) != processsed_types.end ())
      continue; // already accounted for

    // count instances
    count = 0;
    for (RPG_Magic_SpellsIterator_t iterator2 = player_spells.begin ();
         iterator2 != player_spells.end ();
         iterator2++)
      if (*iterator2 == *iterator)
        count++;

    spells[*iterator] = count;
    processsed_types.insert (*iterator);
  } // end FOR
  gtk_table_resize (GTK_TABLE (widget_p),
                    static_cast<guint> (spells.size ()), 2);
  index = 0;
  for (std::map<RPG_Magic_SpellType, unsigned int>::const_iterator iterator = spells.begin ();
       iterator != spells.end ();
       ++iterator, ++index)
  {
    text =
      RPG_Common_Tools::enumToString (RPG_Magic_SpellTypeHelper::RPG_Magic_SpellTypeToString ((*iterator).first));
    //current_box = NULL;
    //current_box = gtk_hbox_new(TRUE, // homogeneous
    //                           0);   // spacing
    //ACE_ASSERT(current_box);
    label_p = NULL;
    label_p = gtk_label_new (text.c_str ());
    ACE_ASSERT (label_p);
    //gtk_box_pack_start (GTK_BOX (current_box), label_p,
    //                   FALSE, // expand
    //                   FALSE, // fill
    //                   0);    // padding
    gtk_table_attach_defaults (GTK_TABLE (widget_p),
                               label_p,
                               0, 1,
                               index, index + 1);

    converter.str (ACE_TEXT (""));
    converter.clear ();
    converter << (*iterator).second;
    text = converter.str ();
    label_p = NULL;
    label_p = gtk_label_new (text.c_str ());
    ACE_ASSERT (label_p);
    //gtk_box_pack_start (GTK_BOX (current_box), label_p,
    //                   FALSE, // expand
    //                   FALSE, // fill
    //                   0);    // padding
    gtk_table_attach (GTK_TABLE (widget_p),
                      label_p,
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
  gtk_widget_show_all (widget_p);

  // step17: inventory
  update_inventory (player_in,
                    xml_in);
}

void
update_entity_profile (const struct RPG_Engine_Entity& entity_in,
                       GtkBuilder* xml_in)
{
  RPG_TRACE (ACE_TEXT ("::update_entity_profile"));

  // sanity checks
  ACE_ASSERT (entity_in.character);
  ACE_ASSERT (entity_in.character->isPlayerCharacter ());
  RPG_Player* player = static_cast<RPG_Player*> (entity_in.character);
  ACE_ASSERT (xml_in);

  // step1: update character profile
  ::update_character_profile (*player, xml_in);

  // step2: update image (if available)
  // retrieve image widget
  GtkImage* image =
    GTK_IMAGE (gtk_builder_get_object (xml_in,
                                       ACE_TEXT_ALWAYS_CHAR ("image_sprite")));
  ACE_ASSERT (image);

  // retrieve character handle
  RPG_Player_Player_Base* player_base = NULL;
  player_base = static_cast<RPG_Player_Player_Base*> (entity_in.character);
  std::string filename;
  RPG_Graphics_GraphicTypeUnion type;
  type.sprite = RPG_Client_Common_Tools::classToSprite (player_base->getClass ());
  if (type.sprite != RPG_GRAPHICS_SPRITE_INVALID)
  {
    type.discriminator = RPG_Graphics_GraphicTypeUnion::SPRITE;
    RPG_Graphics_t graphic;
    graphic.category = RPG_GRAPHICS_CATEGORY_INVALID;
    graphic.type.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;
    // retrieve properties from the dictionary
    graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance ()->get (type);
    ACE_ASSERT ((graphic.type.sprite == type.sprite) &&
                (graphic.type.discriminator == RPG_Graphics_GraphicTypeUnion::SPRITE));
    // sanity check
    if (graphic.category != CATEGORY_SPRITE)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid category (was: \"%s\"), not a sprite type, returning\n"),
                  ACE_TEXT (RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString (graphic.category).c_str ())));
      return;
    } // end IF

    // assemble path
    RPG_Graphics_Common_Tools::graphicToFile (graphic, filename);
    ACE_ASSERT (!filename.empty ());
    // sanity check(s)
    if (!Common_File_Tools::isReadable (filename))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_File_Tools::isReadable(\"%s\"), returning\n"),
                  ACE_TEXT (filename.c_str ())));
      return;
    } // end IF
  } // end IF
  if (!filename.empty ())
    gtk_image_set_from_file (image, filename.c_str ());
  else
    gtk_image_clear (image);
}

void
reset_character_profile (GtkBuilder* xml_in)
{
  RPG_TRACE (ACE_TEXT ("::reset_character_profile"));

  // sanity checks
  ACE_ASSERT (xml_in);

  // step1: reset character profile
  GtkWidget* current = NULL;

  // step1a: name
  current = GTK_WIDGET (gtk_builder_get_object (xml_in,
                                                ACE_TEXT_ALWAYS_CHAR ("name")));
  ACE_ASSERT (current);
  gtk_label_set_text (GTK_LABEL (current), NULL);

  // step1b: gender
  current = GTK_WIDGET (gtk_builder_get_object (xml_in,
                                                ACE_TEXT_ALWAYS_CHAR ("gender")));
  ACE_ASSERT (current);
  gtk_label_set_text (GTK_LABEL (current), NULL);

  // step1c: race
  current = GTK_WIDGET (gtk_builder_get_object (xml_in,
                                                ACE_TEXT_ALWAYS_CHAR ("race")));
  ACE_ASSERT (current);
  gtk_label_set_text (GTK_LABEL (current), NULL);

  // step1d: class
  current = GTK_WIDGET (gtk_builder_get_object (xml_in,
                                                ACE_TEXT_ALWAYS_CHAR ("class")));
  ACE_ASSERT (current);
  gtk_label_set_text (GTK_LABEL (current), NULL);

  // step1e: alignment
  current = GTK_WIDGET (gtk_builder_get_object (xml_in,
                                                ACE_TEXT_ALWAYS_CHAR ("alignment")));
  ACE_ASSERT (current);
  gtk_label_set_text (GTK_LABEL (current), NULL);

  // step1f: HP
  current = GTK_WIDGET (gtk_builder_get_object (xml_in,
                                                ACE_TEXT_ALWAYS_CHAR ("hitpoints")));
  ACE_ASSERT (current);
  gtk_label_set_text (GTK_LABEL (current), NULL);

  // step1h: XP
  current =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("experience")));
  ACE_ASSERT (current);
  gtk_label_set_text (GTK_LABEL (current), NULL);

  // step1i: level(s)
  current = GTK_WIDGET (gtk_builder_get_object (xml_in,
                                                ACE_TEXT_ALWAYS_CHAR ("level")));
  ACE_ASSERT (current);
  gtk_label_set_text (GTK_LABEL (current), NULL);

  // step1j: gold
  current = GTK_WIDGET (gtk_builder_get_object (xml_in,
                                                ACE_TEXT_ALWAYS_CHAR ("gold")));
  ACE_ASSERT (current);
  gtk_label_set_text (GTK_LABEL (current), NULL);

  // step1k: condition
  current =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("condition_vbox")));
  ACE_ASSERT (current);
  GList* entries = gtk_container_get_children (GTK_CONTAINER (current));
//   ACE_ASSERT(entries);
  if (entries)
  {
    for (GList* iterator = entries;
         iterator;
         iterator = g_list_next (iterator))
    {
      // *NOTE*: effectively removes the widget from the container...
      gtk_widget_destroy (GTK_WIDGET (iterator->data));
    } // end FOR

    g_list_free (entries);
  } // end IF

  // step1l: attributes
  current = GTK_WIDGET (gtk_builder_get_object (xml_in,
                                                ACE_TEXT_ALWAYS_CHAR ("strength")));
  ACE_ASSERT (current);
  gtk_label_set_text (GTK_LABEL (current), NULL);
  current = GTK_WIDGET (gtk_builder_get_object (xml_in,
                                                ACE_TEXT_ALWAYS_CHAR ("dexterity")));
  ACE_ASSERT (current);
  gtk_label_set_text (GTK_LABEL (current), NULL);
  current =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("constitution")));
  ACE_ASSERT (current);
  gtk_label_set_text (GTK_LABEL (current), NULL);
  current =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("intelligence")));
  ACE_ASSERT (current);
  gtk_label_set_text (GTK_LABEL (current), NULL);
  current =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("wisdom")));
  ACE_ASSERT (current);
  gtk_label_set_text (GTK_LABEL (current), NULL);
  current =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("charisma")));
  ACE_ASSERT (current);
  gtk_label_set_text (GTK_LABEL (current), NULL);

  // step1g: AC
  current =
      GTK_WIDGET (gtk_builder_get_object (xml_in,
                                          ACE_TEXT_ALWAYS_CHAR ("armorclass")));
  ACE_ASSERT (current);
  gtk_label_set_text (GTK_LABEL (current), NULL);

  // step1m: feats
  current =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("feats_vbox")));
  ACE_ASSERT (current);
  entries = gtk_container_get_children (GTK_CONTAINER (current));
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
    g_list_free_full (entries,
                      (GDestroyNotify)gtk_widget_destroy);
  } // end IF

  // step1n: abilities
  current =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("abilities_vbox")));
  ACE_ASSERT (current);
  entries = gtk_container_get_children (GTK_CONTAINER (current));
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
    g_list_free_full (entries,
                      (GDestroyNotify)gtk_widget_destroy);
  } // end IF

  // step1o: skills
  current =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("skills_table")));
  ACE_ASSERT (current);
  entries = gtk_container_get_children (GTK_CONTAINER (current));
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
    g_list_free_full (entries,
                      (GDestroyNotify)gtk_widget_destroy);
  } // end IF

  // step1p: spells
  current =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("spells_frame")));
  ACE_ASSERT (current);
  gtk_widget_show_all (current);
  current =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("known_spells_frame")));
  ACE_ASSERT (current);
  gtk_widget_show_all (current);
  current =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("known_spells_vbox")));
  ACE_ASSERT (current);
  entries = gtk_container_get_children (GTK_CONTAINER (current));
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
    g_list_free_full (entries,
                      (GDestroyNotify)gtk_widget_destroy);
  } // end IF
  current =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("prepared_spells_table")));
  ACE_ASSERT (current);
  entries = gtk_container_get_children (GTK_CONTAINER (current));
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
    g_list_free_full (entries,
                      (GDestroyNotify)gtk_widget_destroy);
  } // end IF

  // step1q: inventory
  current =
    GTK_WIDGET (gtk_builder_get_object (xml_in,
                                        ACE_TEXT_ALWAYS_CHAR ("inventory_items_vbox")));
  ACE_ASSERT (current);
  entries = gtk_container_get_children (GTK_CONTAINER (current));
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
    g_list_free_full (entries,
                      (GDestroyNotify)gtk_widget_destroy);
  } // end IF
}

// callbacks used by ::scandir...
int
dirent_selector_profiles (const dirent* entry_in)
{
  //RPG_TRACE (ACE_TEXT ("::dirent_selector_profiles"));

  // *NOTE*: select player profiles
  std::string filename (entry_in->d_name);
  std::string extension (ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT));
  std::string::size_type position =
    filename.rfind (extension, std::string::npos);
  if ((position == std::string::npos) ||
      (position != (filename.size () - extension.size ())))
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
dirent_selector_maps (const dirent* entry_in)
{
  //RPG_TRACE (ACE_TEXT ("::dirent_selector_maps"));

  // *NOTE*: select maps
  std::string filename (entry_in->d_name);
  std::string extension (ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_FILE_EXT));
  std::string::size_type position =
    filename.rfind (extension, std::string::npos);
  if ((position == std::string::npos) ||
      (position != (filename.size () - extension.size ())))
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
dirent_selector_savedstates (const dirent* entry_in)
{
  //RPG_TRACE (ACE_TEXT ("::dirent_selector_savedstates"));

  // *NOTE*: select engine states
  std::string filename (entry_in->d_name);
  std::string extension (ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_STATE_EXT));
  std::string::size_type position =
    filename.rfind (extension, std::string::npos);
  if ((position == std::string::npos) ||
      (position != (filename.size () - extension.size ())))
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
dirent_comparator (const dirent** entry1_in,
                   const dirent** entry2_in)
{
  //RPG_TRACE (ACE_TEXT ("::dirent_comparator"));

  return ACE_OS::strcmp ((*entry1_in)->d_name,
                         (*entry2_in)->d_name);
}

unsigned int
load_files (const RPG_Client_Repository& repository_in,
            GtkListStore* listStore_in)
{
  RPG_TRACE (ACE_TEXT ("::load_files"));

  unsigned int return_value = 0;

  // sanity check(s)
  ACE_ASSERT (listStore_in);
  std::string repository;
  ACE_SCANDIR_SELECTOR selector = NULL;
  std::string extension;
  switch (repository_in)
  {
    case REPOSITORY_MAPS:
    {
      repository = RPG_Map_Common_Tools::getMapsDirectory ();
      selector = ::dirent_selector_maps;
      extension = ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_FILE_EXT);
      break;
    }
    case REPOSITORY_PROFILES:
    {
      repository = RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
      selector = ::dirent_selector_profiles;
      extension = ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);
      break;
    }
    case REPOSITORY_ENGINESTATE:
    {
      repository = RPG_Engine_Common_Tools::getEngineStateDirectory ();
      selector = ::dirent_selector_savedstates;
      extension = ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_STATE_EXT);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid repository (was: %d), aborting\n"),
                  repository_in));
      return 0;
    }
  } // end IF
  if (!Common_File_Tools::isDirectory (repository))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to load_files(\"%s\"), not a directory, aborting\n"),
                ACE_TEXT (repository.c_str ())));
    return 0;
  } // end IF

  // retrieve all relevant files and sort them alphabetically...
  ACE_Dirent_Selector entries;
  if (entries.open (repository.c_str (),
                    selector,
                    &::dirent_comparator) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Dirent_Selector::open(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (repository.c_str ())));
    return 0;
  } // end IF

  // clear existing entries
  // *WARNING* triggers the "changed" signal of the combobox...
  gtk_list_store_clear (listStore_in);

  // iterate over entries
  std::string entry;
  GtkTreeIter iter;
  for (unsigned int i = 0;
       i < static_cast<unsigned int>(entries.length ());
       i++)
  {
    // sanitize name (chop off extension)
    entry = entries[i]->d_name;
    size_t position = entry.rfind (extension,
                            std::string::npos);
    ACE_ASSERT (position != std::string::npos);
    entry.erase (position,
                 std::string::npos);

    // append new (text) entry
    gtk_list_store_append (listStore_in, &iter);
    gtk_list_store_set (listStore_in, &iter,
                        0, ACE_TEXT (entry.c_str ()), // column 0
                        -1);

    return_value++;
  } // end FOR

  // clean up
  if (entries.close () == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Dirent_Selector::close: \"%m\", continuing\n")));

  // debug info
  GValue value;
  ACE_OS::memset (&value, 0, sizeof (value));
  const gchar* text = NULL;
  if (!gtk_tree_model_get_iter_first (GTK_TREE_MODEL (listStore_in),
                                      &iter))
    return 0;
  gtk_tree_model_get_value (GTK_TREE_MODEL (listStore_in), &iter,
                            0, &value);
  text = g_value_get_string (&value);
  extension.erase (0, 1);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%s[0]: %s\n"),
              ACE_TEXT (extension.c_str ()),
              ACE_TEXT (text)));
  g_value_unset (&value);
  for (unsigned int i = 1;
       gtk_tree_model_iter_next (GTK_TREE_MODEL (listStore_in),
                                 &iter);
       i++)
  {
    ACE_OS::memset (&value, 0, sizeof (value));
    text = NULL;

    gtk_tree_model_get_value (GTK_TREE_MODEL (listStore_in), &iter,
                              0, &value);
    text = g_value_get_string (&value);
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%s[%u]: %s\n"),
                ACE_TEXT (extension.c_str ()),
                i,
                ACE_TEXT (text)));

    // clean up
    g_value_unset (&value);
  } // end FOR

  return return_value;
}

gint
combobox_sort_function (GtkTreeModel* model_in,
                        GtkTreeIter*  iterator1_in,
                        GtkTreeIter*  iterator2_in,
                        gpointer      userData_in)
{
  RPG_TRACE (ACE_TEXT ("::combobox_sort_function"));

  gint sort_column = GPOINTER_TO_INT (userData_in);
  gint result = 0; // -1: row1 < row2, 0: equal, 1: row1 > row2

  switch (sort_column)
  {
    case 0:
    {
      gchar* row1, *row2;
      gtk_tree_model_get (model_in, iterator1_in, sort_column, &row1, -1);
      gtk_tree_model_get (model_in, iterator2_in, sort_column, &row2, -1);
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
      row1_lower = g_utf8_strdown (row1, -1);
      row2_lower = g_utf8_strdown (row2, -1);
      g_free (row1);
      g_free (row2);

      result = g_utf8_collate (row1_lower, row2_lower);
      g_free (row1_lower);
      g_free (row2_lower);

      break;
    }
    default:
      g_return_val_if_reached (0);
  } // end SWITCH

  return result;
}

gboolean
idle_initialize_UI_cb (gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::idle_initialize_UI_cb"));

  struct RPG_Client_GTK_CBData* data_p =
      static_cast<struct RPG_Client_GTK_CBData*> (userData_in);

       // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GTK_BuildersConstIterator_t iterator =
      data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

       // step2: init dialog windows
  GtkFileChooserDialog* filechooser_dialog =
      GTK_FILE_CHOOSER_DIALOG (gtk_builder_get_object ((*iterator).second.second,
                                                       ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_FILECHOOSER_NAME)));
  ACE_ASSERT (filechooser_dialog);
  data_p->mapFilter = gtk_file_filter_new ();
  if (!data_p->mapFilter)
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to gtk_file_filter_new(): \"%m\", aborting\n")));

         // clean up
    g_object_unref (G_OBJECT ((*iterator).second.second));

    return FALSE; // G_SOURCE_REMOVE
  } // end IF
  gtk_file_filter_set_name (data_p->mapFilter,
                            ACE_TEXT (RPG_ENGINE_LEVEL_FILE_EXT));
  std::string pattern = ACE_TEXT_ALWAYS_CHAR ("*");
  pattern += ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_FILE_EXT);
  gtk_file_filter_add_pattern (data_p->mapFilter,
                               ACE_TEXT (pattern.c_str ()));
  //gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(filechooser_dialog), userData_in.map_filter);
  //g_object_ref(G_OBJECT(userData_in.map_filter));
  data_p->entityFilter = gtk_file_filter_new ();
  if (!data_p->entityFilter)
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to gtk_file_filter_new(): \"%m\", aborting\n")));

         // clean up
    g_object_unref (G_OBJECT (data_p->mapFilter));
    g_object_unref (G_OBJECT ((*iterator).second.second));

    return FALSE; // G_SOURCE_REMOVE
  } // end IF
  gtk_file_filter_set_name (data_p->entityFilter,
                            ACE_TEXT (RPG_PLAYER_PROFILE_EXT));
  pattern = ACE_TEXT_ALWAYS_CHAR ("*");
  pattern += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);
  gtk_file_filter_add_pattern (data_p->entityFilter,
                               ACE_TEXT (pattern.c_str ()));
  //gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(filechooser_dialog),
  //                            userData_in.entityFilter);
  //g_object_ref(G_OBJECT(userData_in.entity_filter));
  data_p->savedStateFilter = gtk_file_filter_new ();
  if (!data_p->savedStateFilter)
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to gtk_file_filter_new(): \"%m\", aborting\n")));

         // clean up
    g_object_unref (G_OBJECT (data_p->entityFilter));
    g_object_unref (G_OBJECT (data_p->mapFilter));
    g_object_unref (G_OBJECT ((*iterator).second.second));

    return FALSE; // G_SOURCE_REMOVE
  } // end IF
  gtk_file_filter_set_name (data_p->savedStateFilter,
                           ACE_TEXT (RPG_ENGINE_STATE_EXT));
  pattern = ACE_TEXT_ALWAYS_CHAR ("*");
  pattern += ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_STATE_EXT);
  gtk_file_filter_add_pattern (data_p->savedStateFilter,
                               ACE_TEXT (pattern.c_str ()));
  //gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(filechooser_dialog), userData_in.savedstate_filter);
  //g_object_ref(G_OBJECT(userData_in.savedstate_filter));

  GtkWidget* about_dialog =
      GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_ABOUT_NAME)));
  ACE_ASSERT (about_dialog);

  GtkWidget* equipment_dialog =
      GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_EQUIPMENT_NAME)));
  ACE_ASSERT (equipment_dialog);
  GtkWidget* level_dialog =
      GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_LEVELUP_NAME)));
  ACE_ASSERT (level_dialog);

  // set window icon
  GtkWidget* main_dialog =
      GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_MAIN_NAME)));
  ACE_ASSERT (main_dialog);
  RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::IMAGE;
  type.image = IMAGE_WM_ICON;
  RPG_Graphics_t icon_graphic =
      RPG_GRAPHICS_DICTIONARY_SINGLETON::instance ()->get (type);
  ACE_ASSERT (icon_graphic.type.image == IMAGE_WM_ICON);
  std::string path = RPG_Graphics_Common_Tools::getGraphicsDirectory ();
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_TILE_IMAGES_SUB);
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += icon_graphic.file;
  GtkWidget* image_icon = gtk_image_new_from_file (path.c_str ());
  ACE_ASSERT (image_icon);
  gtk_window_set_icon (GTK_WINDOW (main_dialog),
                       gtk_image_get_pixbuf (GTK_IMAGE (image_icon)));
  //GdkWindow* main_dialog_window = gtk_widget_get_window(main_dialog);
  //gtk_window_set_title(,
  //                     caption.c_str());

  GtkWidget* main_entry_dialog =
      GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_ENTRY_NAME)));
  ACE_ASSERT (main_entry_dialog);
  GtkEntry* entry =
      GTK_ENTRY (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_ENTRY_NAME)));
  ACE_ASSERT (entry);
  GtkEntryBuffer* entry_buffer =
      gtk_entry_buffer_new (ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_DEF_NAME), // text
                           -1);                                               // length in bytes (-1: \0-terminated)
  ACE_ASSERT (entry_buffer);
  if (!entry_buffer)
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to gtk_entry_buffer_new(): \"%m\", aborting\n")));

         // clean up
    g_object_unref (G_OBJECT (data_p->savedStateFilter));
    g_object_unref (G_OBJECT (data_p->entityFilter));
    g_object_unref (G_OBJECT (data_p->mapFilter));
    g_object_unref (G_OBJECT ((*iterator).second.second));

    return FALSE; // G_SOURCE_REMOVE
  } // end IF
  gtk_entry_set_buffer (entry, entry_buffer);
  //   g_object_unref(G_OBJECT(entry_buffer));

       // step3: populate comboboxes
       // step3a: character repository
  GtkComboBox* combobox =
      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
  ACE_ASSERT (combobox);
  gtk_cell_layout_clear (GTK_CELL_LAYOUT (combobox));
  GtkCellRenderer* renderer = gtk_cell_renderer_text_new ();
  if (!renderer)
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to gtk_cell_renderer_text_new(): \"%m\", aborting\n")));

         // clean up
    g_object_unref (G_OBJECT (data_p->savedStateFilter));
    g_object_unref (G_OBJECT (data_p->entityFilter));
    g_object_unref (G_OBJECT (data_p->mapFilter));
    g_object_unref (G_OBJECT ((*iterator).second.second));

    return FALSE; // G_SOURCE_REMOVE
  } // end IF
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combobox), renderer,
                              TRUE); // expand ?
  //   gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(repository_combobox), renderer,
  //                                 ACE_TEXT_ALWAYS_CHAR("text"), 0);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combobox), renderer,
                                  ACE_TEXT_ALWAYS_CHAR ("text"), 0,
                                  NULL);
  GtkListStore* list = gtk_list_store_new (1, G_TYPE_STRING);
  if (!list)
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to gtk_list_store_new(): \"%m\", aborting\n")));

         // clean up
    g_object_unref (G_OBJECT (data_p->savedStateFilter));
    g_object_unref (G_OBJECT (data_p->entityFilter));
    g_object_unref (G_OBJECT (data_p->mapFilter));
    g_object_unref (G_OBJECT ((*iterator).second.second));

    return FALSE; // G_SOURCE_REMOVE
  } // end IF
  gtk_combo_box_set_model (combobox, GTK_TREE_MODEL (list));
  g_object_unref (G_OBJECT (list));
  if (::load_files (REPOSITORY_PROFILES, list))
    gtk_widget_set_sensitive (GTK_WIDGET (combobox), TRUE);

  // step3b: maps repository
  combobox =
      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_COMBOBOX_MAP_NAME)));
  ACE_ASSERT (combobox);
  gtk_cell_layout_clear (GTK_CELL_LAYOUT (combobox));
  renderer = gtk_cell_renderer_text_new ();
  if (!renderer)
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to gtk_cell_renderer_text_new(): \"%m\", aborting\n")));

         // clean up
    g_object_unref (G_OBJECT (data_p->savedStateFilter));
    g_object_unref (G_OBJECT (data_p->entityFilter));
    g_object_unref (G_OBJECT (data_p->mapFilter));
    g_object_unref (G_OBJECT ((*iterator).second.second));

    return FALSE; // G_SOURCE_REMOVE
  } // end IF
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combobox), renderer,
                              TRUE); // expand ?
  //   gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(combobox), renderer,
  //                                 ACE_TEXT_ALWAYS_CHAR("text"), 0);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combobox), renderer,
                                  ACE_TEXT_ALWAYS_CHAR ("text"), 0,
                                  NULL);
  list = gtk_list_store_new (1, G_TYPE_STRING);
  if (!list)
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to gtk_list_store_new(): \"%m\", aborting\n")));

         // clean up
    g_object_unref (G_OBJECT (data_p->savedStateFilter));
    g_object_unref (G_OBJECT (data_p->entityFilter));
    g_object_unref (G_OBJECT (data_p->mapFilter));
    g_object_unref (G_OBJECT ((*iterator).second.second));

    return FALSE; // G_SOURCE_REMOVE
  } // end IF
  gtk_combo_box_set_model (combobox, GTK_TREE_MODEL (list));
  g_object_unref (G_OBJECT (list));
  if (::load_files (REPOSITORY_MAPS, list))
    gtk_widget_set_sensitive (GTK_WIDGET (combobox), TRUE);

       // step3c: savedstate repository
  combobox =
      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_COMBOBOX_ENGINESTATE_NAME)));
  ACE_ASSERT (combobox);
  gtk_cell_layout_clear (GTK_CELL_LAYOUT (combobox));
  renderer = gtk_cell_renderer_text_new ();
  if (!renderer)
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to gtk_cell_renderer_text_new(): \"%m\", aborting\n")));

         // clean up
    g_object_unref (G_OBJECT (data_p->savedStateFilter));
    g_object_unref (G_OBJECT (data_p->entityFilter));
    g_object_unref (G_OBJECT (data_p->mapFilter));
    g_object_unref (G_OBJECT ((*iterator).second.second));

    return FALSE; // G_SOURCE_REMOVE
  } // end IF
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combobox), renderer,
                              TRUE); // expand ?
  //   gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(combobox), renderer,
  //                                 ACE_TEXT_ALWAYS_CHAR("text"), 0);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combobox), renderer,
                                  ACE_TEXT_ALWAYS_CHAR ("text"), 0,
                                  NULL);
  list = gtk_list_store_new (1, G_TYPE_STRING);
  if (!list)
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to gtk_list_store_new(): \"%m\", aborting\n")));

    // clean up
    g_object_unref (G_OBJECT (data_p->savedStateFilter));
    g_object_unref (G_OBJECT (data_p->entityFilter));
    g_object_unref (G_OBJECT (data_p->mapFilter));
    g_object_unref (G_OBJECT ((*iterator).second.second));

    return FALSE; // G_SOURCE_REMOVE
  } // end IF
  gtk_tree_sortable_set_sort_func (GTK_TREE_SORTABLE (list),
                                   0,
                                   (GtkTreeIterCompareFunc)combobox_sort_function,
                                   GINT_TO_POINTER (0),
                                   (GDestroyNotify)NULL);
  gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (list),
                                        0,
                                        GTK_SORT_ASCENDING);
  gtk_combo_box_set_model (combobox, GTK_TREE_MODEL (list));
  g_object_unref (G_OBJECT (list));
  if (::load_files (REPOSITORY_ENGINESTATE, list))
    gtk_widget_set_sensitive (GTK_WIDGET (combobox), TRUE);

  GtkHBox* hbox =
      GTK_HBOX (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_HBOX_MAIN_NAME)));
  ACE_ASSERT (hbox);
  GtkFrame* frame =
      GTK_FRAME (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_FRAME_CHARACTER_NAME)));
  ACE_ASSERT (frame);
  g_object_ref (GTK_WIDGET (frame));
  gtk_widget_unparent (GTK_WIDGET (frame));
  gtk_box_pack_start (GTK_BOX (hbox),
                      GTK_WIDGET (frame),
                      FALSE, // expand
                      FALSE, // fill
                      0);    // padding

  GtkTreeView* tree_view =
      GTK_TREE_VIEW (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_TREEVIEW_FEATS_NAME)));
  ACE_ASSERT (tree_view);
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (tree_view,
                                               -1,
                                               ACE_TEXT_ALWAYS_CHAR ("Text"),
                                               renderer,
                                               ACE_TEXT_ALWAYS_CHAR ("text"), 0,
//                                               ACE_TEXT_ALWAYS_CHAR ("value"), 1,
                                               NULL);
  GtkTreeSelection* tree_selection_p = gtk_tree_view_get_selection (tree_view);
  ACE_ASSERT (tree_selection_p);
  gtk_tree_selection_set_mode (tree_selection_p,
                               GTK_SELECTION_MULTIPLE);
  g_signal_connect (G_OBJECT (tree_selection_p),
                    ACE_TEXT_ALWAYS_CHAR ("changed"),
                    G_CALLBACK (treeview_feats_selection_changed_cb),
                    userData_in);

  tree_view =
      GTK_TREE_VIEW (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_TREEVIEW_SKILLS_NAME)));
  ACE_ASSERT (tree_view);
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (tree_view,
                                               -1,
                                               ACE_TEXT_ALWAYS_CHAR ("Text"),
                                               renderer,
                                               ACE_TEXT_ALWAYS_CHAR ("text"), 0,
                                               NULL);
  renderer = gtk_cell_renderer_text_new ();
  GValue value_s = G_VALUE_INIT;
  g_value_init (&value_s, G_TYPE_BOOLEAN);
  g_value_set_boolean (&value_s, TRUE);
  g_object_set_property (G_OBJECT (renderer),
                         ACE_TEXT_ALWAYS_CHAR ("editable"),
                         &value_s);
  g_value_unset (&value_s);
  g_signal_connect (G_OBJECT (renderer),
                    ACE_TEXT_ALWAYS_CHAR ("editing-started"),
                    G_CALLBACK (cellrenderer_skills_editing_started_cb),
                    userData_in);
  gtk_tree_view_insert_column_with_attributes (tree_view,
                                               -1,
                                               ACE_TEXT_ALWAYS_CHAR ("Value"),
                                               renderer,
                                               ACE_TEXT_ALWAYS_CHAR ("text"), 2,
                                               NULL);
  tree_selection_p = gtk_tree_view_get_selection (tree_view);
  ACE_ASSERT (tree_selection_p);
  gtk_tree_selection_set_mode (tree_selection_p,
                               GTK_SELECTION_MULTIPLE);
  g_signal_connect (G_OBJECT (tree_selection_p),
                    ACE_TEXT_ALWAYS_CHAR ("changed"),
                    G_CALLBACK (treeview_skills_selection_changed_cb),
                    userData_in);

  tree_view =
      GTK_TREE_VIEW (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_TREEVIEW_SPELLS_NAME)));
  ACE_ASSERT (tree_view);
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (tree_view,
                                               -1,
                                               ACE_TEXT_ALWAYS_CHAR ("Text"),
                                               renderer,
                                               ACE_TEXT_ALWAYS_CHAR ("text"), 0,
//                                               ACE_TEXT_ALWAYS_CHAR ("value"), 1,
                                               NULL);
  tree_selection_p = gtk_tree_view_get_selection (tree_view);
  ACE_ASSERT (tree_selection_p);
  gtk_tree_selection_set_mode (tree_selection_p,
                               GTK_SELECTION_MULTIPLE);
  g_signal_connect (G_OBJECT (tree_selection_p),
                    ACE_TEXT_ALWAYS_CHAR ("changed"),
                    G_CALLBACK (treeview_spells_selection_changed_cb),
                    userData_in);

 // step4: (auto-)connect signals/slots
 // *NOTE*: glade_xml_signal_autoconnect doesn't work reliably
 //glade_xml_signal_autoconnect(xml);
 // step4a: connect default signals
 // *NOTE*: glade_xml_signal_connect_data doesn't work reliably
 //   glade_xml_signal_connect_data(xml,
 //                                 ACE_TEXT_ALWAYS_CHAR("properties_activated_GTK_cb"),
 //                                 G_CALLBACK(properties_activated_GTK_cb),
 //                                 userData_p);
  gtk_builder_connect_signals ((*iterator).second.second,
                               userData_in);

  g_signal_connect (filechooser_dialog,
                    ACE_TEXT_ALWAYS_CHAR ("response"),
                    G_CALLBACK (gtk_widget_hide),
                    &filechooser_dialog);
  g_signal_connect (about_dialog,
                    ACE_TEXT_ALWAYS_CHAR ("response"),
                    G_CALLBACK (gtk_widget_hide),
                    &about_dialog);
  g_signal_connect (main_entry_dialog,
                    ACE_TEXT_ALWAYS_CHAR ("response"),
                    G_CALLBACK (gtk_widget_hide),
                    &main_entry_dialog);
  g_signal_connect (main_dialog,
                    ACE_TEXT_ALWAYS_CHAR ("destroy"),
                    G_CALLBACK (quit_clicked_GTK_cb),
//                    G_CALLBACK(gtk_widget_destroyed),
//                    &main_dialog,
                    userData_in);

  // step4b: connect custom signals

 //   // step5: use correct screen
 //   if (parentWidget_in)
 //     gtk_window_set_screen(GTK_WINDOW(dialog),
 //                           gtk_widget_get_screen(const_cast<GtkWidget*>(//parentWidget_in)));

  // step6: draw main dialog
  gtk_widget_show_all (main_dialog);

  // activate first repository entry (if any)
  combobox =
      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
  ACE_ASSERT (combobox);
  if (gtk_widget_is_sensitive (GTK_WIDGET (combobox)))
    gtk_combo_box_set_active (combobox, 0);
  combobox =
      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_COMBOBOX_MAP_NAME)));
  ACE_ASSERT (combobox);
  if (gtk_widget_is_sensitive (GTK_WIDGET (combobox)))
    gtk_combo_box_set_active (combobox, 0);

  return FALSE; // G_SOURCE_REMOVE
}

gboolean
idle_finalize_UI_cb (gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::idle_finalize_UI_cb"));

//  struct RPG_Client_GTK_CBData* data_p =
//      static_cast<struct RPG_Client_GTK_CBData*> (userData_in);

//       // sanity check(s)
//  ACE_ASSERT (data_p);

//  Common_UI_GTK_BuildersConstIterator_t iterator =
//      data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
//  // sanity check(s)
//  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

//  GtkWidget* widget =
//      GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
//                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_SERVER_PART_NAME)));
//  ACE_ASSERT (widget);
//  // raise dialog window
//  GdkWindow* toplevel = gtk_widget_get_parent_window (widget);
//  ACE_ASSERT (toplevel);
//  gdk_window_deiconify (toplevel);
//  // emit a signal...
//  gtk_button_clicked (GTK_BUTTON (widget));

  gtk_main_quit ();

  return FALSE; // G_SOURCE_REMOVE
}

gboolean
idle_raise_UI_cb (gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::idle_raise_UI_cb"));

  // sanity check(s)
  struct RPG_Client_GTK_CBData* data_p =
      static_cast<struct RPG_Client_GTK_CBData*> (userData_in);
  ACE_ASSERT (data_p);
  Common_UI_GTK_BuildersConstIterator_t iterator =
      data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  // raise dialog window
  GtkWidget* widget =
      GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_MAIN_NAME)));
  ACE_ASSERT (widget);
  GdkWindow* toplevel = gtk_widget_get_window (widget);
  ACE_ASSERT (toplevel);
  gdk_window_deiconify (toplevel);
  gdk_window_show (toplevel);

  return FALSE; // G_SOURCE_REMOVE
}

gboolean
idle_leave_game_cb (gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::idle_leave_game_cb"));

  // sanity check(s)
  struct RPG_Client_GTK_CBData* data_p =
      static_cast<struct RPG_Client_GTK_CBData*> (userData_in);
  ACE_ASSERT (data_p);
  Common_UI_GTK_BuildersConstIterator_t iterator =
      data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  GtkToggleButton* toggle_button_p =
      GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                 ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_TOGGLEBUTTON_SERVER_JOIN_PART_NAME)));
  ACE_ASSERT (toggle_button_p);
  if (gtk_toggle_button_get_active (toggle_button_p))
    gtk_toggle_button_set_active (toggle_button_p, FALSE);

  return FALSE; // G_SOURCE_REMOVE
}

gboolean
idle_level_up_cb (gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::idle_level_up_cb"));

  // sanity check(s)
  struct RPG_Client_GTK_CBData* data_p =
      static_cast<struct RPG_Client_GTK_CBData*> (userData_in);
  ACE_ASSERT (data_p);
  Common_UI_GTK_BuildersConstIterator_t iterator =
      data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  GtkButton* button_p =
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_LEVELUP_NAME)));
  ACE_ASSERT (button_p);
  gtk_widget_set_sensitive (GTK_WIDGET (button_p), TRUE);
  gtk_button_clicked (button_p);

  return FALSE; // G_SOURCE_REMOVE
}

gboolean
idle_update_profile_cb (gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::idle_update_profile_cb"));

  // sanity check(s)
  struct RPG_Client_GTK_CBData* data_p =
      static_cast<struct RPG_Client_GTK_CBData*> (userData_in);
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->entity.character);
  ACE_ASSERT (data_p->entity.character->isPlayerCharacter ());
  RPG_Player* player = static_cast<RPG_Player*> (data_p->entity.character);
  ACE_ASSERT (data_p->UIState);
  Common_UI_GTK_BuildersConstIterator_t iterator =
      data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  ::update_character_profile (*player, (*iterator).second.second);

  return FALSE; // G_SOURCE_REMOVE
}

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT gint
about_clicked_GTK_cb (GtkWidget* widget_in,
                      gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::about_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  struct RPG_Client_GTK_CBData* data_p =
    static_cast<struct RPG_Client_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GTK_BuildersConstIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  // retrieve about dialog handle
  GtkWidget* about_dialog =
      GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_ABOUT_NAME)));
  ACE_ASSERT(about_dialog);
  if (!about_dialog)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_builder_get_object(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_ABOUT_NAME)));
    return TRUE; // propagate
  } // end IF

  // draw it
  if (!gtk_widget_get_visible (about_dialog))
    gtk_widget_show_all (about_dialog);

  return FALSE;
}

G_MODULE_EXPORT gint
quit_clicked_GTK_cb (GtkWidget* widget_in,
                     gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::quit_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  ACE_UNUSED_ARG (userData_in);

  // trigger SDL event loop
  union SDL_Event sdl_event;
  sdl_event.type = SDL_QUIT;
  if (SDL_PushEvent (&sdl_event))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_PushEvent(): \"%s\", continuing\n"),
                ACE_TEXT (SDL_GetError ())));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("leaving GTK...\n")));

  return FALSE;
}

G_MODULE_EXPORT gint
create_character_clicked_GTK_cb (GtkWidget* widget_in,
                                 gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::create_character_clicked_GTK_cb"));

  struct RPG_Client_GTK_CBData* data_p =
    static_cast<struct RPG_Client_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GTK_BuildersConstIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  // clean up
  if (data_p->entity.character)
  {
    // *TODO*: save existing character first ?
    delete data_p->entity.character;
    data_p->entity.character = NULL;
  } // end IF
  data_p->entity.position =
    std::make_pair (std::numeric_limits<unsigned int>::max (),
                    std::numeric_limits<unsigned int>::max ());
  data_p->entity.modes.clear ();
  data_p->entity.actions.clear ();
  data_p->entity.is_spawned = false;

  data_p->entity = RPG_Engine_Common_Tools::createEntity ();
  ACE_ASSERT (data_p->entity.character);

  // update entity profile widgets
  ::update_entity_profile (data_p->entity,
                           (*iterator).second.second);

  // make character display frame sensitive (if it's not already)
  GtkFrame* character_frame =
    GTK_FRAME (gtk_builder_get_object ((*iterator).second.second,
                                       ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_FRAME_CHARACTER_NAME)));
  ACE_ASSERT (character_frame);
  gtk_widget_set_sensitive (GTK_WIDGET (character_frame), TRUE);

  // make drop button sensitive (if it's not already)
  GtkButton* button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_DROP_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

  // make save button sensitive (if it's not already)
  button = GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_STORE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

  // make load button insensitive (if it's not already)
  button = GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_LOAD_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

  // make join button sensitive (if appropriate)
  GtkComboBox* repository_combobox =
    GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_COMBOBOX_MAP_NAME)));
  ACE_ASSERT (repository_combobox);
  if (gtk_combo_box_get_active (repository_combobox) != -1)
  {
    GtkToggleButton* button_2 =
      GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                 ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_TOGGLEBUTTON_SERVER_JOIN_PART_NAME)));
    ACE_ASSERT (button_2);
    gtk_widget_set_sensitive (GTK_WIDGET (button_2), TRUE);
  } // end IF

  // make this insensitive
  gtk_widget_set_sensitive (widget_in, FALSE);

  return FALSE;
}

G_MODULE_EXPORT gint
drop_character_clicked_GTK_cb (GtkWidget* widget_in,
                               gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::drop_character_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);

  // sanity check(s)
  struct RPG_Client_GTK_CBData* data_p =
    static_cast<struct RPG_Client_GTK_CBData*> (userData_in);
  ACE_ASSERT (data_p);
  Common_UI_GTK_BuildersConstIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());
  ACE_ASSERT (data_p->entity.character);

  // assemble target filename
  std::string profiles_directory =
    RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
  std::string filename = profiles_directory;
  filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  filename += data_p->entity.character->getName ();
  filename += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);
  Common_File_Tools::deleteFile (filename);

  delete data_p->entity.character;
  data_p->entity.character = NULL;
  data_p->entity.position =
    std::make_pair (std::numeric_limits<unsigned int>::max (),
                    std::numeric_limits<unsigned int>::max ());
  data_p->entity.modes.clear ();
  data_p->entity.actions.clear ();
  data_p->entity.is_spawned = false;

  // reset profile widgets
  ::reset_character_profile ((*iterator).second.second);
  GtkImage* image =
    GTK_IMAGE (gtk_builder_get_object ((*iterator).second.second,
                                       ACE_TEXT_ALWAYS_CHAR ("image_sprite")));
  ACE_ASSERT (image);
  gtk_image_clear (image);

  // make character display frame insensitive (if it's not already)
  GtkFrame* character_frame =
    GTK_FRAME (gtk_builder_get_object ((*iterator).second.second,
                                       ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_FRAME_CHARACTER_NAME)));
  ACE_ASSERT (character_frame);
  gtk_widget_set_sensitive (GTK_WIDGET (character_frame), FALSE);

  // make create button sensitive (if it's not already)
  GtkButton* button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_CREATE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

  // make save button insensitive (if it's not already)
  button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_STORE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

  // make load button sensitive (if it's not already)
  button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_LOAD_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

  // make join button insensitive (if it's not already)
  GtkToggleButton* button_2 =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_TOGGLEBUTTON_SERVER_JOIN_PART_NAME)));
  ACE_ASSERT (button_2);
  gtk_widget_set_sensitive (GTK_WIDGET (button_2), FALSE);

  // make this insensitive
  //gtk_widget_set_sensitive (widget_in, FALSE);

  // make equip button insensitive (if it's not already)
  button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_EQUIP_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

  // make rest button insensitive (if it's not already)
  button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_REST_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

  // reset available characters combobox
  GtkComboBox* combobox =
      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
  ACE_ASSERT (combobox);
  g_signal_emit_by_name (G_OBJECT (combobox),
                         ACE_TEXT_ALWAYS_CHAR ("changed"));

  return FALSE;
}

G_MODULE_EXPORT gint
load_character_clicked_GTK_cb (GtkWidget* widget_in,
                               gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::load_character_clicked_GTK_cb"));

  struct RPG_Client_GTK_CBData* data_p =
    static_cast<struct RPG_Client_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->entityFilter);
  ACE_ASSERT (data_p->levelEngine);

  Common_UI_GTK_BuildersConstIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  // step1a: retrieve file chooser dialog handle
  GtkFileChooserDialog* filechooser_dialog =
      GTK_FILE_CHOOSER_DIALOG (gtk_builder_get_object ((*iterator).second.second,
                                                       ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_FILECHOOSER_NAME)));
  ACE_ASSERT (filechooser_dialog);

  // step1b: setup chooser dialog
  std::string profiles_directory =
      RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (filechooser_dialog),
                                       ACE_TEXT (profiles_directory.c_str ()));
  // *TODO*: this crashes on WIN
  gtk_file_chooser_set_filter (GTK_FILE_CHOOSER (filechooser_dialog),
                               data_p->entityFilter);

  // step1c: run chooser dialog
  gint response_id = gtk_dialog_run (GTK_DIALOG (filechooser_dialog));
  if (response_id == -1)
    return FALSE;

  // retrieve selected filename
  std::string filename =
      gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_dialog));

  // clean up
  if (data_p->entity.character)
  {
    delete data_p->entity.character;
    data_p->entity.character = NULL;
    data_p->entity.position =
        std::make_pair (std::numeric_limits<unsigned int>::max (),
                        std::numeric_limits<unsigned int>::max ());
    data_p->entity.modes.clear ();
    data_p->entity.actions.clear ();
    data_p->entity.is_spawned = false;
  } // end IF

  // load player profile
  RPG_Character_Conditions_t condition;
  condition.insert (CONDITION_NORMAL);
  short int hitpoints = std::numeric_limits<short int>::max ();
  RPG_Magic_Spells_t spells;
  data_p->entity.character = RPG_Player::load (filename,
                                               data_p->schemaRepository,
                                               condition,
                                               hitpoints,
                                               spells);
  ACE_ASSERT (data_p->entity.character);

  // update entity profile widgets
  ::update_entity_profile (data_p->entity,
                           (*iterator).second.second);

  // if necessary, update starting position
  if (data_p->entity.position ==
      std::make_pair (std::numeric_limits<unsigned int>::max (),
                      std::numeric_limits<unsigned int>::max ()))
    data_p->entity.position = data_p->levelEngine->getStartPosition ();

  // make character display frame sensitive (if it's not already)
  GtkFrame* character_frame =
      GTK_FRAME (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_FRAME_CHARACTER_NAME)));
  ACE_ASSERT (character_frame);
  gtk_widget_set_sensitive (GTK_WIDGET (character_frame), TRUE);

  // make create button insensitive (if it's not already)
  GtkButton* button =
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_CREATE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

  // make drop button sensitive (if it's not already)
  //button = GTK_BUTTON(gtk_builder_get_object((*iterator).second.second,
  //                                         ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_DROP_NAME)));
  //ACE_ASSERT(button);
  //gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  // make load button insensitive (if it's not already)
  gtk_widget_set_sensitive (widget_in, FALSE);

  // make save button insensitive (if it's not already)
  button = 
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_STORE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

  // make join button sensitive (if appropriate)
  GtkComboBox* repository_combobox =
      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_COMBOBOX_MAP_NAME)));
  ACE_ASSERT(repository_combobox);
  if (gtk_combo_box_get_active (repository_combobox) != -1)
  {
    GtkToggleButton* button_2 =
      GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                 ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_TOGGLEBUTTON_SERVER_JOIN_PART_NAME)));
    ACE_ASSERT (button_2);
    gtk_widget_set_sensitive (GTK_WIDGET (button_2), TRUE);
  } // end IF

  return FALSE;
}

G_MODULE_EXPORT gint
save_character_clicked_GTK_cb(GtkWidget* widget_in,
                              gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::save_character_clicked_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  struct RPG_Client_GTK_CBData* data_p =
    static_cast<struct RPG_Client_GTK_CBData*>(userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->entity.character);

  Common_UI_GTK_BuildersConstIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  // assemble target filename
  std::string profiles_directory =
      RPG_Player_Common_Tools::getPlayerProfilesDirectory();
  std::string filename = profiles_directory;
  filename += ACE_DIRECTORY_SEPARATOR_STR;
  filename += data_p->entity.character->getName ();
  filename += ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_PROFILE_EXT);

  RPG_Player* player_p = NULL;
  try {
    player_p = dynamic_cast<RPG_Player*> (data_p->entity.character);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in dynamic_cast<RPG_Player*>(%@), aborting\n"),
                data_p->entity.character));
    return FALSE;
  }
  if (!player_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dynamic_cast<RPG_Player*>(%@), aborting\n"),
                data_p->entity.character));
    return FALSE;
  }
  if (!player_p->save (filename))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Player::save(\"%s\"), continuing\n"),
                ACE_TEXT (filename.c_str ())));

  // make create button sensitive (if it's not already)
  GtkButton* button =
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_CREATE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

  // make drop button insensitive (if it's not already)
  //button =
  //  GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
  //                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_DROP_NAME)));
  //ACE_ASSERT (button);
  //gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

  // make save button insensitive
  gtk_widget_set_sensitive (widget_in, FALSE);

  return FALSE;
}

gint
character_repository_combobox_changed_GTK_cb (GtkWidget* widget_in,
                                              gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::character_repository_combobox_changed_GTK_cb"));

  // sanity check(s)
  ACE_ASSERT (widget_in);
  struct RPG_Client_GTK_CBData* data_p =
    static_cast<struct RPG_Client_GTK_CBData*> (userData_in);
  ACE_ASSERT (data_p);
  Common_UI_GTK_BuildersConstIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  // retrieve active item
  std::string active_item;
  GtkTreeIter selected;
  GtkTreeModel* model = NULL;
  GValue value;
  const gchar* text = NULL;
  GtkVBox* vbox =
    GTK_VBOX (gtk_builder_get_object ((*iterator).second.second,
                                    ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_VBOX_TOOLS_NAME)));
  ACE_ASSERT (vbox);
  GtkFrame* frame =
    GTK_FRAME (gtk_builder_get_object ((*iterator).second.second,
                                     ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_FRAME_CHARACTER_NAME)));
  ACE_ASSERT (frame);
  if (!gtk_combo_box_get_active_iter (GTK_COMBO_BOX (widget_in), &selected))
  {
    // *WARNING*: refreshing the combobox triggers removal of items
    // which also generates this signal...

    // clean up
    ::reset_character_profile ((*iterator).second.second);
    GtkImage* image =
      GTK_IMAGE (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR ("image_sprite")));
    ACE_ASSERT (image);
    gtk_image_clear (image);

    // desensitize tools vbox
    gtk_widget_set_sensitive (GTK_WIDGET (vbox), FALSE);

    // remove character frame widget
    gtk_widget_set_sensitive (GTK_WIDGET (frame), FALSE);
    gtk_widget_hide (GTK_WIDGET (frame));

    // make drop button insensitive (if it's not already)
    GtkButton* button =
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_DROP_NAME)));
    ACE_ASSERT (button);
    gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

    return FALSE;
  } // end IF
  model = gtk_combo_box_get_model (GTK_COMBO_BOX (widget_in));
  ACE_ASSERT (model);
  ACE_OS::memset (&value, 0, sizeof(value));
  gtk_tree_model_get_value (model, &selected,
                            0, &value);
  text = g_value_get_string (&value);
  // sanity check
  ACE_ASSERT (text);
  active_item = text;
  g_value_unset (&value);

  // clean up
  if (data_p->entity.character)
  {
    delete data_p->entity.character;
    data_p->entity.character = NULL;
    data_p->entity.position =
        std::make_pair (std::numeric_limits<unsigned int>::max (),
                        std::numeric_limits<unsigned int>::max ());
    data_p->entity.modes.clear ();
    data_p->entity.actions.clear ();
    data_p->entity.is_spawned = false;
  } // end IF

  // construct filename
  std::string profiles_directory =
      RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
  std::string filename = profiles_directory;
  filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  filename += active_item;
  filename += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);

  // load player profile
  RPG_Character_Conditions_t condition;
  condition.insert (CONDITION_NORMAL);
  short int hitpoints = std::numeric_limits<short int>::max ();
  RPG_Magic_Spells_t spells;
  data_p->entity.character = RPG_Player::load (filename,
                                               data_p->schemaRepository,
                                               condition,
                                               hitpoints,
                                               spells);
  if (!data_p->entity.character)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Player::load(\"%s\"), aborting\n"),
                ACE_TEXT (filename.c_str ())));
    return FALSE;
  } // end IF

  // update entity profile widgets
  ::update_entity_profile (data_p->entity,
                           (*iterator).second.second);

  // sensitize tools vbox
  gtk_widget_set_sensitive (GTK_WIDGET (vbox), TRUE);
  // make character frame visible/sensitive (if it's not already)
  gtk_widget_show (GTK_WIDGET (frame));
  gtk_widget_set_sensitive (GTK_WIDGET (frame), TRUE);

  // make join button sensitive IFF player is not disabled
  GtkToggleButton* button_2 =
      GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                 ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_TOGGLEBUTTON_SERVER_JOIN_PART_NAME)));
  ACE_ASSERT(button_2);
  if (!RPG_Engine_Common_Tools::isCharacterDisabled (data_p->entity.character))
    gtk_widget_set_sensitive (GTK_WIDGET(button_2), TRUE);

  // make drop button sensitive (if it's not already)
  GtkButton* button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_DROP_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

  // make equip button sensitive (if it's not already)
  button =
    GTK_BUTTON(gtk_builder_get_object ((*iterator).second.second,
                                       ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_EQUIP_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

  return FALSE;
}

G_MODULE_EXPORT gint
character_repository_button_clicked_GTK_cb (GtkWidget* widget_in,
                                            gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::character_repository_button_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  struct RPG_Client_GTK_CBData* data_p =
    static_cast<struct RPG_Client_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GTK_BuildersConstIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  // retrieve tree model
  GtkComboBox* repository_combobox =
      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
  ACE_ASSERT(repository_combobox);
  GtkTreeModel* model = gtk_combo_box_get_model (repository_combobox);
  ACE_ASSERT(model);

  // re-load profile data
  unsigned int num_entries = ::load_files (REPOSITORY_PROFILES,
                                           GTK_LIST_STORE (model));

  // set sensitive as appropriate
  GtkFrame* character_frame =
      GTK_FRAME (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_FRAME_CHARACTER_NAME)));
  ACE_ASSERT (character_frame);

  // ... sensitize/activate widgets as appropriate
  if (num_entries)
  {
    gtk_widget_set_sensitive (GTK_WIDGET (repository_combobox), TRUE);
    gtk_combo_box_set_active (repository_combobox, 0);
  } // end IF
  else
  {
    gtk_widget_set_sensitive (GTK_WIDGET (repository_combobox), FALSE);
    gtk_widget_set_sensitive (GTK_WIDGET (character_frame), FALSE);

    // make create button sensitive (if it's not already)
    GtkButton* button =
        GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_CREATE_NAME)));
    ACE_ASSERT (button);
    gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

    // make load button sensitive (if it's not already)
    button =
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_LOAD_NAME)));
    ACE_ASSERT (button);
    gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);
  } // end ELSE

  return FALSE;
}

G_MODULE_EXPORT gint
create_map_clicked_GTK_cb (GtkWidget* widget_in,
                           gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::create_map_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  struct RPG_Client_GTK_CBData* data_p =
    static_cast<struct RPG_Client_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->levelEngine);
  ACE_ASSERT (data_p->clientEngine);

  Common_UI_GTK_BuildersConstIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  // step1: get map name...
  // step1a: setup entry dialog
  GtkEntry* main_entry_dialog_entry =
    GTK_ENTRY (gtk_builder_get_object ((*iterator).second.second,
                                       ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_ENTRY_NAME)));
  ACE_ASSERT (main_entry_dialog_entry);
  gtk_entry_buffer_delete_text (gtk_entry_get_buffer (main_entry_dialog_entry),
                                0, -1);
  // enforce sane values
  gtk_entry_set_max_length (main_entry_dialog_entry,
                            RPG_ENGINE_LEVEL_NAME_MAX_LENGTH);
//   gtk_entry_set_width_chars(main_entry_dialog_entry,
//                             -1); // reset to default
  gtk_entry_set_text (main_entry_dialog_entry,
                      ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_DEF_NAME));
  gtk_editable_select_region (GTK_EDITABLE (main_entry_dialog_entry),
                              0, -1);
  // step1b: retrieve entry dialog handle
  GtkDialog* main_entry_dialog =
    GTK_DIALOG (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_ENTRY_NAME)));
  ACE_ASSERT(main_entry_dialog);
  // step1c: run entry dialog
  gint response_id = gtk_dialog_run (main_entry_dialog);
  if (response_id == -1)
    return FALSE;

  // step1c: convert UTF8 --> locale
  const gchar* text = gtk_entry_get_text (main_entry_dialog_entry);
  ACE_ASSERT (text);
  gchar* converted_text = NULL;
  GError* conversion_error = NULL;
  converted_text = g_locale_from_utf8 (text,               // text
                                       -1,                 // length in bytes (-1: \0-terminated)
                                       NULL,               // bytes read (don't care)
                                       NULL,               // bytes written (don't care)
                                       &conversion_error); // return value: error
  if (conversion_error)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to convert string: \"%s\", continuing\n"),
                ACE_TEXT (conversion_error->message)));

    // clean up
    g_error_free(conversion_error);
  } // end IF
  else
   data_p->levelMetadata.name = converted_text;
  // clean up
  g_free (converted_text);

  // step2: create new (random) map
  struct RPG_Engine_LevelData level;
  RPG_Engine_Level::random (data_p->levelMetadata,
                            data_p->mapConfiguration,
                            level);

  // step3: assign new map to level engine
  if (data_p->levelEngine->isRunning ())
    data_p->levelEngine->stop ();
  data_p->levelEngine->set (level);
  data_p->levelEngine->start ();

  // make "this" insensitive
  gtk_widget_set_sensitive(widget_in, FALSE);

  // make drop button sensitive (if it's not already)
  GtkButton* button =
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_DROP_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

  // make save button sensitive (if it's not already)
  button =
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_STORE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

  // make load button insensitive (if it's not already)
  button =
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_MAP_LOAD_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

  // make join button sensitive (if appropriate)
  GtkComboBox* repository_combobox =
    GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
  ACE_ASSERT (repository_combobox);
  if (gtk_combo_box_get_active (repository_combobox) != -1)
  {
    GtkToggleButton* button_2 =
      GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                 ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_TOGGLEBUTTON_SERVER_JOIN_PART_NAME)));
    ACE_ASSERT (button_2);
    gtk_widget_set_sensitive (GTK_WIDGET (button_2), TRUE);
  } // end IF

  return FALSE;
}

G_MODULE_EXPORT gint
drop_map_clicked_GTK_cb (GtkWidget* widget_in,
                         gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::drop_map_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  struct RPG_Client_GTK_CBData* data_p =
    static_cast<struct RPG_Client_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->levelEngine);
  ACE_ASSERT (data_p->clientEngine);

  Common_UI_GTK_BuildersConstIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  struct RPG_Engine_LevelData level;
  level.metadata = data_p->levelMetadata;
  level.map.start = std::make_pair (std::numeric_limits<unsigned int>::max (),
                                    std::numeric_limits<unsigned int>::max ());
  level.map.seeds.clear ();
  level.map.plan.size_x = 0;
  level.map.plan.size_y = 0;
  level.map.plan.unmapped.clear ();
  level.map.plan.walls.clear ();
  level.map.plan.doors.clear ();
  level.map.plan.rooms_are_square = false;

  data_p->levelEngine->stop ();
  // assign empty map to level engine
  data_p->levelEngine->set (level);

  // make "this" insensitive
  gtk_widget_set_sensitive (widget_in, FALSE);

  // make create_map button sensitive (if it's not already)
  GtkButton* button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_MAP_CREATE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

  // make map_load button sensitive (if it's not already)
  button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_MAP_LOAD_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

  // make join button insensitive (if it's not already)
  GtkToggleButton* button_2 =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_TOGGLEBUTTON_SERVER_JOIN_PART_NAME)));
  ACE_ASSERT (button_2);
  gtk_widget_set_sensitive (GTK_WIDGET (button_2), FALSE);

  return FALSE;
}

G_MODULE_EXPORT gint
load_map_clicked_GTK_cb (GtkWidget* widget_in,
                         gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::load_map_clicked_GTK_cb"));

  struct RPG_Client_GTK_CBData* data_p =
    static_cast<struct RPG_Client_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->entityFilter);
  ACE_ASSERT (data_p->levelEngine);

  Common_UI_GTK_BuildersConstIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  // step1a: retrieve file chooser dialog handle
  GtkFileChooserDialog* filechooser_dialog =
    GTK_FILE_CHOOSER_DIALOG (gtk_builder_get_object ((*iterator).second.second,
                                                     ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_FILECHOOSER_NAME)));
  ACE_ASSERT (filechooser_dialog);

  // step1b: setup chooser dialog
  std::string maps_directory = RPG_Map_Common_Tools::getMapsDirectory ();
  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(filechooser_dialog),
                                       ACE_TEXT (maps_directory.c_str ()));
  gtk_file_chooser_set_filter (GTK_FILE_CHOOSER (filechooser_dialog),
                               data_p->mapFilter);

  // step1c: run chooser dialog
  gint response_id = gtk_dialog_run (GTK_DIALOG (filechooser_dialog));
  if (response_id == -1)
    return FALSE;

  // retrieve selected filename
  std::string filename =
    gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_dialog));

  // load level
  struct RPG_Engine_LevelData level;
  if (!RPG_Engine_Level::load (filename,
                               data_p->schemaRepository,
                               level))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Engine_Level::load(\"%s\"), aborting"),
                ACE_TEXT (filename.c_str ())));
    return FALSE;
  } // end IF
  if (data_p->levelEngine->isRunning ())
    data_p->levelEngine->stop ();
  data_p->levelEngine->set (level);
  data_p->levelEngine->start ();

  // make create button insensitive (if it's not already)
  GtkButton* button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_MAP_CREATE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

  // make map_drop button sensitive (if it's not already)
  button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_MAP_DROP_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

  // make load button insensitive (if it's not already)
  gtk_widget_set_sensitive (widget_in, FALSE);

  // make map_save button insensitive (if it's not already)
  button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_MAP_STORE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

  // make join button sensitive (if appropriate)
  GtkComboBox* repository_combobox =
    GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
  ACE_ASSERT (repository_combobox);
  if (gtk_combo_box_get_active (repository_combobox) != -1)
  {
    GtkToggleButton* button_2 =
      GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                 ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_TOGGLEBUTTON_SERVER_JOIN_PART_NAME)));
    ACE_ASSERT (button_2);
    gtk_widget_set_sensitive (GTK_WIDGET (button_2), TRUE);
  } // end IF

  return FALSE;
}

G_MODULE_EXPORT gint
save_map_clicked_GTK_cb (GtkWidget* widget_in,
                         gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::save_map_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  struct RPG_Client_GTK_CBData* data_p =
    static_cast<struct RPG_Client_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->levelEngine);

  Common_UI_GTK_BuildersConstIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  std::string filename = RPG_Map_Common_Tools::getMapsDirectory ();
  filename += ACE_DIRECTORY_SEPARATOR_STR;
  filename += data_p->levelEngine->getMetaData (true).name;
  filename += ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_FILE_EXT);
  data_p->levelEngine->save (filename);

  // make create button sensitive (if it's not already)
  GtkButton* button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_MAP_CREATE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

  // make drop button insensitive (if it's not already)
  button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_MAP_DROP_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

  // make save button insensitive
  gtk_widget_set_sensitive (widget_in, FALSE);

  return FALSE;
}

G_MODULE_EXPORT gint
map_repository_combobox_changed_GTK_cb (GtkWidget* widget_in,
                                        gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::map_repository_combobox_changed_GTK_cb"));

  struct RPG_Client_GTK_CBData* data_p =
    static_cast<struct RPG_Client_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (widget_in);
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->levelEngine);

  Common_UI_GTK_BuildersConstIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  // retrieve active item
  std::string active_item;
  GtkTreeIter selected;
  GtkTreeModel* model = NULL;
  GValue value;
  const gchar* text = NULL;
  if (!gtk_combo_box_get_active_iter (GTK_COMBO_BOX (widget_in), &selected))
  {
    // *WARNING*: refreshing the combobox triggers removal of items
    // which also generates this signal...
    return FALSE;
  } // end IF
  model = gtk_combo_box_get_model (GTK_COMBO_BOX (widget_in));
  ACE_ASSERT (model);
  ACE_OS::memset (&value, 0, sizeof(value));
  gtk_tree_model_get_value (model, &selected,
                            0, &value);
  text = g_value_get_string (&value);
  // sanity check
  ACE_ASSERT (text);
  active_item = text;
  g_value_unset (&value);

  // construct filename
  std::string filename = RPG_Map_Common_Tools::getMapsDirectory ();
  filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  filename += active_item;
  filename += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT);

  // load level
  struct RPG_Engine_LevelData level;
  if (!RPG_Engine_Level::load (filename,
                               data_p->schemaRepository,
                               level))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Engine_Level::load(\"%s\"), aborting"),
                ACE_TEXT (filename.c_str ())));
    return FALSE;
  } // end IF
  bool engine_was_running = data_p->levelEngine->isRunning ();
  if (engine_was_running)
    data_p->levelEngine->stop ();
  data_p->levelEngine->set (level);
  if (engine_was_running)
    data_p->levelEngine->start ();

  // make map_save button insensitive (if it's not already)
  GtkButton* button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_MAP_STORE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

  return FALSE;
}

G_MODULE_EXPORT gint
map_repository_button_clicked_GTK_cb (GtkWidget* widget_in,
                                      gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::map_repository_button_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  struct RPG_Client_GTK_CBData* data_p =
      static_cast<struct RPG_Client_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GTK_BuildersConstIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  // retrieve tree model
  GtkComboBox* repository_combobox =
      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_MAP_NAME)));
  ACE_ASSERT (repository_combobox);
  GtkTreeModel* model = gtk_combo_box_get_model (repository_combobox);
  ACE_ASSERT (model);

  // re-load maps data
  unsigned int num_entries =
    ::load_files (REPOSITORY_MAPS, GTK_LIST_STORE (model));

  // ... sensitize/activate widgets as appropriate
  if (num_entries)
  {
    gtk_widget_set_sensitive (GTK_WIDGET (repository_combobox), TRUE);
    gtk_combo_box_set_active (repository_combobox, 0);
  } // end IF
  else
  {
    // make join button insensitive (if it's not already)
    GtkToggleButton* button =
        GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                   ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_TOGGLEBUTTON_SERVER_JOIN_PART_NAME)));
    ACE_ASSERT (button);
    gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);
  } // end ELSE

  return FALSE;
}

G_MODULE_EXPORT gint
load_state_clicked_GTK_cb (GtkWidget* widget_in,
                           gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::load_state_clicked_GTK_cb"));

  struct RPG_Client_GTK_CBData* data_p =
    static_cast<struct RPG_Client_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->levelEngine);
  ACE_ASSERT (data_p->savedStateFilter);

  Common_UI_GTK_BuildersConstIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  // step1a: retrieve file chooser dialog handle
  GtkFileChooserDialog* filechooser_dialog =
    GTK_FILE_CHOOSER_DIALOG (gtk_builder_get_object ((*iterator).second.second,
                                                     ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_FILECHOOSER_NAME)));
  ACE_ASSERT (filechooser_dialog);

  // step1b: setup chooser dialog
  std::string savedstate_directory =
    RPG_Engine_Common_Tools::getEngineStateDirectory ();
  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (filechooser_dialog),
                                       ACE_TEXT (savedstate_directory.c_str ()));
  gtk_file_chooser_set_filter (GTK_FILE_CHOOSER (filechooser_dialog),
                               data_p->savedStateFilter);

  // step1c: run chooser dialog
  gint response_id = gtk_dialog_run (GTK_DIALOG (filechooser_dialog));
  gtk_widget_hide (GTK_WIDGET (filechooser_dialog));
  if (response_id == -1)
    return FALSE;

  // retrieve selected filename
  std::string filename =
    gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_dialog));

  // load saved state
  if (!data_p->levelEngine->load (filename,
                                  data_p->schemaRepository))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Engine::load(\"%s\"), aborting"),
                ACE_TEXT (filename.c_str ())));
    return FALSE;
  } // end IF

  // make state_save button sensitive (if it is not already)
  GtkButton* button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_ENGINESTATE_STORE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

  // make join button in-sensitive (if it is not already)
  GtkToggleButton* button_2 =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_TOGGLEBUTTON_SERVER_JOIN_PART_NAME)));
  ACE_ASSERT (button_2);
  gtk_widget_set_sensitive (GTK_WIDGET (button_2), FALSE);
  //// make part button sensitive (if it is not already)
  //button =
  //  GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
  //                                    ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_SERVER_PART_NAME)));
  //ACE_ASSERT(button);
  //gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  return FALSE;
}

G_MODULE_EXPORT gint
save_state_clicked_GTK_cb (GtkWidget* widget_in,
                           gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::save_state_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  struct RPG_Client_GTK_CBData* data_p =
    static_cast<struct RPG_Client_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->levelEngine);

  Common_UI_GTK_BuildersConstIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  // get descriptor...
  // step1a: setup entry dialog
  GtkEntry* entry =
      GTK_ENTRY (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_ENTRY_NAME)));
  ACE_ASSERT (entry);
  gtk_entry_buffer_delete_text (gtk_entry_get_buffer (entry),
                                0, -1);
  // enforce sane values
  gtk_entry_set_max_length (entry,
                            RPG_ENGINE_LEVEL_NAME_MAX_LENGTH);
//   gtk_entry_set_width_chars(entry,
//                             -1); // reset to default
  gtk_entry_set_text (entry,
                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_STATE_DEF_DESCRIPTOR));
  gtk_editable_select_region (GTK_EDITABLE (entry),
                              0, -1);
  // step1b: retrieve entry dialog handle
  GtkDialog* entry_dialog =
    GTK_DIALOG (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_ENTRY_NAME)));
  ACE_ASSERT (entry_dialog);
  // step1b: run entry dialog
  gint response_id = gtk_dialog_run (entry_dialog);
  gtk_widget_hide (GTK_WIDGET (entry_dialog));
  if (response_id == -1)
    return FALSE;
  // step1c: convert UTF8 --> locale
  const gchar* text = gtk_entry_get_text (entry);
  ACE_ASSERT(text);
  gchar* converted_text = NULL;
  GError* conversion_error = NULL;
  converted_text = g_locale_from_utf8 (text,               // text
                                       -1,                 // length in bytes (-1: \0-terminated)
                                       NULL,               // bytes read (don't care)
                                       NULL,               // bytes written (don't care)
                                       &conversion_error); // return value: error
  if (conversion_error)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to convert string: \"%s\", continuing\n"),
                ACE_TEXT (conversion_error->message)));

    // clean up
    g_error_free (conversion_error);
  } // end IF
  std::string descriptor = converted_text;
  g_free (converted_text);
  if (!data_p->levelEngine->save (descriptor))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Engine::save(), continuing\n")));

  return FALSE;
}

G_MODULE_EXPORT gint
state_repository_combobox_changed_GTK_cb (GtkWidget* widget_in,
                                          gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::state_repository_combobox_changed_GTK_cb"));

  struct RPG_Client_GTK_CBData* data_p =
    static_cast<struct RPG_Client_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (widget_in);
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->levelEngine);

  Common_UI_GTK_BuildersConstIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  // retrieve active item
  std::string active_item;
  GtkTreeIter selected;
  GtkTreeModel* model = NULL;
  GValue value;
  const gchar* text = NULL;
  if (!gtk_combo_box_get_active_iter (GTK_COMBO_BOX (widget_in), &selected))
  {
    // *WARNING*: refreshing the combobox triggers removal of items
    // which also generates this signal...
    return FALSE;
  } // end IF
  model = gtk_combo_box_get_model (GTK_COMBO_BOX (widget_in));
  ACE_ASSERT (model);
  ACE_OS::memset (&value, 0, sizeof (GValue));
  gtk_tree_model_get_value (model, &selected,
                            0, &value);
  text = g_value_get_string (&value);
  // sanity check
  ACE_ASSERT (text);
  active_item = text;
  g_value_unset (&value);

  // construct filename
  std::string filename = RPG_Engine_Common_Tools::getEngineStateDirectory ();
  filename += ACE_DIRECTORY_SEPARATOR_STR;
  filename += active_item;
  filename += ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_STATE_EXT);

  // load state
  if (!data_p->levelEngine->load (filename,
                                  data_p->schemaRepository))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Engine::load(\"%s\"), aborting"),
                ACE_TEXT (filename.c_str ())));
    return FALSE;
  } // end IF

  // make state_save button sensitive (if it is not already)
  GtkButton* button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_ENGINESTATE_STORE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

  // make join button in-sensitive (if it is not already)
  GtkToggleButton* button_2 =
    GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_TOGGLEBUTTON_SERVER_JOIN_PART_NAME)));
  ACE_ASSERT (button_2);
  gtk_widget_set_sensitive (GTK_WIDGET (button_2), FALSE);
  //// make part button sensitive (if it is not already)
  //button =
  //  GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
  //                                    ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_SERVER_PART_NAME)));
  //ACE_ASSERT (button);
  //gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

  return FALSE;
}

G_MODULE_EXPORT gint
state_repository_button_clicked_GTK_cb (GtkWidget* widget_in,
                                        gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::state_repository_button_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  struct RPG_Client_GTK_CBData* data_p =
      static_cast<struct RPG_Client_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GTK_BuildersConstIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  // retrieve tree model
  GtkComboBox* repository_combobox =
      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_ENGINESTATE_NAME)));
  ACE_ASSERT (repository_combobox);
  GtkTreeModel* model = gtk_combo_box_get_model (repository_combobox);
  ACE_ASSERT (model);

  // re-load savedstates data
  unsigned int num_entries =
    ::load_files (REPOSITORY_ENGINESTATE, GTK_LIST_STORE (model));

  // ... sensitize/activate widgets
  gtk_widget_set_sensitive (GTK_WIDGET (repository_combobox),
                            (num_entries > 0));
  gtk_combo_box_set_active (repository_combobox, -1);

  return FALSE;
}

void
togglebutton_join_part_toggled_cb (GtkToggleButton* toggleButton_in,
                                   gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::togglebutton_join_part_toggled_cb"));

  // handle manual toggles
  if (untoggling_server_join_button)
  {
    untoggling_server_join_button = false;
    return; // done
  } // end IF

  // sanity check(s)
  struct RPG_Client_GTK_CBData* data_p =
    static_cast<struct RPG_Client_GTK_CBData*> (userData_in);
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->clientEngine);
  ACE_ASSERT (data_p->levelEngine);
  Common_UI_GTK_BuildersConstIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  if (gtk_toggle_button_get_active (toggleButton_in))
  {
    if (data_p->levelEngine->isRunning ())
      data_p->levelEngine->stop (true); // locked access ?

    // set start position, if necessary
    if (data_p->entity.position == std::make_pair (std::numeric_limits<unsigned int>::max (),
                                                   std::numeric_limits<unsigned int>::max ()))
      data_p->entity.position = data_p->levelEngine->getStartPosition (true); // locked access ?

    // update the level state

    // activate the current character
    RPG_Engine_EntityID_t id = data_p->levelEngine->add (&(data_p->entity),
                                                         true); // locked access ?
    data_p->levelEngine->setActive (id);

    // center on character
    data_p->clientEngine->setView (data_p->entity.position,
                                   true); // refresh ?

    // play ambient sound
    RPG_SOUND_EVENT_MANAGER_SINGLETON::instance ()->start ();

    // (re-)start game engine
    data_p->levelEngine->start ();

    gtk_button_set_label (GTK_BUTTON (toggleButton_in),
                          GTK_STOCK_DISCONNECT);

    // make create button insensitive
    GtkButton* button =
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_CREATE_NAME)));
    ACE_ASSERT (button);
    gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

    // make drop button insensitive
    button =
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_DROP_NAME)));
    ACE_ASSERT (button);
    gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

    // make load button insensitive
    button =
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_LOAD_NAME)));
    ACE_ASSERT (button);
    gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

    // make save button insensitive
    button =
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_STORE_NAME)));
    ACE_ASSERT (button);
    gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

    // make create_map button insensitive
    button =
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_MAP_CREATE_NAME)));
    ACE_ASSERT (button);
    gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

    // make drop_map button insensitive
    button =
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_MAP_DROP_NAME)));
    ACE_ASSERT (button);
    gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

    // make load_map button insensitive
    button =
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_MAP_LOAD_NAME)));
    ACE_ASSERT (button);
    gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

    // make save_map button insensitive
    button =
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_MAP_STORE_NAME)));
    ACE_ASSERT (button);
    gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

    // make character combox insensitive
    GtkComboBox* combo_box =
      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
    ACE_ASSERT (combo_box);
    gtk_widget_set_sensitive (GTK_WIDGET (combo_box), FALSE);

    // make map combobox insensitive
    combo_box =
      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_COMBOBOX_MAP_NAME)));
    ACE_ASSERT (combo_box);
    gtk_widget_set_sensitive (GTK_WIDGET (combo_box), FALSE);

    // make quit button insensitive
    button =
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_QUIT_NAME)));
    ACE_ASSERT (button);
    gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

    // make rest button sensitive
    button =
      GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_REST_NAME)));
    ACE_ASSERT (button);
    gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

    // minimize dialog window
    GdkWindow* toplevel =
      gtk_widget_get_parent_window (GTK_WIDGET (toggleButton_in));
    ACE_ASSERT (toplevel);
    gdk_window_iconify (toplevel);

    return;
  } // end IF

  // deactivate the current character (if any)
  // *NOTE*: iff the active player died, do nothing
  RPG_Engine_EntityID_t id = data_p->levelEngine->getActive (true); // locked access ?
  if (id)
    data_p->levelEngine->remove (id,
                                 true); // locked access ?

  // stop ambient sound
  RPG_SOUND_EVENT_MANAGER_SINGLETON::instance ()->stop ();

  //// make join button sensitive IFF player is not disabled
  //if (!RPG_Engine_Common_Tools::isCharacterDisabled (data_p->entity.character))
  //  gtk_widget_set_sensitive (GTK_WIDGET (toggleButton_in), TRUE);

  gtk_button_set_label (GTK_BUTTON (toggleButton_in),
                        GTK_STOCK_CONNECT);

  // make drop button sensitive
  GtkButton* button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_DROP_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

  // make save button sensitive
  button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_STORE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

  // make map create button sensitive
  button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_MAP_CREATE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

  // make map load button sensitive
  button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_MAP_LOAD_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

  // make character combox sensitive
  GtkComboBox* combo_box =
    GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
  ACE_ASSERT (combo_box);
  gtk_widget_set_sensitive (GTK_WIDGET (combo_box), TRUE);

  // make map combobox sensitive
  combo_box =
    GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_COMBOBOX_MAP_NAME)));
  ACE_ASSERT (combo_box);
  gtk_widget_set_sensitive (GTK_WIDGET (combo_box), TRUE);

  // make quit button sensitive
  button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_QUIT_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

  // make rest button insensitive
  button =
    GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_REST_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);
}

gint
equip_clicked_GTK_cb (GtkWidget* widget_in,
                      gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::equip_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);

  // sanity check(s)
  struct RPG_Client_GTK_CBData* data_p =
    static_cast<struct RPG_Client_GTK_CBData*> (userData_in);
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->entity.character);
  ACE_ASSERT (data_p->entity.character->isPlayerCharacter ());
  RPG_Player* player = static_cast<RPG_Player*> (data_p->entity.character);
  ACE_ASSERT (player);
  Common_UI_GTK_BuildersConstIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());
  GtkWidget* equipment_dialog =
      GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_EQUIPMENT_NAME)));
  ACE_ASSERT (equipment_dialog);

  ::update_equipment (*data_p);

  // draw it
  if (!gtk_widget_get_visible (equipment_dialog))
    gtk_widget_show_all (equipment_dialog);

  return FALSE;
}

void
equipment_dialog_response_cb (GtkDialog* dialog_in,
                              gint responseId_in,
                              gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::equipment_dialog_response_cb"));

  // sanity check(s)
  struct RPG_Client_GTK_CBData* data_p =
    static_cast<struct RPG_Client_GTK_CBData*> (userData_in);
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->entity.character);
  ACE_ASSERT (data_p->entity.character->isPlayerCharacter ());
  RPG_Player* player = static_cast<RPG_Player*> (data_p->entity.character);
  ACE_ASSERT (player);
  Common_UI_GTK_BuildersConstIterator_t iterator =
    data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  if (responseId_in == GTK_RESPONSE_OK)
  {
    ::update_entity_profile (data_p->entity,
                             (*iterator).second.second);
    ::update_inventory (*player,
                        (*iterator).second.second);
  } // end IF

  gtk_widget_hide (GTK_WIDGET (dialog_in));
}

gint
item_toggled_GTK_cb (GtkWidget* widget_in,
                     gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::item_toggled_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  struct RPG_Client_GTK_CBData* data_p =
      static_cast<struct RPG_Client_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->entity.character);
  ACE_ASSERT (data_p->entity.character->isPlayerCharacter ());
  RPG_Player* player =
    static_cast<RPG_Player*> (data_p->entity.character);
  ACE_ASSERT (data_p->levelEngine);
  ACE_ASSERT (data_p->clientEngine);

  // retrieve item id
  std::string widget_name = gtk_widget_get_name (widget_in);
  std::istringstream input (widget_name);
  RPG_Item_ID_t item_id = 0;
  if (!(input >> item_id))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to retrieve item id (was: \"%s\"), aborting\n"),
                ACE_TEXT (widget_name.c_str ())));
    return TRUE; // propagate
  } // end IF

  data_p->levelEngine->lock ();
  RPG_Engine_EntityID_t active_entity = data_p->levelEngine->getActive (false); // locked access ?
  ACE_UINT8 visible_radius_before = 0;
  if (active_entity)
    visible_radius_before = data_p->levelEngine->getVisibleRadius (active_entity,
                                                                   false); // locked access ?

  // *TODO*: where ?
  RPG_Player_Equipment& equipment_r = player->getEquipment ();
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget_in)))
    equipment_r.equip (item_id,
                       player->getOffHand (),
                       EQUIPMENTSLOT_ANY);
  else
    equipment_r.unequip (item_id);

  // equipped light source --> update vision ?
  if (active_entity)
  {
    struct RPG_Engine_ClientNotificationParameters parameters;
    parameters.visible_radius =
      data_p->levelEngine->getVisibleRadius (active_entity,
                                             false); // locked access ?
    if (visible_radius_before != parameters.visible_radius)
    {
      // notify client window
      parameters.entity_id = active_entity;
      parameters.condition = RPG_COMMON_CONDITION_INVALID;
      data_p->levelEngine->getVisiblePositions (active_entity,
                                                parameters.positions,
                                                false); // locked access ?
      parameters.previous_position =
        std::make_pair (std::numeric_limits<unsigned int>::max (),
                        std::numeric_limits<unsigned int>::max ());
      try {
        data_p->clientEngine->notify (COMMAND_E2C_ENTITY_VISION,
                                      parameters,
                                      false); // locked access ?
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in RPG_Engine_IClient::notify(\"%s\"), continuing\n"),
                    ACE_TEXT (RPG_Engine_CommandHelper::RPG_Engine_CommandToString (COMMAND_E2C_ENTITY_VISION).c_str ())));
      }
    } // end IF
  } // end IF
  data_p->levelEngine->unlock ();

  ::update_equipment (*data_p);

  return FALSE;
}

gint
rest_clicked_GTK_cb (GtkWidget* widget_in,
                     gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::rest_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);

  struct RPG_Client_GTK_CBData* data_p =
      static_cast<struct RPG_Client_GTK_CBData*> (userData_in);
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->UIState);
  Common_UI_GTK_BuildersConstIterator_t iterator =
      data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());
  GtkDialog* dialog_p =
      GTK_DIALOG (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_REST_NAME)));
  ACE_ASSERT (dialog_p);

  // draw it
  if (!gtk_widget_get_visible (GTK_WIDGET (dialog_p)))
    gtk_widget_show_all (GTK_WIDGET (dialog_p));

  return FALSE;
}

void
levelup_clicked_cb (GtkButton* button_in,
                    gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::levelup_clicked_cb"));

  ACE_UNUSED_ARG (button_in);

  // sanity check(s)
  struct RPG_Client_GTK_CBData* data_p =
      static_cast<struct RPG_Client_GTK_CBData*> (userData_in);
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->UIState);
  Common_UI_GTK_BuildersConstIterator_t iterator =
      data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());
  GtkDialog* dialog_p =
      GTK_DIALOG (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_LEVELUP_NAME)));
  ACE_ASSERT (dialog_p);

  // compute assigned skill points of the current player
  ACE_ASSERT (data_p->entity.character);
  ACE_ASSERT (data_p->entity.character->isPlayerCharacter ());
  RPG_Player* player = static_cast<RPG_Player*> (data_p->entity.character);
  ACE_ASSERT (player);
  data_p->assignedSkillPointsPreLevelUp = 0;
  const RPG_Character_Skills_t& skills_r = player->getSkills ();
  for (RPG_Character_SkillsConstIterator_t iterator = skills_r.begin ();
       iterator != skills_r.end ();
       ++iterator)
    data_p->assignedSkillPointsPreLevelUp += (*iterator).second;

  ::update_levelup (*data_p);
  ::hitdice_button_clicked_cb (NULL, userData_in); // roll once

  // draw it
  if (!gtk_widget_get_visible (GTK_WIDGET (dialog_p)))
    gtk_widget_show_all (GTK_WIDGET (dialog_p));
  //gint response_i = gtk_dialog_run (dialog_p);
}

void
hitdice_button_clicked_cb (GtkButton* button_in,
                           gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::hitdice_button_clicked_cb"));

  ACE_UNUSED_ARG (button_in);

  // sanity check(s)
  struct RPG_Client_GTK_CBData* data_p =
      static_cast<struct RPG_Client_GTK_CBData*> (userData_in);
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->entity.character);
  ACE_ASSERT (data_p->entity.character->isPlayerCharacter ());
  RPG_Player* player = static_cast<RPG_Player*> (data_p->entity.character);
  ACE_ASSERT (data_p->UIState);
  Common_UI_GTK_BuildersConstIterator_t iterator =
      data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());
  GtkLabel* label_p =
      GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_LABEL_HITPOINTS_NAME)));
  ACE_ASSERT (label_p);

  RPG_Dice_RollResult_t result;
  RPG_Dice::simulateRoll (current_dice_roll,
                          1,
                          result);
  std::ostringstream converter;
  converter << player->getNumTotalHitPoints () + result.front ();
  gtk_label_set_text (label_p,
                      converter.str ().c_str ());
}

void
rest_button_rest_clicked_cb (GtkButton* button_in,
                             gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::rest_button_rest_clicked_cb"));

  ACE_UNUSED_ARG (button_in);

  // sanity check(s)
  struct RPG_Client_GTK_CBData* data_p =
      static_cast<struct RPG_Client_GTK_CBData*> (userData_in);
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->entity.character);
  ACE_ASSERT (data_p->entity.character->isPlayerCharacter ());
  RPG_Player* player = static_cast<RPG_Player*> (data_p->entity.character);
  ACE_ASSERT (data_p->UIState);
  Common_UI_GTK_BuildersConstIterator_t iterator =
      data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());
  GtkEntry* entry_p =
      GTK_ENTRY (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_ENTRY_HOURS_NAME)));
  ACE_ASSERT (entry_p);

  std::istringstream converter;
  converter.str (gtk_entry_buffer_get_text (gtk_entry_get_buffer (entry_p)));
  unsigned int value_i = 0;
  converter >> value_i;

  player->rest (REST_FULL,
                value_i);

  ::update_character_profile (*player, (*iterator).second.second);
}

void
treeview_feats_selection_changed_cb (GtkTreeSelection* selection_in,
                                     gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::treeview_feats_selection_changed_cb"));

  // sanity check(s)
  struct RPG_Client_GTK_CBData* data_p =
      static_cast<struct RPG_Client_GTK_CBData*> (userData_in);
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->entity.character);
  ACE_ASSERT (data_p->entity.character->isPlayerCharacter ());
  RPG_Player* player = static_cast<RPG_Player*> (data_p->entity.character);
  ACE_ASSERT (player);
  ACE_ASSERT (data_p->UIState);
  Common_UI_GTK_BuildersConstIterator_t iterator =
      data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  gint selected_rows_i = gtk_tree_selection_count_selected_rows (selection_in);
  GtkLabel* label_p =
      GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_LABEL_FEATSREMAINING_NAME)));
  ACE_ASSERT (label_p);
  std::ostringstream converter;
  converter << player->getFeatsPerLevel (data_p->subClass) - selected_rows_i;
  gtk_label_set_text (label_p,
                      converter.str ().c_str ());
}

//gboolean
//treeview_feats_button_press_event_cb (GtkWidget* widget_in,
//                                      GdkEventButton* event_in,
//                                      gpointer userData_in)
//{
//  RPG_TRACE (ACE_TEXT ("::treeview_feats_button_press_event_cb"));

//  ACE_UNUSED_ARG (widget_in);

//  // sanity check(s)
//  struct RPG_Client_GTK_CBData* data_p =
//      static_cast<struct RPG_Client_GTK_CBData*> (userData_in);
//  ACE_ASSERT (data_p);
//  ACE_ASSERT (data_p->UIState);
//  Common_UI_GTK_BuildersConstIterator_t iterator =
//      data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
//  ACE_ASSERT (iterator != data_p->UIState->builders.end ());
//  GtkTreeView* tree_view_p =
//      GTK_TREE_VIEW (gtk_builder_get_object ((*iterator).second.second,
//                                             ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_TREEVIEW_FEATS_NAME)));
//  ACE_ASSERT (tree_view_p);
//}

void
treeview_skills_selection_changed_cb (GtkTreeSelection* selection_in,
                                      gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::treeview_skills_selection_changed_cb"));

  // sanity check(s)
  struct RPG_Client_GTK_CBData* data_p =
      static_cast<struct RPG_Client_GTK_CBData*> (userData_in);
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->entity.character);
  ACE_ASSERT (data_p->entity.character->isPlayerCharacter ());
  RPG_Player* player = static_cast<RPG_Player*> (data_p->entity.character);
  ACE_ASSERT (data_p->UIState);
  Common_UI_GTK_BuildersConstIterator_t iterator =
      data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());
  GtkListStore* list_store_p =
      GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_LISTSTORE_SKILLS_NAME)));
  ACE_ASSERT (list_store_p);

//  gint selected_rows_i = 0;
////  selected_rows_i = gtk_tree_selection_count_selected_rows (selection_in);
//  GtkTreeIter tree_iterator;
//  gboolean valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (list_store_p),
//                                                  &tree_iterator);
//  while (valid)
//  {
//    gint data_i = 0;
//    gtk_tree_model_get (GTK_TREE_MODEL (list_store_p),
//                        &tree_iterator,
//                        2, &data_i,
//                        -1);
//    selected_rows_i += data_i;
//    valid = gtk_tree_model_iter_next (GTK_TREE_MODEL (list_store_p),
//                                      &tree_iterator);
//  } // end WHILE
//
//  GtkLabel* label_p =
//      GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
//                                       ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_LABEL_SKILLSREMAINING_NAME)));
//  ACE_ASSERT (label_p);
//  std::ostringstream converter;
//  converter << data_p->assignedSkillPointsPreLevelUp + player->getSkillsPerLevel (data_p->subClass) - selected_rows_i;
//  gtk_label_set_text (label_p,
//                      converter.str ().c_str ());
}

void
cellrenderer_skills_editing_started_cb (GtkCellRenderer* renderer_in,
                                        GtkCellEditable* editable_in,
                                        char* path_in,
                                        gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::cellrenderer_skills_editing_started_cb"));

  // sanity check(s)
  ACE_ASSERT (path_in);

  g_signal_connect (G_OBJECT (editable_in),
                    ACE_TEXT_ALWAYS_CHAR ("editing-done"),
                    G_CALLBACK (celleditable_skills_editing_done_cb),
                    userData_in);

  current_path = path_in;
}

void
celleditable_skills_editing_done_cb (GtkCellEditable* editable_in,
                                     gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::celleditable_skills_editing_done_cb"));

  // sanity check(s)
  ACE_ASSERT (!current_path.empty ());
  ACE_ASSERT (GTK_IS_ENTRY (editable_in));
  struct RPG_Client_GTK_CBData* data_p =
      static_cast<struct RPG_Client_GTK_CBData*> (userData_in);
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->entity.character);
  ACE_ASSERT (data_p->entity.character->isPlayerCharacter ());
  RPG_Player* player = static_cast<RPG_Player*> (data_p->entity.character);
  ACE_ASSERT (data_p->UIState);
  Common_UI_GTK_BuildersConstIterator_t iterator =
      data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());
  GtkListStore* list_store_p =
      GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_LISTSTORE_SKILLS_NAME)));
  ACE_ASSERT (list_store_p);

  GtkTreeIter tree_iterator;
  if (!gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL (list_store_p),
                                            &tree_iterator,
                                            current_path.c_str ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_tree_model_get_iter_from_string() (was: \"%s\"), returning\n"),
                ACE_TEXT (current_path.c_str ())));
    return;
  } // end IF

  std::istringstream converter;
  converter.str (gtk_entry_buffer_get_text (gtk_entry_get_buffer (GTK_ENTRY (editable_in))));
  gint value_i = 0;
  converter >> value_i;
  // *TODO*: is this necessary ?
  gtk_list_store_set (list_store_p,
                      &tree_iterator,
                      2, value_i,
                      -1);

  gint selected_rows_i = 0;
//  selected_rows_i = gtk_tree_selection_count_selected_rows (selection_in);
  gboolean valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (list_store_p),
                                                  &tree_iterator);
  while (valid)
  {
    gint data_i = 0;
    gtk_tree_model_get (GTK_TREE_MODEL (list_store_p),
                        &tree_iterator,
                        2, &data_i,
                        -1);
    selected_rows_i += data_i;
    valid = gtk_tree_model_iter_next (GTK_TREE_MODEL (list_store_p),
                                      &tree_iterator);
  } // end WHILE

  GtkLabel* label_p =
      GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                       ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_LABEL_SKILLSREMAINING_NAME)));
  ACE_ASSERT (label_p);
  std::ostringstream converter_2;
  converter_2 << data_p->assignedSkillPointsPreLevelUp + player->getSkillsPerLevel (data_p->subClass) - selected_rows_i;
  gtk_label_set_text (label_p,
                      converter_2.str ().c_str ());
}

void
treeview_spells_selection_changed_cb (GtkTreeSelection* selection_in,
                                      gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::treeview_spells_selection_changed_cb"));

  // sanity check(s)
  struct RPG_Client_GTK_CBData* data_p =
      static_cast<struct RPG_Client_GTK_CBData*> (userData_in);
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->entity.character);
  ACE_ASSERT (data_p->entity.character->isPlayerCharacter ());
  RPG_Player* player = static_cast<RPG_Player*> (data_p->entity.character);
  ACE_ASSERT (player);
  ACE_ASSERT (data_p->UIState);
  Common_UI_GTK_BuildersConstIterator_t iterator =
      data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  gint selected_rows_i = gtk_tree_selection_count_selected_rows (selection_in);
  GtkLabel* label_p =
      GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                       ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_LABEL_SPELLSREMAINING_NAME)));
  ACE_ASSERT (label_p);
  std::ostringstream converter;
  ACE_UINT16 number_of_known_spells_i = 0;
  const RPG_Magic_SpellTypes_t& known_spells_a = player->getKnownSpells ();
  for (ACE_UINT8 i = 0;
       i < RPG_COMMON_MAX_SPELL_LEVEL;
       ++i)
  {
    ACE_UINT16 number_of_spells_per_level_i =
        player->getKnownSpellsPerLevel (data_p->subClass,
                                        i);
    number_of_known_spells_i +=
        (std::numeric_limits<ACE_UINT16>::max () == number_of_spells_per_level_i ? 0
                                                                                 : number_of_spells_per_level_i);
  } // end FOR
  if (number_of_known_spells_i)
    number_of_known_spells_i -= static_cast<ACE_UINT16> (known_spells_a.size ());
  int value_i = number_of_known_spells_i - selected_rows_i;
  if (value_i < 0)
  {
    gtk_tree_selection_unselect_all (selection_in);
    value_i = 0;
  } // end IF
  converter << value_i;
  gtk_label_set_text (label_p,
                      converter.str ().c_str ());
}

void
spinbutton_attribute_value_changed_cb (GtkSpinButton* spinButton_in,
                                       gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::spinbutton_attribute_value_changed_cb"));

  // sanity check(s)
//  if (setting_up_levelup_attributes)
//    return;
  struct RPG_Client_GTK_CBData* data_p =
      static_cast<struct RPG_Client_GTK_CBData*> (userData_in);
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->entity.character);
  ACE_ASSERT (data_p->entity.character->isPlayerCharacter ());
  RPG_Player* player = static_cast<RPG_Player*> (data_p->entity.character);
  ACE_ASSERT (data_p->UIState);
  Common_UI_GTK_BuildersConstIterator_t iterator =
      data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());

  // test: which attribute changed ?
  enum RPG_Common_Attribute attribute_e = RPG_COMMON_ATTRIBUTE_INVALID;
  if (!ACE_OS::strcmp (gtk_buildable_get_name (GTK_BUILDABLE (spinButton_in)),
                       ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_STRENGTH_NAME)))
    attribute_e = ATTRIBUTE_STRENGTH;
  else if (!ACE_OS::strcmp (gtk_buildable_get_name (GTK_BUILDABLE (spinButton_in)),
                            ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_DEXTERITY_NAME)))
    attribute_e = ATTRIBUTE_DEXTERITY;
  else if (!ACE_OS::strcmp (gtk_buildable_get_name (GTK_BUILDABLE (spinButton_in)),
                            ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_CONSTITUTION_NAME)))
    attribute_e = ATTRIBUTE_CONSTITUTION;
  else if (!ACE_OS::strcmp (gtk_buildable_get_name (GTK_BUILDABLE (spinButton_in)),
                            ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_INTELLIGENCE_NAME)))
    attribute_e = ATTRIBUTE_INTELLIGENCE;
  else if (!ACE_OS::strcmp (gtk_buildable_get_name (GTK_BUILDABLE (spinButton_in)),
                            ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_WISDOM_NAME)))
    attribute_e = ATTRIBUTE_WISDOM;
  else if (!ACE_OS::strcmp (gtk_buildable_get_name (GTK_BUILDABLE (spinButton_in)),
                            ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_CHARISMA_NAME)))
    attribute_e = ATTRIBUTE_CHARISMA;
  else
    ACE_ASSERT (false);

  // test: compute #values changed so far
  GtkSpinButton* spinbutton_p =
      GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_STRENGTH_NAME)));
  ACE_ASSERT (spinbutton_p);
  gint value_i = gtk_spin_button_get_value_as_int (spinbutton_p);
  spinbutton_p =
      GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_DEXTERITY_NAME)));
  ACE_ASSERT (spinbutton_p);
  value_i += gtk_spin_button_get_value_as_int (spinbutton_p);
  spinbutton_p =
      GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_CONSTITUTION_NAME)));
  ACE_ASSERT (spinbutton_p);
  value_i += gtk_spin_button_get_value_as_int (spinbutton_p);
  spinbutton_p =
      GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_INTELLIGENCE_NAME)));
  ACE_ASSERT (spinbutton_p);
  value_i += gtk_spin_button_get_value_as_int (spinbutton_p);
  spinbutton_p =
      GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_WISDOM_NAME)));
  ACE_ASSERT (spinbutton_p);
  value_i += gtk_spin_button_get_value_as_int (spinbutton_p);
  spinbutton_p =
      GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_CHARISMA_NAME)));
  ACE_ASSERT (spinbutton_p);
  value_i += gtk_spin_button_get_value_as_int (spinbutton_p);
  gint value_2 = player->getAttribute (ATTRIBUTE_STRENGTH);
  value_2 += player->getAttribute (ATTRIBUTE_DEXTERITY);
  value_2 += player->getAttribute (ATTRIBUTE_CONSTITUTION);
  value_2 += player->getAttribute (ATTRIBUTE_INTELLIGENCE);
  value_2 += player->getAttribute (ATTRIBUTE_WISDOM);
  value_2 += player->getAttribute (ATTRIBUTE_CHARISMA);
  unsigned int num_values_changed_i = std::abs (value_i - value_2);

  // test: value went up / down ?
  GtkLabel* label_p = NULL;
  std::stringstream converter;
  unsigned int points_remaining_i = 0;
  if (num_values_changed_i > RPG_CHARACTER_ATTRIBUTE_LEVELUP_POINTS)
  {
    value_2 = gtk_spin_button_get_value_as_int (spinButton_in);
    --value_2;
    goto reset;
  } // end IF

  value_i = gtk_spin_button_get_value_as_int (spinButton_in);
  value_2 = player->getAttribute (attribute_e);
  if (value_i < value_2)
    goto reset;

  label_p =
      GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_LABEL_POINTSREMAINING_NAME)));
  ACE_ASSERT (label_p);
  points_remaining_i =
      RPG_CHARACTER_ATTRIBUTE_LEVELUP_POINTS - num_values_changed_i;
  converter << points_remaining_i;
  gtk_label_set_text (label_p,
                      converter.str ().c_str ());

  return;

reset:
  gtk_spin_button_set_value (spinButton_in,
                             (gdouble)value_2);
}

void
levelup_dialog_response_cb (GtkDialog* dialog_in,
                            gint responseId_in,
                            gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::levelup_dialog_response_cb"));

  // sanity check(s)
  struct RPG_Client_GTK_CBData* data_p =
      static_cast<struct RPG_Client_GTK_CBData*> (userData_in);
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->UIState);
  Common_UI_GTK_BuildersConstIterator_t iterator =
      data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->UIState->builders.end ());
  ACE_ASSERT (data_p->entity.character);
  ACE_ASSERT (data_p->entity.character->isPlayerCharacter ());
  RPG_Player* player = static_cast<RPG_Player*> (data_p->entity.character);

  if (responseId_in == GTK_RESPONSE_OK)
  {
    // update character values
    GtkLabel* label_p =
        GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_LABEL_HITPOINTS_NAME)));
    ACE_ASSERT (label_p);
    std::istringstream converter;
    converter.str (gtk_label_get_text (label_p));
    ACE_UINT16 value_i = 0;
    converter >> value_i;
    player->setNumTotalHitPoints (value_i);

    GtkSpinButton* spin_button_p =
        GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                 ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_STRENGTH_NAME)));
    ACE_ASSERT (spin_button_p);
    const_cast<struct RPG_Character_Attributes&> (player->getAttributes ()).strength =
        gtk_spin_button_get_value_as_int (spin_button_p);
    spin_button_p =
        GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                 ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_DEXTERITY_NAME)));
    ACE_ASSERT (spin_button_p);
    const_cast<struct RPG_Character_Attributes&> (player->getAttributes ()).dexterity =
        gtk_spin_button_get_value_as_int (spin_button_p);
    spin_button_p =
        GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                 ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_CONSTITUTION_NAME)));
    ACE_ASSERT (spin_button_p);
    const_cast<struct RPG_Character_Attributes&> (player->getAttributes ()).constitution =
        gtk_spin_button_get_value_as_int (spin_button_p);
    spin_button_p =
        GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                 ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_INTELLIGENCE_NAME)));
    ACE_ASSERT (spin_button_p);
    const_cast<struct RPG_Character_Attributes&> (player->getAttributes ()).intelligence =
        gtk_spin_button_get_value_as_int (spin_button_p);
    spin_button_p =
        GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                 ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_WISDOM_NAME)));
    ACE_ASSERT (spin_button_p);
    const_cast<struct RPG_Character_Attributes&> (player->getAttributes ()).wisdom =
        gtk_spin_button_get_value_as_int (spin_button_p);
    spin_button_p =
        GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                 ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_SPINBUTTON_CHARISMA_NAME)));
    ACE_ASSERT (spin_button_p);
    const_cast<struct RPG_Character_Attributes&> (player->getAttributes ()).charisma =
        gtk_spin_button_get_value_as_int (spin_button_p);

    GtkTreeView* tree_view_p =
        GTK_TREE_VIEW (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_TREEVIEW_FEATS_NAME)));
    ACE_ASSERT (tree_view_p);
    GtkTreeSelection* tree_selection_p =
        gtk_tree_view_get_selection (tree_view_p);
    ACE_ASSERT (tree_selection_p);
    GtkTreeIter tree_iterator;
    GtkListStore* list_store_p;
    GList* list_p = gtk_tree_selection_get_selected_rows (tree_selection_p,
                                                          NULL);    
    if (!list_p)
      goto continue_; // no selection
    list_store_p =
        GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                                ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_LISTSTORE_FEATS_NAME)));
    ACE_ASSERT (list_store_p);
    for (GList* list_2 = list_p;
         list_2;
         list_2 = g_list_next (list_2))
    {
      GtkTreePath* tree_path_p = static_cast<GtkTreePath*> (list_2->data);
      ACE_ASSERT (tree_path_p);
      gtk_tree_model_get_iter (GTK_TREE_MODEL (list_store_p),
                               &tree_iterator,
                               tree_path_p);
      gint data_i = 0;
      gtk_tree_model_get (GTK_TREE_MODEL (list_store_p),
                          &tree_iterator,
                          1, &data_i,
                          -1);
      const_cast<RPG_Character_Feats_t&> (player->getFeats ()).insert (static_cast<enum RPG_Character_Feat> (data_i));
    } // end FOR
    g_list_free_full (list_p, (GDestroyNotify)gtk_tree_path_free);
continue_:
    RPG_Character_Skills_t& skills_a =
        const_cast<RPG_Character_Skills_t&> (player->getSkills ());
    list_store_p =
        GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                                ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_LISTSTORE_SKILLS_NAME)));
    ACE_ASSERT (list_store_p);
    gboolean valid_b = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (list_store_p),
                                                      &tree_iterator);
    while (valid_b)
    {
      gint data_i = 0;
      gint data_2 = 0;
      gtk_tree_model_get (GTK_TREE_MODEL (list_store_p),
                          &tree_iterator,
                          1, &data_i,
                          2, &data_2,
                          -1);
      RPG_Character_SkillsIterator_t iterator_2;
      enum RPG_Common_Skill skill_e;
      if (!data_2) // value is zero ? continue : proceed
        goto next;
      skill_e = static_cast<enum RPG_Common_Skill> (data_i);
      iterator_2 = skills_a.find (skill_e);
      if (iterator_2 == skills_a.end ())
        skills_a.insert (std::make_pair (skill_e, data_2));
      else
        (*iterator_2).second = data_2;
next:
      valid_b = gtk_tree_model_iter_next (GTK_TREE_MODEL (list_store_p),
                                          &tree_iterator);
    } // end WHILE

    tree_view_p =
        GTK_TREE_VIEW (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_TREEVIEW_SPELLS_NAME)));
    ACE_ASSERT (tree_view_p);
    tree_selection_p = gtk_tree_view_get_selection (tree_view_p);
    ACE_ASSERT (tree_selection_p);
    list_p = gtk_tree_selection_get_selected_rows (tree_selection_p,
                                                   NULL);
    if (!list_p)
      goto continue_2; // no selection
    list_store_p =
        GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                                ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_LISTSTORE_SPELLS_NAME)));
    ACE_ASSERT (list_store_p);
    for (GList* list_2 = list_p;
         list_2;
         list_2 = g_list_next (list_2))
    {
      GtkTreePath* tree_path_p = static_cast<GtkTreePath*> (list_2->data);
      ACE_ASSERT (tree_path_p);
      gtk_tree_model_get_iter (GTK_TREE_MODEL (list_store_p),
                               &tree_iterator,
                               tree_path_p);
      gint data_i = 0;
      gtk_tree_model_get (GTK_TREE_MODEL (list_store_p),
                          &tree_iterator,
                          1, &data_i,
                          -1);
      const_cast<RPG_Magic_SpellTypes_t&> (player->getKnownSpells ()).insert (static_cast<enum RPG_Magic_SpellType> (data_i));
    } // end FOR
    g_list_free_full (list_p, (GDestroyNotify)gtk_tree_path_free);

continue_2:
    ::update_entity_profile (data_p->entity,
                             (*iterator).second.second);

    GtkButton* button_p =
        GTK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_LEVELUP_NAME)));
    ACE_ASSERT (button_p);
    gtk_widget_set_sensitive (GTK_WIDGET (button_p), FALSE);
  } // end IF

  gtk_widget_hide (GTK_WIDGET (dialog_in));
}

void
rest_dialog_response_cb (GtkDialog* dialog_in,
                         gint responseId_in,
                         gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::rest_dialog_response_cb"));

  ACE_UNUSED_ARG (responseId_in);
  ACE_UNUSED_ARG (userData_in);

  gtk_widget_hide (GTK_WIDGET (dialog_in));
}

gint
server_repository_combobox_changed_GTK_cb (GtkWidget* widget_in,
                                           gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::server_repository_combobox_changed_GTK_cb"));

//   struct RPG_Client_GTK_CBData* data = static_cast<struct RPG_Client_GTK_CBData*>(userData_in);
//   ACE_ASSERT(data);
//
//   // sanity check(s)
//   ACE_ASSERT(widget_in);
//   ACE_ASSERT((*iterator).second.second);
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
//   data_p->entity = RPG_Engine_Common_Tools::loadEntity(filename,
//                                                      data_p->schema_repository);
//   ACE_ASSERT(data_p->entity.character);
//
//   // update entity profile widgets
//   ::update_entity_profile(data_p->entity,
//                           (*iterator).second.second);
//
//   // make character display frame sensitive (if it's not already)
//   GtkFrame* character_frame = GTK_FRAME(gtk_builder_get_object((*iterator).second.second,
//                                                              ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_FRAME_CHARACTER_NAME)));
//   ACE_ASSERT(character_frame);
//   gtk_widget_set_sensitive(GTK_WIDGET(character_frame), TRUE);
//
//   // make join button sensitive (if it's not already)
//   GtkButton* join_game = GTK_BUTTON(gtk_builder_get_object((*iterator).second.second,
//                                                          ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_JOIN_NAME)));
//   ACE_ASSERT(join_game);
//   gtk_widget_set_sensitive(GTK_WIDGET(join_game), TRUE);

  return FALSE;
}

G_MODULE_EXPORT gint
server_repository_button_clicked_GTK_cb (GtkWidget* widget_in,
                                         gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::server_repository_button_clicked_GTK_cb"));

//   ACE_UNUSED_ARG(widget_in);
//   struct RPG_Client_GTK_CBData* data = static_cast<struct RPG_Client_GTK_CBData*>(userData_in);
//   ACE_ASSERT(data);
//
//   // sanity check(s)
//   ACE_ASSERT((*iterator).second.second);
//
//   // retrieve tree model
//   GtkComboBox* repository_combobox = GTK_COMBO_BOX(gtk_builder_get_object((*iterator).second.second,
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
//   GtkFrame* character_frame = GTK_FRAME(gtk_builder_get_object((*iterator).second.second,
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
#ifdef __cplusplus
}
#endif /* __cplusplus */
