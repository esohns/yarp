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

#ifndef NET_CALLBACKS_H
#define NET_CALLBACKS_H

#include "gtk/gtk.h"

gboolean idle_finalize_UI_cb (gpointer);
gboolean idle_update_info_display_cb (gpointer);
gboolean idle_update_log_display_cb (gpointer);

//////////////////////////////////////////

gboolean idle_initialize_client_UI_cb (gpointer);

//////////////////////////////////////////

gboolean idle_initialize_server_UI_cb (gpointer);

//------------------------------------------------------------------------------

// GTK callback functions
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT gint button_close_clicked_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint button_close_all_clicked_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint button_connect_clicked_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint togglebutton_listen_toggled_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint button_report_clicked_cb (GtkWidget*, gpointer);
// -----------------------------------------------------------------------------
G_MODULE_EXPORT gint button_about_clicked_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint button_quit_clicked_cb (GtkWidget*, gpointer);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
