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

#include "map_generator_gui_callbacks.h"

#include "map_generator_gui_common.h"

#include "rpg_client_defines.h"
#include "rpg_client_callbacks.h"
#include "rpg_client_ui_tools.h"

#include "rpg_engine_defines.h"
#include "rpg_engine_common.h"
#include "rpg_engine_common_tools.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_cursor.h"
#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_common_tools.h"

#include "rpg_map_defines.h"
#include "rpg_map_common_tools.h"
#include "rpg_map_level.h"

#include "rpg_player_defines.h"
#include "rpg_player_common_tools.h"

#include "rpg_item_instance_manager.h"
#include "rpg_item_weapon.h"
#include "rpg_item_armor.h"

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"
#include "rpg_common_tools.h"
#include "rpg_common_file_tools.h"

#include <gmodule.h>

#include <ace/Log_Msg.h>

#include <sstream>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT void
reset_layout(GTK_cb_data_t& userData_in)
{
	RPG_TRACE(ACE_TEXT("::reset_layout"));

  // sanity check(s)
  ACE_ASSERT(userData_in.XML);

  GtkComboBox* combobox =
      GTK_COMBO_BOX(glade_xml_get_widget(userData_in.XML,
                                         ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENVIRONMENT_PLANE_COMBOBOX_NAME)));
  ACE_ASSERT(combobox);
  gtk_combo_box_set_active(combobox, -1);

  combobox =
        GTK_COMBO_BOX(glade_xml_get_widget(userData_in.XML,
                                           ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENVIRONMENT_TERRAIN_COMBOBOX_NAME)));
  ACE_ASSERT(combobox);
  gtk_combo_box_set_active(combobox, -1);

  combobox =
        GTK_COMBO_BOX(glade_xml_get_widget(userData_in.XML,
                                           ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENVIRONMENT_CLIMATE_COMBOBOX_NAME)));
  ACE_ASSERT(combobox);
  gtk_combo_box_set_active(combobox, -1);

  combobox =
        GTK_COMBO_BOX(glade_xml_get_widget(userData_in.XML,
                                           ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENVIRONMENT_TIMEOFDAY_COMBOBOX_NAME)));
  ACE_ASSERT(combobox);
  gtk_combo_box_set_active(combobox, -1);

  combobox =
        GTK_COMBO_BOX(glade_xml_get_widget(userData_in.XML,
                                           ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENVIRONMENT_LIGHTING_COMBOBOX_NAME)));
  ACE_ASSERT(combobox);
  gtk_combo_box_set_active(combobox, -1);

	GtkToggleButton* togglebutton =
			GTK_TOGGLE_BUTTON(glade_xml_get_widget(userData_in.XML,
																						 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENVIRONMENT_OUTDOORS_CHECKBUTTON_NAME)));
	ACE_ASSERT(togglebutton);
	gtk_toggle_button_set_active(togglebutton,
															 RPG_ENGINE_ENVIRONMENT_DEF_OUTDOORS);

  GtkTreeView* treeview =
        GTK_TREE_VIEW(glade_xml_get_widget(userData_in.XML,
                                           ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENCOUNTERS_MONSTERS_TREEVIEW_NAME)));
  ACE_ASSERT(treeview);
  GtkListStore* liststore = GTK_LIST_STORE(gtk_tree_view_get_model(treeview));
  ACE_ASSERT(liststore);
  gtk_list_store_clear(liststore);

	GtkSpinButton* spinbutton =
			GTK_SPIN_BUTTON(glade_xml_get_widget(userData_in.XML,
																					 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENCOUNTERS_SPAWNINTERVAL_SPINBUTTON_NAME)));
	ACE_ASSERT(spinbutton);
	gtk_spin_button_set_value(spinbutton,
														RPG_ENGINE_ENCOUNTER_DEF_TIMER_INTERVAL);

	spinbutton =
			GTK_SPIN_BUTTON(glade_xml_get_widget(userData_in.XML,
																					 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENCOUNTERS_SPAWNPROBABILITY_SPINBUTTON_NAME)));
	ACE_ASSERT(spinbutton);
	gtk_spin_button_set_value(spinbutton,
														RPG_ENGINE_ENCOUNTER_DEF_PROBABILITY);

	spinbutton =
			GTK_SPIN_BUTTON(glade_xml_get_widget(userData_in.XML,
																					 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENCOUNTERS_MAXNUMSPAWNED_SPINBUTTON_NAME)));
	ACE_ASSERT(spinbutton);
	gtk_spin_button_set_value(spinbutton,
														RPG_ENGINE_ENCOUNTER_DEF_NUM_SPAWNED_MAX);

	spinbutton =
			GTK_SPIN_BUTTON(glade_xml_get_widget(userData_in.XML,
																					 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENCOUNTERS_AMBLEPROBABILITY_SPINBUTTON_NAME)));
	ACE_ASSERT(spinbutton);
	gtk_spin_button_set_value(spinbutton,
														RPG_ENGINE_ENCOUNTER_DEF_AMBLE_PROBABILITY);

  GtkTextView* view =
      GTK_TEXT_VIEW(glade_xml_get_widget(userData_in.XML,
                                         ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_LAYOUT_MAP_TEXTVIEW_NAME)));
  ACE_ASSERT(view);
  GtkTextBuffer* buffer = gtk_text_view_get_buffer(view);
  ACE_ASSERT(buffer);

  GtkTextIter start, end;
  gtk_text_buffer_get_start_iter(buffer, &start);
  gtk_text_buffer_get_end_iter(buffer, &end);
  gtk_text_buffer_delete(buffer, &start, &end);

  // scroll the view accordingly
  // move the iterator to the beginning of line, so it doesn't scroll
  // in horizontal direction
  gtk_text_iter_set_line_offset(&end, 0);

  // ...and place the mark at iter. The mark will stay there after insertion
  // because it has "right" gravity
  GtkTextMark* mark =
      gtk_text_buffer_get_mark(buffer,
                               ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_SCROLLMARK_NAME));
  //  gtk_text_buffer_move_mark(buffer,
  //                            mark,
  //                            &end);

  // scroll the mark onscreen
  gtk_text_view_scroll_mark_onscreen(view,
                                     mark);
}

