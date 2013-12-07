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

#ifndef RPG_Net_Server_SocketHandler_H
#define RPG_Net_Server_SocketHandler_H

#include "rpg_net_server_exports.h"

#include "rpg_net_stream_socket_base.h"

#include <ace/Global_Macros.h>
#include <ace/Asynch_IO.h>

class RPG_Net_Server_Export RPG_Net_Server_SocketHandler
 : public ACE_Service_Handler
{
 public:
  RPG_Net_Server_SocketHandler();
  virtual ~RPG_Net_Server_SocketHandler();

  virtual void open(ACE_HANDLE,          // socket identifier
                    ACE_Message_Block&); // initial data (if any)

 protected:
  virtual void handle_read_stream(const ACE_Asynch_Read_Stream::Result&); // result
  virtual void handle_write_stream(const ACE_Asynch_Write_Stream::Result&); // result

 private:
  typedef ACE_Service_Handler inherited;

  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Server_SocketHandler(const RPG_Net_Server_SocketHandler&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Server_SocketHandler& operator=(const RPG_Net_Server_SocketHandler&));
  void initiate_read_stream(void);

  ACE_Asynch_Read_Stream  myInputStream;
  ACE_Asynch_Write_Stream myOutputStream;
  ACE_HANDLE              myHandle;
};

#endif
