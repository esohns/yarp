/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
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

#ifndef MAP_GENERATOR_GUI_COMMON_H
#define MAP_GENERATOR_GUI_COMMON_H

#include <string>

#include "common_ui_gtk_common.h"

#include "rpg_map_common.h"

#include "rpg_engine_level.h"

// gtk widget names
#define MAP_GENERATOR_GNOME_CONFIGURATION_DOORS_CHECKBUTTON_NAME          "doors_checkbutton"
#define MAP_GENERATOR_GNOME_CONFIGURATION_CORRIDORS_CHECKBUTTON_NAME      "corridors_checkbutton"
#define MAP_GENERATOR_GNOME_CONFIGURATION_SIZEX_SPINBUTTON_NAME           "size_x_spinbutton"
#define MAP_GENERATOR_GNOME_CONFIGURATION_SIZEY_SPINBUTTON_NAME           "size_y_spinbutton"
#define MAP_GENERATOR_GNOME_CONFIGURATION_MAXNUMDOORSROOM_SPINBUTTON_NAME "max_num_doors_room_spinbutton"
#define MAP_GENERATOR_GNOME_CONFIGURATION_MAXIMIZEROOMS_CHECKBUTTON_NAME  "maximize_rooms_checkbutton"
#define MAP_GENERATOR_GNOME_CONFIGURATION_MINROOMSIZE_SPINBUTTON_NAME     "min_roomsize_spinbutton"
#define MAP_GENERATOR_GNOME_CONFIGURATION_NUMAREAS_SPINBUTTON_NAME        "num_areas_spinbutton"
#define MAP_GENERATOR_GNOME_CONFIGURATION_SQUAREROOMS_CHECKBUTTON_NAME    "square_rooms_checkbutton"
#define MAP_GENERATOR_GNOME_ENVIRONMENT_PLANE_COMBOBOX_NAME               "plane_combobox"
#define MAP_GENERATOR_GNOME_ENVIRONMENT_TERRAIN_COMBOBOX_NAME             "terrain_combobox"
#define MAP_GENERATOR_GNOME_ENVIRONMENT_CLIMATE_COMBOBOX_NAME             "climate_combobox"
#define MAP_GENERATOR_GNOME_ENVIRONMENT_TIMEOFDAY_COMBOBOX_NAME           "time_of_day_combobox"
#define MAP_GENERATOR_GNOME_ENVIRONMENT_LIGHTING_COMBOBOX_NAME            "lighting_combobox"
#define MAP_GENERATOR_GNOME_ENVIRONMENT_OUTDOORS_CHECKBUTTON_NAME         "outdoors_checkbutton"
#define MAP_GENERATOR_GNOME_ENCOUNTERS_MONSTERS_TREEVIEW_NAME             "monsters_treeview"
#define MAP_GENERATOR_GNOME_ENCOUNTERS_SPAWNINTERVAL_SPINBUTTON_NAME      "spawn_interval_spinbutton"
#define MAP_GENERATOR_GNOME_ENCOUNTERS_SPAWNPROBABILITY_SPINBUTTON_NAME   "spawn_probability_spinbutton"
#define MAP_GENERATOR_GNOME_ENCOUNTERS_MAXNUMSPAWNED_SPINBUTTON_NAME      "max_num_spawned_spinbutton"
#define MAP_GENERATOR_GNOME_ENCOUNTERS_AMBLEPROBABILITY_SPINBUTTON_NAME   "amble_probability_spinbutton"
#define MAP_GENERATOR_GNOME_LAYOUT_MAP_TEXTVIEW_NAME                      "map_layout_textview"
//#define MAP_GENERATOR_GNOME_LAYOUT_BOX_NAME                               "map_hbox"
#define MAP_GENERATOR_GNOME_DIALOG_ENTRY_NAME                             "map_layout_textview"

#define MAP_GENERATOR_GNOME_SCROLLMARK_NAME                               "insert"

// widget style
#define MAP_GENERATOR_GNOME_TEXTVIEW_FONTDESCRIPTION                      "Monospace 9"
#define MAP_GENERATOR_GNOME_TEXTVIEW_BASE                                 "#000000" // black
#define MAP_GENERATOR_GNOME_TEXTVIEW_TEXT                                 "#FFFFFF" // white

struct GTK_CBData
{
  struct RPG_Engine_LevelData            currentLevel;
  bool                                   isTransient;
  Common_UI_GTK_State_t*                 GTKState;
  struct RPG_Map_FloorPlan_Configuration mapConfiguration;
  std::string                            schemaRepository;
};

#endif
