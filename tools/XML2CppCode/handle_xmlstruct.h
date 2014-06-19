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

#ifndef HANDLE_XMLSTRUCT_H
#define HANDLE_XMLSTRUCT_H

#include "handle_xmlsequence.h"

#include "ace/Global_Macros.h"

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class Handle_XMLStruct
 : public Handle_XMLSequence
{
 public:
  Handle_XMLStruct(std::ofstream&,      // target file stream
                   const std::string&,  // type prefix
                   const std::string&,  // type postfix
                   const std::string&); // emit class qualifier (DLL symbol import/export) ?
  virtual ~Handle_XMLStruct();

  virtual void startElement(const std::string&); // name of struct
  virtual void handleData(const std::string&); // base class (if any)
  virtual void endElement();

 private:
  typedef Handle_XMLSequence inherited;

  ACE_UNIMPLEMENTED_FUNC(Handle_XMLStruct());
  ACE_UNIMPLEMENTED_FUNC(Handle_XMLStruct(const Handle_XMLStruct&));
  ACE_UNIMPLEMENTED_FUNC(Handle_XMLStruct& operator=(const Handle_XMLStruct&));

  std::string myBaseType;
};

#endif