G_MODULE_EXPORT void
update_layout(GTK_cb_data_t& userData_in)
{
  RPG_TRACE(ACE_TEXT("::update_layout"));

  // sanity check(s)
  ACE_ASSERT(userData_in.XML);

  GtkComboBox* combobox =
      GTK_COMBO_BOX(glade_xml_get_widget(userData_in.XML,
                                         ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENVIRONMENT_PLANE_COMBOBOX_NAME)));
  ACE_ASSERT(combobox);
  gtk_combo_box_set_active(combobox,
                           userData_in.current_level.metadata.environment.plane);

  combobox =
        GTK_COMBO_BOX(glade_xml_get_widget(userData_in.XML,
                                           ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENVIRONMENT_TERRAIN_COMBOBOX_NAME)));
  ACE_ASSERT(combobox);
  gtk_combo_box_set_active(combobox,
                           userData_in.current_level.metadata.environment.terrain);

  combobox =
        GTK_COMBO_BOX(glade_xml_get_widget(userData_in.XML,
                                           ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENVIRONMENT_CLIMATE_COMBOBOX_NAME)));
  ACE_ASSERT(combobox);
  gtk_combo_box_set_active(combobox,
                           userData_in.current_level.metadata.environment.climate);

  combobox =
        GTK_COMBO_BOX(glade_xml_get_widget(userData_in.XML,
                                           ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENVIRONMENT_TIMEOFDAY_COMBOBOX_NAME)));
  ACE_ASSERT(combobox);
  gtk_combo_box_set_active(combobox,
                           userData_in.current_level.metadata.environment.time);

  combobox =
        GTK_COMBO_BOX(glade_xml_get_widget(userData_in.XML,
                                           ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENVIRONMENT_LIGHTING_COMBOBOX_NAME)));
  ACE_ASSERT(combobox);
  gtk_combo_box_set_active(combobox,
                           userData_in.current_level.metadata.environment.lighting);

	GtkToggleButton* togglebutton =
			GTK_TOGGLE_BUTTON(glade_xml_get_widget(userData_in.XML,
																						 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENVIRONMENT_OUTDOORS_CHECKBUTTON_NAME)));
	ACE_ASSERT(togglebutton);
	gtk_toggle_button_set_active(togglebutton,
															 userData_in.current_level.metadata.environment.outdoors);

  GtkTreeView* treeview =
      GTK_TREE_VIEW(glade_xml_get_widget(userData_in.XML,
                                         ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENCOUNTERS_MONSTERS_TREEVIEW_NAME)));
  ACE_ASSERT(treeview);
  GtkListStore* liststore = GTK_LIST_STORE(gtk_tree_view_get_model(treeview));
  ACE_ASSERT(liststore);
  gtk_list_store_clear(liststore);
  GtkTreeIter treeiterator;
  for (RPG_Engine_SpawnsConstIterator_t iterator = userData_in.current_level.metadata.spawns.begin();
       iterator != userData_in.current_level.metadata.spawns.end();
       iterator++)
  {
    // append new (text) entry
    gtk_list_store_append(liststore, &treeiterator);
    gtk_list_store_set(liststore, &treeiterator,
                       0, ACE_TEXT((*iterator).spawn.type.c_str()), // column 0
                       -1);
  } // end FOR

  GtkTextView* view =
      GTK_TEXT_VIEW(glade_xml_get_widget(userData_in.XML,
                                         ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_LAYOUT_MAP_TEXTVIEW_NAME)));
  ACE_ASSERT(view);
  GtkTextBuffer* buffer = gtk_text_view_get_buffer(view);
  ACE_ASSERT(buffer);
  GtkTextIter start, end;
  gtk_text_buffer_get_start_iter(buffer, &start);
  gtk_text_buffer_get_end_iter(buffer, &end);
  gtk_text_buffer_delete(buffer, &start, &end);

  GtkTextIter iterator;
  gtk_text_buffer_get_end_iter(buffer,
                               &iterator);

  std::string map_info = RPG_Map_Level::string(userData_in.current_level.map);
  map_info.resize(map_info.size() - 1);
  gchar* converted_text = NULL;
  converted_text =
      RPG_Client_UI_Tools::Locale2UTF8(map_info);
  if (!converted_text)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert map to text, returning\n")));

		return;
	} // end IF

	// display text
	gtk_text_buffer_insert(buffer,
												 &iterator,
												 converted_text,
												 -1);
	g_free(converted_text);

  // scroll the view accordingly
  // move the iterator to the beginning of line, so it doesn't scroll
  // in horizontal direction
  gtk_text_iter_set_line_offset(&iterator, 0);

  // ...and place the mark at iter. The mark will stay there after insertion
  // because it has "right" gravity
  GtkTextMark* mark =
      gtk_text_buffer_get_mark(buffer,
                               ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_SCROLLMARK_NAME));
  //  gtk_text_buffer_move_mark(buffer,
  //                            mark,
  //                            &iterator);

  // scroll the mark onscreen
  gtk_text_view_scroll_mark_onscreen(view,
                                     mark);
}

