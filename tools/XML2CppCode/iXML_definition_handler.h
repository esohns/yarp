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
#ifndef IXML_DEFINITION_HANDLER_H
#define IXML_DEFINITION_HANDLER_H

#include <ace/Global_Macros.h>

#include <string>

class IXML_Definition_Handler
{
 public:
  // make sure base class dtors are called !
  inline virtual ~IXML_Definition_Handler() {};

  virtual void startElement(const std::string&) = 0; // name
  virtual void handleData(const std::string&) = 0; // value
  virtual void endElement() = 0;
};

#endif
