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

#include <gtk/gtk.h>

#include "rpg_client_iGTK_ui.h"

#include "net_common.h"

// GTK callback functions
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT gboolean idle_update_log_display_cb(gpointer);
G_MODULE_EXPORT gboolean idle_update_info_display_cb(gpointer);
// -----------------------------------------------------------------------------
G_MODULE_EXPORT gint button_connect_clicked_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint button_close_clicked_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint button_close_all_clicked_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint button_ping_clicked_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint togglebutton_stress_toggled_cb(GtkWidget*, gpointer);
// -----------------------------------------------------------------------------
G_MODULE_EXPORT gint togglebutton_listen_toggled_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint button_report_clicked_cb(GtkWidget*, gpointer);
// -----------------------------------------------------------------------------
G_MODULE_EXPORT gint button_about_clicked_cb(GtkWidget*, gpointer);
G_MODULE_EXPORT gint button_quit_clicked_cb(GtkWidget*, gpointer);
#ifdef __cplusplus
}
#endif /* __cplusplus */

// GTK init functions
bool init_UI_client(const std::string&, // UI definiton file
                    Net_GTK_CBData_t&); // GTK cb data (inout)
bool init_UI_server(const std::string&, // UI definiton file
	                  const bool&,        // allow runtime stats trigger ?
                    Net_GTK_CBData_t&); // GTK cb data (inout)
void fini_UI(Net_GTK_CBData_t&); // GTK cb data (inout)

class Net_GTKUIDefinition
 : public RPG_Client_IGTKUI
{
 public:
	enum Role_t
	{
		ROLE_CLIENT = 0,
		ROLE_SERVER,
		// ---------------------
		ROLE_MAX,
		ROLE_INVALID = -1
	};

  Net_GTKUIDefinition(const Role_t&,      // role
		                  const bool&,        // allow runtime user interaction ?
		                  Net_GTK_CBData_t*); // GTK cb data
  virtual ~Net_GTKUIDefinition();

	// implement RPG_Client_IGTKUI
	virtual bool init(const std::string&); // definiton filename
	virtual void fini();

 private:
  ACE_UNIMPLEMENTED_FUNC(Net_GTKUIDefinition());
  ACE_UNIMPLEMENTED_FUNC(Net_GTKUIDefinition(const Net_GTKUIDefinition&));
  ACE_UNIMPLEMENTED_FUNC(Net_GTKUIDefinition& operator=(const Net_GTKUIDefinition&));

	Role_t            myRole;
	bool              myAllowRuntimeInteraction;
	Net_GTK_CBData_t* myGTKCBData;
};

#endif
