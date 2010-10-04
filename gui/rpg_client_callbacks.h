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

  // IRC_Client_GUI_Connection
  void disconnect_clicked_cb(GtkWidget*, gpointer);
  gboolean nick_entry_kb_focused_cb(GtkWidget*, GdkEventFocus*, gpointer);
  void change_clicked_cb(GtkWidget*, gpointer);
  void usersbox_changed_cb(GtkWidget*, gpointer);
  void refresh_users_clicked_cb(GtkWidget*, gpointer);
  gboolean channel_entry_kb_focused_cb(GtkWidget*, GdkEventFocus*, gpointer);
  void join_clicked_cb(GtkWidget*, gpointer);
  void channelbox_changed_cb(GtkWidget*, gpointer);
  void refresh_channels_clicked_cb(GtkWidget*, gpointer);
  void user_mode_toggled_cb(GtkToggleButton*, gpointer);
  void switch_channel_cb(GtkNotebook*, GtkNotebookPage*, guint, gpointer);
  void action_away_cb(GtkAction*, gpointer);

  // IRC_Client_GUI_MessageHandler
  void channel_mode_toggled_cb(GtkToggleButton*, gpointer);
  void topic_clicked_cb(GtkWidget*, GdkEventButton*, gpointer);
  void part_clicked_cb(GtkWidget*, gpointer);
  gboolean members_clicked_cb(GtkWidget*, GdkEventButton*, gpointer);
  void action_msg_cb(GtkAction*, gpointer);
  void action_invite_cb(GtkAction*, gpointer);
  void action_info_cb(GtkAction*, gpointer);
  void action_kick_cb(GtkAction*, gpointer);
  void action_ban_cb(GtkAction*, gpointer);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
