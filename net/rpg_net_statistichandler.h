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

#ifndef RPG_NET_STATISTICHANDLER_H
#define RPG_NET_STATISTICHANDLER_H

#include <rpg_common_istatistic.h>

#include <ace/Global_Macros.h>
#include <ace/Event_Handler.h>
#include <ace/Time_Value.h>

template <typename StatisticsInfoContainer_t>
class RPG_Net_StatisticHandler
 : public ACE_Event_Handler
{
 public:
   // define different types of Actions for this interface
  enum ActionSpecifier
  {
    ACTION_REPORT = 0,
    ACTION_COLLECT,
  };

  typedef RPG_Common_IStatistic<StatisticsInfoContainer_t> COLLECTOR_TYPE;

  RPG_Net_StatisticHandler(const COLLECTOR_TYPE*,   // interface handle
                           const ActionSpecifier&); // action: collect/report
  virtual ~RPG_Net_StatisticHandler();

  // implement specific behaviour
  virtual int handle_timeout(const ACE_Time_Value&, // current time
                             const void*);          // asynchronous completion token

 private:
  typedef ACE_Event_Handler inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_StatisticHandler());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_StatisticHandler(const RPG_Net_StatisticHandler&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_StatisticHandler& operator=(const RPG_Net_StatisticHandler&));

  const COLLECTOR_TYPE* myInterface;
  ActionSpecifier       myAction;
};

// include template implementation
#include "rpg_net_statistichandler.i"

#endif
