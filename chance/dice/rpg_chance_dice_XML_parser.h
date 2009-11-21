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
#ifndef RPG_CHANCE_DICE_XML_PARSER_H
#define RPG_CHANCE_DICE_XML_PARSER_H

#include "rpg_chance_dice_XML_parser_base.h"

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Chance_DiceType_Type
 : public RPG_Chance_DiceType_Type_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Chance_DiceType post_RPG_Chance_DiceType_Type();
};

class RPG_Chance_DiceRoll_Type
 : public RPG_Chance_DiceRoll_Type_pimpl
{
 public:
  RPG_Chance_DiceRoll_Type();

//   virtual void pre();
  virtual void numDice(unsigned int);
  virtual void typeDice(const RPG_Chance_DiceType&);
  virtual void modifier(long long);
  virtual RPG_Chance_DiceRoll post_RPG_Chance_DiceRoll_Type();

 private:
  RPG_Chance_DiceRoll myCurrentDiceRoll;
};

class RPG_Chance_ValueRange_Type
 : public RPG_Chance_ValueRange_Type_pimpl
{
 public:
  RPG_Chance_ValueRange_Type();

//   virtual void pre();
  virtual void begin(long long);
  virtual void end(long long);
  virtual RPG_Chance_ValueRange post_RPG_Chance_ValueRange_Type();

 private:
  RPG_Chance_ValueRange myCurrentValueRange;
};

#endif
