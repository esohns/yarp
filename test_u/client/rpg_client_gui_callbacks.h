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

#ifndef RPG_CLIENT_GUI_CALLBACKS_H
#define RPG_CLIENT_GUI_CALLBACKS_H

//#include "glade/glade.h"
#include "gtk/gtk.h"

#include "rpg_player.h"

#include "rpg_engine_common.h"

#include "rpg_client_common.h"

void update_equipment (const struct RPG_Client_GTK_CBData&);
void update_character_profile (const RPG_Player&, GtkBuilder*);
void update_entity_profile (const struct RPG_Engine_Entity&, GtkBuilder*);
void reset_character_profile (GtkBuilder*);
unsigned int load_files (const RPG_Client_Repository&, // repository
                         GtkListStore*);               // target liststore
gint combobox_sort_function (GtkTreeModel*, // model
                             GtkTreeIter*,  // row 1
                             GtkTreeIter*,  // row 2
                             gpointer);     // user data

// -----------------------------------------------------------------------------

gboolean idle_initialize_UI_cb (gpointer);
gboolean idle_finalize_UI_cb (gpointer);
gboolean idle_raise_UI_cb (gpointer);

// -----------------------------------------------------------------------------

// GTK callback functions
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT gint create_character_clicked_GTK_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint drop_character_clicked_GTK_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint load_character_clicked_GTK_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint save_character_clicked_GTK_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint character_repository_combobox_changed_GTK_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint character_repository_button_clicked_GTK_cb (GtkWidget*, gpointer);
//
G_MODULE_EXPORT gint create_map_clicked_GTK_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint drop_map_clicked_GTK_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint load_map_clicked_GTK_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint save_map_clicked_GTK_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint map_repository_combobox_changed_GTK_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint map_repository_button_clicked_GTK_cb (GtkWidget*, gpointer);
//
G_MODULE_EXPORT gint load_state_clicked_GTK_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint save_state_clicked_GTK_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint state_repository_combobox_changed_GTK_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint state_repository_button_clicked_GTK_cb (GtkWidget*, gpointer);
//
G_MODULE_EXPORT void togglebutton_join_part_toggled_cb (GtkToggleButton*, gpointer);
G_MODULE_EXPORT gint server_repository_combobox_changed_GTK_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint server_repository_button_clicked_GTK_cb (GtkWidget*, gpointer);
//
G_MODULE_EXPORT gint equip_clicked_GTK_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT void equipment_dialog_response_cb (GtkDialog*, gint, gpointer);
G_MODULE_EXPORT gint item_toggled_GTK_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint rest_clicked_GTK_cb (GtkWidget*, gpointer);
//
G_MODULE_EXPORT gint about_clicked_GTK_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint quit_clicked_GTK_cb (GtkWidget*, gpointer);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