G_MODULE_EXPORT void
update_configuration(GTK_cb_data_t& userData_in)
{
	RPG_TRACE(ACE_TEXT("::update_configuration"));

	GtkToggleButton* togglebutton =
			GTK_TOGGLE_BUTTON(glade_xml_get_widget(userData_in.XML,
																						 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_CONFIGURATION_CORRIDORS_CHECKBUTTON_NAME)));
	ACE_ASSERT(togglebutton);
	gtk_toggle_button_set_active(togglebutton,
															 userData_in.map_configuration.corridors);

	togglebutton =
			GTK_TOGGLE_BUTTON(glade_xml_get_widget(userData_in.XML,
																						 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_CONFIGURATION_DOORS_CHECKBUTTON_NAME)));
	ACE_ASSERT(togglebutton);
	gtk_toggle_button_set_active(togglebutton,
															 userData_in.map_configuration.doors);

	GtkSpinButton* spinbutton =
			GTK_SPIN_BUTTON(glade_xml_get_widget(userData_in.XML,
																					 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_CONFIGURATION_SIZEX_SPINBUTTON_NAME)));
	ACE_ASSERT(spinbutton);
	gtk_spin_button_set_value(spinbutton,
														userData_in.map_configuration.map_size_x);

	spinbutton =
			GTK_SPIN_BUTTON(glade_xml_get_widget(userData_in.XML,
																					 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_CONFIGURATION_SIZEY_SPINBUTTON_NAME)));
	ACE_ASSERT(spinbutton);
	gtk_spin_button_set_value(spinbutton,
														userData_in.map_configuration.map_size_y);

	spinbutton =
			GTK_SPIN_BUTTON(glade_xml_get_widget(userData_in.XML,
																					 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_CONFIGURATION_MAXNUMDOORSROOM_SPINBUTTON_NAME)));
	ACE_ASSERT(spinbutton);
	gtk_spin_button_set_value(spinbutton,
														userData_in.map_configuration.max_num_doors_per_room);

	togglebutton =
			GTK_TOGGLE_BUTTON(glade_xml_get_widget(userData_in.XML,
																						 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_CONFIGURATION_MAXIMIZEROOMS_CHECKBUTTON_NAME)));
	ACE_ASSERT(togglebutton);
	gtk_toggle_button_set_active(togglebutton,
															 userData_in.map_configuration.maximize_rooms);

	spinbutton =
			GTK_SPIN_BUTTON(glade_xml_get_widget(userData_in.XML,
																					 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_CONFIGURATION_MINROOMSIZE_SPINBUTTON_NAME)));
	ACE_ASSERT(spinbutton);
	gtk_spin_button_set_value(spinbutton,
														userData_in.map_configuration.min_room_size);

	spinbutton =
			GTK_SPIN_BUTTON(glade_xml_get_widget(userData_in.XML,
																					 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_CONFIGURATION_NUMAREAS_SPINBUTTON_NAME)));
	ACE_ASSERT(spinbutton);
	gtk_spin_button_set_value(spinbutton,
														userData_in.map_configuration.num_areas);

	togglebutton =
			GTK_TOGGLE_BUTTON(glade_xml_get_widget(userData_in.XML,
																						 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_CONFIGURATION_SQUAREROOMS_CHECKBUTTON_NAME)));
	ACE_ASSERT(togglebutton);
	gtk_toggle_button_set_active(togglebutton,
															 userData_in.map_configuration.square_rooms);
}

