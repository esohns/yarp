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
#ifndef RPG_CHARACTER_PLAYER_H
#define RPG_CHARACTER_PLAYER_H

#include "rpg_character_player_base.h"

#include <xsd/cxx/xml/error-handler.hxx>

/**
player character

  @author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Character_Player
 : public RPG_Character_Player_Base
{
 public:
  RPG_Character_Player(const std::string&,               // name
                       const RPG_Character_Gender&,      // gender
                       const RPG_Character_Race&,        // race
                       const RPG_Character_Class&,       // (starting) class(es)
                       const RPG_Character_Alignment&,   // (starting) alignment
                       const RPG_Character_Attributes&,  // base attributes
                       const RPG_Character_Skills_t&,    // (starting) skills
                       const RPG_Character_Feats_t&,     // base feats
                       const RPG_Character_Abilities_t&, // base abilities
                       const RPG_Character_OffHand&,     // off-hand
                       const unsigned int&,              // (starting) XP
                       const unsigned short int&,        // (starting) HP
                       const unsigned int&,              // (starting) wealth (GP)
                       const RPG_Magic_Spells_t&,        // (starting) set of known spells (if any)
                       const RPG_Magic_SpellList_t&,     // (starting) set of prepared spells (if any)
                       const RPG_Item_List_t&);          // (starting) list of (carried) items
  RPG_Character_Player(const RPG_Character_Player&);
  virtual ~RPG_Character_Player();

  RPG_Character_Player& operator=(const RPG_Character_Player&);

  // load/save player from/to an XML file
  static const RPG_Character_Player load(const std::string&); // FQ filename
  const bool save(const std::string&); // FQ filename

 private:
  typedef RPG_Character_Player_Base inherited;

  // private error handler
  class XSD_Error_Handler
   : public ::xsd::cxx::xml::error_handler<char>
  {
   public:
    virtual bool handle(const std::string&,                             // id
                        unsigned long,                                  // line
                        unsigned long,                                  // column
                        ::xsd::cxx::xml::error_handler<char>::severity, // severity
                        const std::string&);                            // message
  };

  // safety measures
  RPG_Character_Player();

  // helper methods
  void defaultEquip();

  static XSD_Error_Handler myXSDErrorHandler;
};

#endif
