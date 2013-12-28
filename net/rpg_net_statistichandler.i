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

#include <ace/Reactor.h>
#include <ace/Proactor.h>
#include <ace/Log_Msg.h>

#include "rpg_common_macros.h"

template <typename StatisticsInfoContainer_t>
RPG_Net_StatisticHandler_Reactor_T<StatisticsInfoContainer_t>::RPG_Net_StatisticHandler_Reactor_T(const COLLECTOR_TYPE* interface_in,
                                                                                                  const ActionSpecifier& action_in)
 : inherited(ACE_Reactor::instance(),         // use default reactor
             ACE_Event_Handler::LO_PRIORITY), // priority
   myInterface(interface_in),
   myAction(action_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StatisticHandler_Reactor_T::RPG_Net_StatisticHandler_Reactor_T"));

}

template <typename StatisticsInfoContainer_t>
RPG_Net_StatisticHandler_Reactor_T<StatisticsInfoContainer_t>::~RPG_Net_StatisticHandler_Reactor_T()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StatisticHandler_Reactor_T::~RPG_Net_StatisticHandler_Reactor_T"));

}

template <typename StatisticsInfoContainer_t>
int
RPG_Net_StatisticHandler_Reactor_T<StatisticsInfoContainer_t>::handle_timeout(const ACE_Time_Value& tv_in,
                                                                              const void* arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StatisticHandler_Reactor_T::handle_timeout"));

  ACE_UNUSED_ARG(tv_in);
  ACE_UNUSED_ARG(arg_in);

  switch (myAction)
  {
    case ACTION_COLLECT:
    {
      StatisticsInfoContainer_t result;
      ACE_OS::memset(&result, 0, sizeof(StatisticsInfoContainer_t));

      try
      {
        if (!myInterface->collect(result))
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to RPG_Common_IStatistic::collect(), continuing\n")));
      }
      catch (...)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("caught an exception in RPG_Common_IStatistic::collect(), continuing\n")));

      }

      // *TODO*: what else can we do, dump the result somehow ?
      break;
    }
    case ACTION_REPORT:
    {
      try
      {
        myInterface->report();
      }
      catch (...)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("caught an exception in RPG_Common_IStatistic::report(), continuing\n")));
      }

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("unknown/invalid action %u, aborting\n"),
                 myAction));

      return -1;
    }
  } // end SWITCH

  // reschedule timer...
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

template <typename StatisticsInfoContainer_t>
RPG_Net_StatisticHandler_Proactor_T<StatisticsInfoContainer_t>::RPG_Net_StatisticHandler_Proactor_T(const COLLECTOR_TYPE* interface_in,
                                                                                                    const ActionSpecifier& action_in)
 : inherited(ACE_Proactor::instance()), // use default proactor
   myInterface(interface_in),
   myAction(action_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StatisticHandler_Proactor_T::RPG_Net_StatisticHandler_Proactor_T"));

}

template <typename StatisticsInfoContainer_t>
RPG_Net_StatisticHandler_Proactor_T<StatisticsInfoContainer_t>::~RPG_Net_StatisticHandler_Proactor_T()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StatisticHandler_Proactor_T::~RPG_Net_StatisticHandler_Proactor_T"));

}

template <typename StatisticsInfoContainer_t>
void
RPG_Net_StatisticHandler_Proactor_T<StatisticsInfoContainer_t>::handle_time_out(const ACE_Time_Value& tv_in,
                                                                                const void* arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StatisticHandler_Proactor_T::handle_time_out"));

  ACE_UNUSED_ARG(tv_in);
  ACE_UNUSED_ARG(arg_in);

  switch (myAction)
  {
    case ACTION_COLLECT:
    {
      StatisticsInfoContainer_t result;
      ACE_OS::memset(&result, 0, sizeof(StatisticsInfoContainer_t));

      try
      {
        if (!myInterface->collect(result))
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to RPG_Common_IStatistic::collect(), continuing\n")));
      }
      catch (...)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("caught an exception in RPG_Common_IStatistic::collect(), continuing\n")));

      }

      // *TODO*: what else can we do, dump the result somehow ?
      break;
    }
    case ACTION_REPORT:
    {
      try
      {
        myInterface->report();
      }
      catch (...)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("caught an exception in RPG_Common_IStatistic::report(), continuing\n")));
      }

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("unknown/invalid action %u, continuing\n"),
                 myAction));

      break;
    }
  } // end SWITCH
}
