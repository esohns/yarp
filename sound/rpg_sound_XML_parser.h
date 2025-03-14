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

#ifndef RPG_SOUND_XML_PARSER_H
#define RPG_SOUND_XML_PARSER_H

#include "ace/Global_Macros.h"

#include "rpg_XMLSchema_XML_types.h"

#include "rpg_sound_common.h"
#include "rpg_sound_XML_types.h"

class RPG_Sound_Category_Type
 : public RPG_Sound_Category_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual enum RPG_Sound_Category post_RPG_Sound_Category_Type ();
};

class RPG_Sound_Event_Type
 : public RPG_Sound_Event_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual enum RPG_Sound_Event post_RPG_Sound_Event_Type ();
};

class RPG_Sound_Type
 : public RPG_Sound_Type_pskel
{
 public:
  RPG_Sound_Type ();

//     virtual void pre();
  virtual void category (const RPG_Sound_Category&);
  virtual void sound_event (const RPG_Sound_Event&);
  virtual void volume (unsigned char);
  virtual void file (const ::std::string&);
  virtual void interval (unsigned char);
  virtual struct RPG_Sound post_RPG_Sound_Type ();

 private:
  struct RPG_Sound myCurrentSound;
};

class RPG_Sound_Dictionary_Type
 : public RPG_Sound_Dictionary_Type_pskel
{
 public:
  RPG_Sound_Dictionary_Type (RPG_Sound_Dictionary_t*); // sound dictionary
  inline virtual ~RPG_Sound_Dictionary_Type () {}

//   virtual void pre();
  virtual void sound (const struct RPG_Sound&);
  virtual void post_RPG_Sound_Dictionary_Type ();

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Sound_Dictionary_Type ());
  ACE_UNIMPLEMENTED_FUNC (RPG_Sound_Dictionary_Type (const RPG_Sound_Dictionary_Type&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Sound_Dictionary_Type& operator= (const RPG_Sound_Dictionary_Type&));

  RPG_Sound_Dictionary_t* myDictionary;
};

#endif