G_MODULE_EXPORT gint
about_clicked_GTK_cb(GtkWidget* widget_in,
                     gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::about_clicked_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  GTK_cb_data_t* data = static_cast<GTK_cb_data_t*>(userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->XML);

  // retrieve about dialog handle
  GtkDialog* about_dialog =
      GTK_DIALOG(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_ABOUT_NAME)));
  ACE_ASSERT(about_dialog);

  gtk_dialog_run(about_dialog);
  gtk_widget_hide(GTK_WIDGET(about_dialog));
  //if (!GTK_WIDGET_VISIBLE(about_dialog))
  //  gtk_widget_show(about_dialog);

  return FALSE;
}

G_MODULE_EXPORT gint
quit_clicked_GTK_cb(GtkWidget* widget_in,
                    gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::quit_clicked_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  ACE_UNUSED_ARG(userData_in);

  // leave GTK
  gtk_main_quit();

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("leaving GTK...\n")));

  // this is the "delete-event" handler
  // --> destroy the main dialog widget
  return TRUE; // --> propagate
}

G_MODULE_EXPORT gint
create_map_clicked_GTK_cb(GtkWidget* widget_in,
                          gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::create_map_clicked_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);

  GTK_cb_data_t* data = static_cast<GTK_cb_data_t*>(userData_in);
  ACE_ASSERT(data);

	// read configuration
	GtkToggleButton* togglebutton =
			GTK_TOGGLE_BUTTON(glade_xml_get_widget(data->XML,
																						 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_CONFIGURATION_CORRIDORS_CHECKBUTTON_NAME)));
	ACE_ASSERT(togglebutton);
	data->map_configuration.corridors = gtk_toggle_button_get_active(togglebutton);

	togglebutton =
			GTK_TOGGLE_BUTTON(glade_xml_get_widget(data->XML,
																						 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_CONFIGURATION_DOORS_CHECKBUTTON_NAME)));
	ACE_ASSERT(togglebutton);
	data->map_configuration.doors = gtk_toggle_button_get_active(togglebutton);

	GtkSpinButton* spinbutton =
			GTK_SPIN_BUTTON(glade_xml_get_widget(data->XML,
																					 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_CONFIGURATION_SIZEX_SPINBUTTON_NAME)));
	ACE_ASSERT(spinbutton);
	data->map_configuration.map_size_x = gtk_spin_button_get_value_as_int(spinbutton);

	spinbutton =
			GTK_SPIN_BUTTON(glade_xml_get_widget(data->XML,
																					 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_CONFIGURATION_SIZEY_SPINBUTTON_NAME)));
	ACE_ASSERT(spinbutton);
	data->map_configuration.map_size_y = gtk_spin_button_get_value_as_int(spinbutton);

	spinbutton =
			GTK_SPIN_BUTTON(glade_xml_get_widget(data->XML,
																					 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_CONFIGURATION_MAXNUMDOORSROOM_SPINBUTTON_NAME)));
	ACE_ASSERT(spinbutton);
	data->map_configuration.max_num_doors_per_room =
			gtk_spin_button_get_value_as_int(spinbutton);

	togglebutton =
			GTK_TOGGLE_BUTTON(glade_xml_get_widget(data->XML,
																						 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_CONFIGURATION_MAXIMIZEROOMS_CHECKBUTTON_NAME)));
	ACE_ASSERT(togglebutton);
	data->map_configuration.maximize_rooms = gtk_toggle_button_get_active(togglebutton);

	spinbutton =
			GTK_SPIN_BUTTON(glade_xml_get_widget(data->XML,
																					 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_CONFIGURATION_MINROOMSIZE_SPINBUTTON_NAME)));
	ACE_ASSERT(spinbutton);
	data->map_configuration.min_room_size =
			gtk_spin_button_get_value_as_int(spinbutton);

	spinbutton =
			GTK_SPIN_BUTTON(glade_xml_get_widget(data->XML,
																					 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_CONFIGURATION_NUMAREAS_SPINBUTTON_NAME)));
	ACE_ASSERT(spinbutton);
	data->map_configuration.num_areas =
			gtk_spin_button_get_value_as_int(spinbutton);

	togglebutton =
			GTK_TOGGLE_BUTTON(glade_xml_get_widget(data->XML,
																						 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_CONFIGURATION_SQUAREROOMS_CHECKBUTTON_NAME)));
	ACE_ASSERT(togglebutton);
	data->map_configuration.square_rooms = gtk_toggle_button_get_active(togglebutton);

  RPG_Engine_Level::create(data->map_configuration,
                           data->current_level);
  data->is_transient = true;

  // update layout
  ::update_layout(*data);

  // make map display frame sensitive (if it's not already)
  GtkWidget* widget =
      GTK_WIDGET(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_LAYOUT_MAP_TEXTVIEW_NAME)));
  ACE_ASSERT(widget);
  gtk_widget_set_sensitive(widget, TRUE);
  // make drop button sensitive (if it's not already)
  GtkButton* button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_DROP_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
  // make save button sensitive (if it's not already)
  button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_STORE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
  // make load button in-sensitive (if it's not already)
  button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_LOAD_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
  // make this in-sensitive
  gtk_widget_set_sensitive(widget_in, FALSE);

  return FALSE;
}

