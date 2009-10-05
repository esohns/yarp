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
#ifndef RPG_CHANCE_DICE_COMMON_H
#define RPG_CHANCE_DICE_COMMON_H

enum RPG_Chance_Dice_Type
{
  D_4   = 4,
  D_6   = 6,
  D_8   = 8,
  D_10  = 10,
  D_12  = 12,
  D_20  = 20,
  D_100 = 100
};

struct RPG_Chance_Roll
{
  unsigned int         numDice;  // number of dice to roll
  RPG_Chance_Dice_Type typeDice; // the type of dice to roll
  int                  modifier; // +/- x modifier (e.g. 2D4 + 4)
};

#endif
