/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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

#ifndef RPG_CLIENT_LOGGER_H
#define RPG_CLIENT_LOGGER_H

#include "ace/Log_Msg_Backend.h"
#include "ace/Log_Record.h"

#include "rpg_client_common.h"
#include "rpg_client_exports.h"

// forward declaration(s)
class ACE_Recursive_Thread_Mutex;

/**
  @author Erik Sohns <erik.sohns@web.de>
 */
class RPG_Client_Export RPG_Client_Logger
 : public ACE_Log_Msg_Backend
{
 public:
  RPG_Client_Logger (RPG_Client_MessageStack_t*, // message stack
                     ACE_Thread_Mutex*);         // synch lock
  virtual ~RPG_Client_Logger ();

  // implement ACE_Log_Msg_Backend interface
  virtual int open (const ACE_TCHAR*); // logger key
  virtual int reset (void);
  virtual int close (void);
  virtual ssize_t log (ACE_Log_Record&); // record

 private:
  typedef ACE_Log_Msg_Backend inherited;

  ACE_UNIMPLEMENTED_FUNC (RPG_Client_Logger ());
  ACE_UNIMPLEMENTED_FUNC (RPG_Client_Logger (const RPG_Client_Logger&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Client_Logger& operator=(const RPG_Client_Logger&));

  RPG_Client_MessageStack_t* myMessageStack;
  ACE_Thread_Mutex*          myLock;
};

#endif