G_MODULE_EXPORT gint
drop_map_clicked_GTK_cb(GtkWidget* widget_in,
                        gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::drop_map_clicked_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);

  GTK_cb_data_t* data = static_cast<GTK_cb_data_t*>(userData_in);
  ACE_ASSERT(data);

	// delete file if non-transient ?
	if (!data->is_transient)
	{
		// retrieve confirmation dialog handle
		GtkDialog* confirmation_dialog =
				GTK_DIALOG(glade_xml_get_widget(data->XML,
																				ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_CONFIRMATION_NAME)));
		ACE_ASSERT(confirmation_dialog);
		gint response = gtk_dialog_run(confirmation_dialog);
		gtk_widget_hide(GTK_WIDGET(confirmation_dialog));
		if (response == GTK_RESPONSE_OK)
		{
			// retrieve active item
			GtkComboBox* repository_combobox =
					GTK_COMBO_BOX(glade_xml_get_widget(data->XML,
																						 ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_MAP_NAME)));
			ACE_ASSERT(repository_combobox);
			std::string active_item;
			GtkTreeIter selected;
			GtkTreeModel* model = NULL;
			GValue value;
			const gchar* text = NULL;
			if (!gtk_combo_box_get_active_iter(repository_combobox, &selected))
			{
				ACE_DEBUG((LM_ERROR,
									 ACE_TEXT("failed to gtk_combo_box_get_active_iter(), aborting\n")));

				return FALSE;
			} // end IF
			model = gtk_combo_box_get_model(repository_combobox);
			ACE_ASSERT(model);
			ACE_OS::memset(&value, 0, sizeof(value));
			gtk_tree_model_get_value(model, &selected,
															 0, &value);
			text = g_value_get_string(&value);
			// sanity check
			ACE_ASSERT(text);
			active_item = ACE_TEXT_ALWAYS_CHAR(text);
			g_value_unset(&value);

			// construct filename
			std::string filename = RPG_Map_Common_Tools::getMapsDirectory();
			filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
			filename += active_item;
			filename += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT);

			// remove file
			if (!RPG_Common_File_Tools::deleteFile(filename))
				ACE_DEBUG((LM_ERROR,
									 ACE_TEXT("failed to RPG_Common_File_Tools::deleteFile(\"%s\"), aborting\n"),
									 ACE_TEXT(filename.c_str())));
			else
			{
				ACE_DEBUG((LM_DEBUG,
									 ACE_TEXT("deleted \"%s\"...\n"),
									 ACE_TEXT(filename.c_str())));

				// refresh combobox
				GtkButton* button =
						GTK_BUTTON(glade_xml_get_widget(data->XML,
																						ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_REFRESH_NAME)));
				ACE_ASSERT(button);
				g_signal_emit_by_name(button,
					                    ACE_TEXT_ALWAYS_CHAR("clicked"),
															userData_in);
			} // end IF
		} // end IF
		else
			return FALSE; // cancelled --> done
	} // end IF

  // clean up
  data->current_level.metadata.environment.plane    = RPG_COMMON_PLANE_INVALID;
  data->current_level.metadata.environment.terrain  = RPG_COMMON_TERRAIN_INVALID;
  data->current_level.metadata.environment.climate  = RPG_COMMON_CLIMATE_INVALID;
  data->current_level.metadata.environment.time     = RPG_COMMON_TIMEOFDAY_INVALID;
  data->current_level.metadata.environment.lighting = RPG_COMMON_AMBIENTLIGHTING_INVALID;
  data->current_level.metadata.environment.outdoors = false;

  data->current_level.metadata.max_num_spawned      = 0;
  data->current_level.metadata.name.clear();
  data->current_level.metadata.spawns.clear();

  ::reset_layout(*data);

	// load active combobox entry (if any)
	bool desensitize_frame = false;
	GtkComboBox* repository_combobox =
			GTK_COMBO_BOX(glade_xml_get_widget(data->XML,
																				 ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_MAP_NAME)));
	ACE_ASSERT(repository_combobox);
	if (gtk_combo_box_get_active(repository_combobox) >= 0)
		g_signal_emit_by_name(repository_combobox,
					                ACE_TEXT_ALWAYS_CHAR("changed"),
													userData_in);
	else
		desensitize_frame = true;

  // make character display frame in-sensitive ?
	if (desensitize_frame)
	{
		GtkWidget* widget =
				GTK_WIDGET(glade_xml_get_widget(data->XML,
																				ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_LAYOUT_MAP_TEXTVIEW_NAME)));
		ACE_ASSERT(widget);
		gtk_widget_set_sensitive(widget, FALSE);
	} // end IF
  // make create button sensitive
  GtkButton* button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_CREATE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
  // make save button in-sensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_STORE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
  // make load button sensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_LOAD_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
  // make this in-sensitive
  gtk_widget_set_sensitive(widget_in, FALSE);

  return FALSE;
}

