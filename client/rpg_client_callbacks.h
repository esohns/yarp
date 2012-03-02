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

#ifndef RPG_CLIENT_CALLBACKS_H
#define RPG_CLIENT_CALLBACKS_H

#include "rpg_client_exports.h"

#include <rpg_engine_common.h>

#include <rpg_character_player.h>

#include <glade/glade.h>
#include <gtk/gtk.h>

#include <string>

RPG_Client_Export void update_character_profile(const RPG_Character_Player&, GladeXML*);
RPG_Client_Export void update_entity_profile(const RPG_Engine_Entity&, GladeXML*);
RPG_Client_Export void reset_entity_profile(GladeXML*);
// int dirent_selector_profiles(const dirent*);
// int dirent_selector_maps(const dirent*);
// int dirent_comparator(const dirent**,
//                       const dirent**);
RPG_Client_Export const unsigned long load_files(const std::string&, // directory
                                                 const bool&,        // load player profiles ? : maps
                                                 GtkListStore*);     // target liststore

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT gint about_clicked_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint quit_clicked_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint create_character_clicked_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint drop_character_clicked_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint load_character_clicked_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint save_character_clicked_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint character_repository_combobox_changed_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint character_repository_button_clicked_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint create_map_clicked_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint drop_map_clicked_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint load_map_clicked_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint save_map_clicked_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint map_repository_combobox_changed_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint map_repository_button_clicked_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint join_game_clicked_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint part_game_clicked_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint server_repository_combobox_changed_GTK_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint server_repository_button_clicked_GTK_cb(GtkWidget*, gpointer);
//   gint do_SDLEventLoop_GTK_cb(gpointer);
//   gboolean gtk_quit_handler_cb(gpointer);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
