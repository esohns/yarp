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

#ifndef CHARACTER_GENERATOR_GUI_COMMON_H
#define CHARACTER_GENERATOR_GUI_COMMON_H

#include "rpg_engine_common.h"

#include "rpg_graphics_sprite.h"

#include "glade/glade.h"

#include <string>
#include <vector>
#include <map>

#define CHARACTER_GENERATOR_GTK_BUTTON_IMAGENEXT_NAME "next_button"
#define CHARACTER_GENERATOR_GTK_BUTTON_IMAGEPREV_NAME "previous_button"
#define CHARACTER_GENERATOR_GTK_HBOX_NAME             "hbox"

#define CHARACTER_GENERATOR_GTK_GLADE_FILE            "character_generator_gui.glade"

typedef std::vector<RPG_Graphics_Sprite> Character_Generator_GUI_SpriteGallery_t;
typedef Character_Generator_GUI_SpriteGallery_t::const_iterator Character_Generator_GUI_SpriteGalleryIterator_t;

typedef std::vector<std::string> Character_Generator_XMLFiles_t;
typedef Character_Generator_XMLFiles_t::const_iterator Character_Generator_XMLFilesConstIterator_t;
typedef std::map<std::string, GladeXML*> Character_Generator_XMLPool_t;
typedef Character_Generator_XMLPool_t::iterator Character_Generator_XMLPoolIterator_t;

struct GTK_cb_data_t
{
	Character_Generator_XMLPool_t                   XML_pool;
	std::string                                     schema_repository;
  RPG_Engine_Entity_t                             entity;
  RPG_Graphics_Sprite                             current_sprite;
  Character_Generator_GUI_SpriteGalleryIterator_t sprite_gallery_iterator;
  Character_Generator_GUI_SpriteGallery_t         sprite_gallery;
	bool                                            is_transient;
};

#define CHARACTER_GENERATOR_DEF_ENTITY_SPRITE SPRITE_HUMAN

#endif