G_MODULE_EXPORT gint
load_map_clicked_GTK_cb(GtkWidget* widget_in,
                        gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::load_map_clicked_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  GTK_cb_data_t* data = static_cast<GTK_cb_data_t*>(userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->XML);

  // *NOTE*: this callback just presents the file picker
  // --> business logic happens in map_file_actived_GTK_cb

  // retrieve file chooser dialog handle
  GtkFileChooserDialog* filechooser_dialog =
      GTK_FILE_CHOOSER_DIALOG(glade_xml_get_widget(data->XML,
                                                   ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_FILECHOOSER_NAME)));
  ACE_ASSERT(filechooser_dialog);
  if (!filechooser_dialog)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
               ACE_TEXT(RPG_CLIENT_GTK_DIALOG_FILECHOOSER_NAME)));

    return FALSE;
  } // end IF

  // draw it
  gtk_dialog_run(GTK_DIALOG(filechooser_dialog));
  gtk_widget_hide(GTK_WIDGET(filechooser_dialog));
  //if (!GTK_WIDGET_VISIBLE(filechooser_dialog))
  //  gtk_widget_show(GTK_WIDGET(filechooser_dialog));

  return FALSE;
}

G_MODULE_EXPORT gint
map_file_activated_GTK_cb(GtkWidget* widget_in,
                          gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::map_file_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  GTK_cb_data_t* data = static_cast<GTK_cb_data_t*>(userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->XML);

  // retrieve file chooser dialog handle
  GtkFileChooserDialog* filechooser_dialog =
      GTK_FILE_CHOOSER_DIALOG(glade_xml_get_widget(data->XML,
                                                   ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_FILECHOOSER_NAME)));
  ACE_ASSERT(filechooser_dialog);

  // hide widget
  gtk_widget_hide(GTK_WIDGET(filechooser_dialog));

  // load entity
  std::string filename(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooser_dialog)));
  if (!RPG_Engine_Level::load(filename,
                              data->schema_repository,
                              data->current_level))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Engine_Level::load(\"%s\"), aborting\n"),
               ACE_TEXT(filename).c_str()));

    return FALSE;

  } // end IF
  data->is_transient = false;

  // update entity profile widgets
  ::update_layout(*data);

  // make character display frame sensitive (if it's not already)
  GtkWidget* widget =
      GTK_WIDGET(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_LAYOUT_MAP_TEXTVIEW_NAME)));
  ACE_ASSERT(widget);
  gtk_widget_set_sensitive(widget, TRUE);
  // make create button in-sensitive (if it's not already)
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
  // make save button in-sensitive (if it's not already)
  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_STORE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  return FALSE;
}

