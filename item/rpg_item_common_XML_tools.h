/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns                                      *
 *   erik.sohns@web.de                                                     *
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

#ifndef RPG_ITEM_COMMON_XML_TOOLS_H
#define RPG_ITEM_COMMON_XML_TOOLS_H

#include "rpg_item_exports.h"
#include "rpg_item_instance_common.h"
#include "rpg_item_XML_tree.h"

#include <string>

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Item_Export RPG_Item_Common_XML_Tools
{
 public:
  static RPG_Item_List_t instantiate(const RPG_Item_InventoryXML_XMLTree_Type&);

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Common_XML_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Item_Common_XML_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Common_XML_Tools(const RPG_Item_Common_XML_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Common_XML_Tools& operator=(const RPG_Item_Common_XML_Tools&));
};

#endif
