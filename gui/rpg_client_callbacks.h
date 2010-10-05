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

#include <rpg_character_player.h>

#include <glade/glade.h>
#include <gtk/gtk.h>

#include <string>

void update_character_profile(const RPG_Character_Player&, GladeXML*);
void load_character_profiles(const std::string&, GtkListStore*);

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
  // main
  gint about_activated_GTK_cb(GtkWidget*, gpointer);
  gint properties_activated_GTK_cb(GtkWidget*, gpointer);
  gint quit_activated_GTK_cb(GtkWidget*, gpointer);
  gint create_character_activated_GTK_cb(GtkWidget*, gpointer);
  gint load_character_activated_GTK_cb(GtkWidget*, gpointer);
  gint character_file_activated_GTK_cb(GtkWidget*, gpointer);
  gint save_character_activated_GTK_cb(GtkWidget*, gpointer);
  gint join_game_activated_GTK_cb(GtkWidget*, gpointer);
  gint characters_activated_GTK_cb(GtkWidget*, gpointer);
  gint characters_refresh_activated_GTK_cb(GtkWidget*, gpointer);
  gint do_SDLEventLoop_GTK_cb(gpointer);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