G_MODULE_EXPORT gint
save_map_clicked_GTK_cb(GtkWidget* widget_in,
                        gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::save_map_clicked_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  GTK_cb_data_t* data = static_cast<GTK_cb_data_t*>(userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->XML);

  // get a name
  std::string name = ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_DEF_NAME);
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
                     ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_DEF_NAME));
  gtk_editable_select_region(GTK_EDITABLE(entry),
                             0, -1);
  // step1b: retrieve entry dialog handle
  GtkDialog* entry_dialog =
    GTK_DIALOG(glade_xml_get_widget(data->XML,
                                    ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_DIALOG_ENTRY_NAME)));
  ACE_ASSERT(entry_dialog);
  // step1c: run entry dialog
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
               ACE_TEXT("failed to convert string: \"%s\", aborting\n"),
               ACE_TEXT(conversion_error->message)));

    // clean up
    g_error_free(conversion_error);

    return FALSE;
  } // end IF
  name = converted_text;
  g_free(converted_text);

  // assemble target filename
  std::string filename = RPG_Map_Common_Tools::getMapsDirectory();
  filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  filename += name;
  filename += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT);
  if (!RPG_Engine_Level::save(filename,
                              data->current_level))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Engine_Level::save(\"%s\"), continuing\n"),
               ACE_TEXT(filename.c_str())));
	data->is_transient = false;

  // make new button sensitive
  GtkButton* button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_CREATE_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
  // make save button in-sensitive
  gtk_widget_set_sensitive(widget_in, FALSE);
  button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_LOAD_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

  return FALSE;
}

G_MODULE_EXPORT gint
map_repository_combobox_changed_GTK_cb(GtkWidget* widget_in,
                                       gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::map_repository_combobox_changed_GTK_cb"));

  GTK_cb_data_t* data = static_cast<GTK_cb_data_t*>(userData_in);
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
  active_item = ACE_TEXT_ALWAYS_CHAR(text);
  g_value_unset(&value);

  // construct filename
  std::string filename = RPG_Map_Common_Tools::getMapsDirectory();
  filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  filename += active_item;
  filename += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT);
  if (!RPG_Engine_Level::load(filename,
                              data->schema_repository,
                              data->current_level))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Engine_Level::load(\"%s\"), aborting\n"),
               ACE_TEXT(filename.c_str())));

		return FALSE;
	} // end IF
	data->is_transient = false;

	::update_layout(*data);

  // make map layout sensitive (if it's not already)
  GtkWidget* widget =
      GTK_WIDGET(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_LAYOUT_MAP_TEXTVIEW_NAME)));
  ACE_ASSERT(widget);
  gtk_widget_set_sensitive(widget, TRUE);
  // make drop button sensitive (if it's not already)
  GtkButton* button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_DROP_NAME)));
  ACE_ASSERT(button);
  gtk_widget_set_sensitive(GTK_WIDGET(button),
                           (active_item != RPG_Common_Tools::sanitize(ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_DEF_NAME))));
  // make save button in-sensitive (if it's not already)
  button =
      GTK_BUTTON(glade_xml_get_widget(data->XML,
                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_STORE_NAME)));
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
  GTK_cb_data_t* data = static_cast<GTK_cb_data_t*>(userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->XML);

  // retrieve tree model
  GtkComboBox* repository_combobox =
      GTK_COMBO_BOX(glade_xml_get_widget(data->XML,
                                         ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_MAP_NAME)));
  ACE_ASSERT(repository_combobox);
  GtkTreeModel* model = gtk_combo_box_get_model(repository_combobox);
  ACE_ASSERT(model);

  // re-load map data
  unsigned int num_entries = ::load_files(REPOSITORY_MAPS,
                                          GTK_LIST_STORE(model));

  // ... sensitize/activate widgets as appropriate
	gtk_widget_set_sensitive(GTK_WIDGET(repository_combobox), (num_entries > 0));
  if (num_entries)
    gtk_combo_box_set_active(repository_combobox, 0);

  return FALSE;
}

