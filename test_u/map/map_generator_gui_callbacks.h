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

#include "gtk/gtk.h"

#include "map_generator_gui_common.h"

void reset_layout(GTK_cb_data_t&);
void update_configuration(GTK_cb_data_t&);
void update_layout(GTK_cb_data_t&);

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT gboolean idle_initialize_UI_cb (gpointer);
G_MODULE_EXPORT gboolean idle_finalize_UI_cb (gpointer);
// -----------------------------------------------------------------------------
G_MODULE_EXPORT gint about_clicked_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint quit_clicked_GTK_cb(GtkWidget*, gpointer);
//------------------------------------------------------------------------------
G_MODULE_EXPORT gint create_map_clicked_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint drop_map_clicked_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint load_map_clicked_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint map_file_activated_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint save_map_clicked_GTK_cb(GtkWidget*, gpointer);
//------------------------------------------------------------------------------
G_MODULE_EXPORT gint map_repository_combobox_changed_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint map_repository_button_clicked_GTK_cb(GtkWidget*, gpointer);
//------------------------------------------------------------------------------
//G_MODULE_EXPORT gint doors_checkbutton_toggled_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint corridors_checkbutton_toggled_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint size_x_spinbutton_value_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint size_y_spinbutton_value_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint max_num_doors_room_spinbutton_value_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint maximize_rooms_checkbutton_toggled_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint min_roomsize_spinbutton_value_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint num_areas_spinbutton_value_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint square_rooms_checkbutton_toggled_cb(GtkWidget*, gpointer);
////------------------------------------------------------------------------------
//G_MODULE_EXPORT gint plane_combobox_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint terrain_combobox_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint climate_combobox_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint timeofday_combobox_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint lighting_combobox_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint outdoors_checkbutton_toggled_cb(GtkWidget*, gpointer);
////------------------------------------------------------------------------------
G_MODULE_EXPORT gint encounter_selection_changed_GTK_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint spawn_interval_spinbutton_value_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint spawn_probability_spinbutton_value_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint max_num_spawned_spinbutton_value_changed_cb(GtkWidget*, gpointer);
//G_MODULE_EXPORT gint amble_probability_spinbutton_value_changed_cb(GtkWidget*, gpointer);
//--------------------------------------------------------------------------------
G_MODULE_EXPORT gint map_layout_textview_changed_cb(GtkWidget*, gpointer);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
