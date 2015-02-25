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

#include "ace/Global_Macros.h"
#include "ace/Event_Handler.h"
#include "ace/Asynch_IO.h"

#include "common_istatistic.h"

template <typename StatisticsInfoContainer_t>
class RPG_Net_StatisticHandler_Reactor
 : public ACE_Event_Handler
{
 public:
  enum ActionSpecifier_t
  {
    ACTION_REPORT = 0,
    ACTION_COLLECT
  };

  typedef Common_IStatistic<StatisticsInfoContainer_t> COLLECTOR_TYPE;

  RPG_Net_StatisticHandler_Reactor(const COLLECTOR_TYPE*,     // interface handle
                                   const ActionSpecifier_t&); // handler action
  virtual ~RPG_Net_StatisticHandler_Reactor();

  // implement specific behaviour
  virtual int handle_timeout(const ACE_Time_Value&, // current time
                             const void*);          // asynchronous completion token

 private:
  typedef ACE_Event_Handler inherited;

  ACE_UNIMPLEMENTED_FUNC(RPG_Net_StatisticHandler_Reactor());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_StatisticHandler_Reactor(const RPG_Net_StatisticHandler_Reactor&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_StatisticHandler_Reactor& operator=(const RPG_Net_StatisticHandler_Reactor&));

  const COLLECTOR_TYPE* myInterface;
  ActionSpecifier_t     myAction;
};

////////////////////////////////////////////////////////////////////////////////

template <typename StatisticsInfoContainer_t>
class RPG_Net_StatisticHandler_Proactor
 : public ACE_Handler
{
 public:
  enum ActionSpecifier_t
  {
    ACTION_REPORT = 0,
    ACTION_COLLECT
  };

  typedef RPG_Common_IStatistic<StatisticsInfoContainer_t> COLLECTOR_TYPE;

  RPG_Net_StatisticHandler_Proactor(const COLLECTOR_TYPE*,     // interface handle
                                    const ActionSpecifier_t&); // handler action
  virtual ~RPG_Net_StatisticHandler_Proactor();

  // implement specific behaviour
  virtual void handle_time_out(const ACE_Time_Value&, // current time
                               const void* = NULL);   // asynchronous completion token

 private:
  typedef ACE_Handler inherited;

  ACE_UNIMPLEMENTED_FUNC(RPG_Net_StatisticHandler_Proactor());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_StatisticHandler_Proactor(const RPG_Net_StatisticHandler_Proactor&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_StatisticHandler_Proactor& operator=(const RPG_Net_StatisticHandler_Proactor&));

  const COLLECTOR_TYPE* myInterface;
  ActionSpecifier_t     myAction;
};

// include template implementation
#include "rpg_net_statistichandler.inl"

#endif