G_MODULE_EXPORT gint
encounter_selection_changed_GTK_cb(GtkWidget* widget_in,
                                   gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::encounter_selection_changed_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  GTK_cb_data_t* data = static_cast<GTK_cb_data_t*>(userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->XML);

  GtkTreeView* treeview =
        GTK_TREE_VIEW(glade_xml_get_widget(data->XML,
                                           ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENCOUNTERS_MONSTERS_TREEVIEW_NAME)));
  ACE_ASSERT(treeview);
  GtkTreeSelection* treeselection = gtk_tree_view_get_selection(treeview);
  ACE_ASSERT(treeselection);
//  GtkListStore* liststore = GTK_LIST_STORE(gtk_tree_view_get_model(treeview));
//  ACE_ASSERT(liststore);
  GtkTreeModel* treemodel = NULL;
  GtkTreeIter iterator;
  if (!gtk_tree_selection_get_selected(treeselection,
                                       &treemodel,
                                       &iterator))
    return FALSE; // nothing selected

  gchar* selected_item = NULL;
  gtk_tree_model_get(treemodel,
                     &iterator,
                     0, &selected_item,
                     -1);
//  g_print ("selected row is: %s\n", name);
  std::string selection = ACE_TEXT_ALWAYS_CHAR(selected_item);
  g_free(selected_item);
  RPG_Engine_SpawnsConstIterator_t iterator2 =
      data->current_level.metadata.spawns.begin();
  for (;
       iterator2 != data->current_level.metadata.spawns.end();
       iterator2++)
    if ((*iterator2).spawn.type == selection)
      break;
  ACE_ASSERT(iterator2 != data->current_level.metadata.spawns.end());

	GtkSpinButton* spinbutton =
			GTK_SPIN_BUTTON(glade_xml_get_widget(data->XML,
																					 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENCOUNTERS_SPAWNINTERVAL_SPINBUTTON_NAME)));
	ACE_ASSERT(spinbutton);
	gtk_spin_button_set_value(spinbutton,
														(*iterator2).spawn.interval.seconds);

	spinbutton =
			GTK_SPIN_BUTTON(glade_xml_get_widget(data->XML,
																					 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENCOUNTERS_SPAWNPROBABILITY_SPINBUTTON_NAME)));
	ACE_ASSERT(spinbutton);
	gtk_spin_button_set_value(spinbutton,
														(*iterator2).spawn.probability);

	spinbutton =
			GTK_SPIN_BUTTON(glade_xml_get_widget(data->XML,
																					 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENCOUNTERS_MAXNUMSPAWNED_SPINBUTTON_NAME)));
	ACE_ASSERT(spinbutton);
	gtk_spin_button_set_value(spinbutton,
														(*iterator2).spawn.max_num_spawned);

	spinbutton =
			GTK_SPIN_BUTTON(glade_xml_get_widget(data->XML,
																					 ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GNOME_ENCOUNTERS_AMBLEPROBABILITY_SPINBUTTON_NAME)));
	ACE_ASSERT(spinbutton);
	gtk_spin_button_set_value(spinbutton,
														(*iterator2).spawn.amble_probability);

	return FALSE;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

bool
init_UI_client(const std::string& UIFile_in,
               GTK_cb_data_t& userData_in)
{
  RPG_TRACE(ACE_TEXT("::init_UI_client"));

  return true;
}

void
fini_UI_client(GTK_cb_data_t& userData_in)
{
  RPG_TRACE(ACE_TEXT("::fini_UI_client"));

}

Map_Generator_GTKUIDefinition::Map_Generator_GTKUIDefinition(GTK_cb_data_t* GTKCBData_in)
: myGTKCBData(GTKCBData_in)
{
	RPG_TRACE(ACE_TEXT("Map_Generator_GTKUIDefinition::Map_Generator_GTKUIDefinition"));

	ACE_ASSERT(myGTKCBData);
}

Map_Generator_GTKUIDefinition::~Map_Generator_GTKUIDefinition()
{
	RPG_TRACE(ACE_TEXT("Map_Generator_GTKUIDefinition::~Map_Generator_GTKUIDefinition"));

}

bool
Map_Generator_GTKUIDefinition::init(const std::string& filename_in)
{
	RPG_TRACE(ACE_TEXT("Map_Generator_GTKUIDefinition::init"));

	return init_UI_client(filename_in,
												*myGTKCBData);
}

void
Map_Generator_GTKUIDefinition::fini()
{
	RPG_TRACE(ACE_TEXT("Map_Generator_GTKUIDefinition::fini"));

	fini_UI_client(*myGTKCBData);
}
