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

#ifndef CHARACTER_GENERATOR_GUI_CALLBACKS_H
#define CHARACTER_GENERATOR_GUI_CALLBACKS_H

#include "character_generator_gui_common.h"

#include <gtk/gtk.h>

void update_sprite_gallery(GTK_cb_data_t&);
void set_current_image(const RPG_Graphics_Sprite&,
                       GladeXML*);

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
  gint about_activated_GTK_cb(GtkWidget*, gpointer);
  gint quit_activated_GTK_cb(GtkWidget*, gpointer);
  gint create_character_activated_GTK_cb(GtkWidget*, gpointer);
  gint drop_character_activated_GTK_cb(GtkWidget*, gpointer);
  gint load_character_activated_GTK_cb(GtkWidget*, gpointer);
  gint character_file_activated_GTK_cb(GtkWidget*, gpointer);
  gint save_character_activated_GTK_cb(GtkWidget*, gpointer);
  gint character_repository_combobox_changed_GTK_cb(GtkWidget*, gpointer);
  gint character_repository_button_clicked_GTK_cb(GtkWidget*, gpointer);
  gint prev_image_activated_GTK_cb(GtkWidget*, gpointer);
  gint next_image_activated_GTK_cb(GtkWidget*, gpointer);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
