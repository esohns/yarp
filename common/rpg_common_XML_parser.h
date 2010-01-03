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
#ifndef RPG_COMMON_XML_PARSER_H
#define RPG_COMMON_XML_PARSER_H

#include "rpg_common_XML_parser_base.h"

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Common_Attribute_Type
 : public RPG_Common_Attribute_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Common_Attribute post_RPG_Common_Attribute_Type();
};

class RPG_Common_SavingThrow_Type
 : public RPG_Common_SavingThrow_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Common_SavingThrow post_RPG_Common_SavingThrow_Type();
};

class RPG_Common_SavingThrowCheck_Type
 : public RPG_Common_SavingThrowCheck_Type_pimpl
{
 public:
  RPG_Common_SavingThrowCheck_Type();

//   virtual void pre();
  virtual void type(const RPG_Common_SavingThrow&);
  virtual void attribute(const RPG_Common_Attribute&);
  virtual void difficultyClass(unsigned char);
  virtual RPG_Common_SavingThrowCheck post_RPG_Common_SavingThrowCheck_Type();

 private:
  RPG_Common_SavingThrowCheck myCurrentSavingThrowCheck;
};

#endif