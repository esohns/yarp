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

#ifndef MAP_GENERATOR_GUI_CALLBACKS_H
#define MAP_GENERATOR_GUI_CALLBACKS_H

#include "map_generator_gui_common.h"

#include "rpg_client_iGTK_ui.h"

#include "rpg_map_common.h"

#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT void reset_layout(GTK_cb_data_t&);
G_MODULE_EXPORT void update_configuration(GTK_cb_data_t&);
G_MODULE_EXPORT void update_layout(GTK_cb_data_t&);

G_MODULE_EXPORT gint about_clicked_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint quit_clicked_GTK_cb(GtkWidget*, gpointer);
//
G_MODULE_EXPORT gint create_map_clicked_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint drop_map_clicked_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint load_map_clicked_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint map_file_activated_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint save_map_clicked_GTK_cb(GtkWidget*, gpointer);
//
G_MODULE_EXPORT gint map_repository_combobox_changed_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint map_repository_button_clicked_GTK_cb(GtkWidget*, gpointer);
//
//G_MODULE_EXPORT gint doors_checkbutton_toggled_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint corridors_checkbutton_toggled_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint size_x_spinbutton_value_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint size_y_spinbutton_value_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint max_num_doors_room_spinbutton_value_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint maximize_rooms_checkbutton_toggled_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint min_roomsize_spinbutton_value_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint num_areas_spinbutton_value_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint square_rooms_checkbutton_toggled_cb(GtkWidget*, gpointer);
////
//G_MODULE_EXPORT gint plane_combobox_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint terrain_combobox_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint climate_combobox_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint timeofday_combobox_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint lighting_combobox_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint outdoors_checkbutton_toggled_cb(GtkWidget*, gpointer);
////
G_MODULE_EXPORT gint encounter_selection_changed_GTK_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint spawn_interval_spinbutton_value_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint spawn_probability_spinbutton_value_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint max_num_spawned_spinbutton_value_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint amble_probability_spinbutton_value_changed_cb(GtkWidget*, gpointer);
//
G_MODULE_EXPORT gint map_layout_textview_changed_cb(GtkWidget*, gpointer);

#ifdef __cplusplus
}
#endif /* __cplusplus */

// GTK init functions
bool
init_UI_client(const std::string&, // UI definiton file
               GTK_cb_data_t&);    // GTK cb data (inout)
void
fini_UI_client(GTK_cb_data_t&); // GTK cb data (inout)

class Map_Generator_GTKUIDefinition
 : public RPG_Client_IGTKUI
{
 public:
  Map_Generator_GTKUIDefinition(GTK_cb_data_t*); // GTK cb data handle
  virtual ~Map_Generator_GTKUIDefinition();

	// implement RPG_Client_IGTKUI
	virtual bool init(const std::string&); // definiton filename
	virtual void fini();

 private:
  ACE_UNIMPLEMENTED_FUNC(Map_Generator_GTKUIDefinition());
  ACE_UNIMPLEMENTED_FUNC(Map_Generator_GTKUIDefinition(const Map_Generator_GTKUIDefinition&));
  ACE_UNIMPLEMENTED_FUNC(Map_Generator_GTKUIDefinition& operator=(const Map_Generator_GTKUIDefinition&));

  GTK_cb_data_t* myGTKCBData;
};

#endif
