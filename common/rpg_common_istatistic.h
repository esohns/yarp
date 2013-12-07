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

#ifndef RPG_COMMON_ISTATISTIC_H
#define RPG_COMMON_ISTATISTIC_H

template <typename StatisticsInfoContainer_Type>
class RPG_Common_IStatistic
{
 public:
  virtual ~RPG_Common_IStatistic() {}

  // exposed interface
  // *NOTE*: the argument MAY act both as input/output,
  // this depends on the implementation...
  virtual bool collect(StatisticsInfoContainer_Type&) const = 0;
  virtual void report() const = 0;
};

#endif
