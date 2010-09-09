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
#ifndef IRC_CLIENT_GUI_CONNECTION_HANDLER_H
#define IRC_CLIENT_GUI_CONNECTION_HANDLER_H

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class IRC_Client_GUI_Connection_Handler
{
 public:
//   IRC_Client_GUI_Connection_Handler();
  virtual~IRC_Client_GUI_Connection_Handler();

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(IRC_Client_GUI_Connection_Handler());
  ACE_UNIMPLEMENTED_FUNC(IRC_Client_GUI_Connection_Handler(const IRC_Client_GUI_Connection_Handler&));
  ACE_UNIMPLEMENTED_FUNC(IRC_Client_GUI_Connection_Handler& operator=(const IRC_Client_GUI_Connection_Handler&));
};

#endif
