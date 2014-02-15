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

#ifndef RPG_COMMON_SIGNALHANDLER_H
#define RPG_COMMON_SIGNALHANDLER_H

#include "rpg_common_exports.h"

#include <ace/Global_Macros.h>
#include <ace/Asynch_IO.h>
#include <ace/Event_Handler.h>

// forward declaration(s)
class RPG_Common_ISignal;

class RPG_Common_Export RPG_Common_SignalHandler
 : public ACE_Handler,
   public ACE_Event_Handler
{
 public:
  virtual ~RPG_Common_SignalHandler();

  // *NOTE*: proactor code: invoke handle_exception
  virtual void handle_time_out(const ACE_Time_Value&, // target time
                               const void* = NULL);   // act
  // *NOTE*: save state and notify the proactor/reactor
  virtual int handle_signal(int,                 // signal
                            siginfo_t* = NULL,   // not needed on UNIX
                            ucontext_t* = NULL); // not used

 protected:
	RPG_Common_SignalHandler(RPG_Common_ISignal*, // event handler handle
                           const bool& = true); // use reactor ?

 private:
  typedef ACE_Handler inherited;
  typedef ACE_Event_Handler inherited2;

  ACE_UNIMPLEMENTED_FUNC(RPG_Common_SignalHandler());
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_SignalHandler(const RPG_Common_SignalHandler&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_SignalHandler& operator=(const RPG_Common_SignalHandler&));

  // *NOTE*: implement specific behaviour
  virtual int handle_exception(ACE_HANDLE = ACE_INVALID_HANDLE); // handle

  RPG_Common_ISignal* myHandler;
  bool                myUseReactor;
  int                 mySignal;
  siginfo_t           mySigInfo;
  ucontext_t          myUContext;
};

#endif
