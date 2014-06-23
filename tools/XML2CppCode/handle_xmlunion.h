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

#ifndef HANDLE_XMLUNION_H
#define HANDLE_XMLUNION_H

#include "iXML_definition_handler.h"

#include <ace/Global_Macros.h>

#include <string>
#include <fstream>
#include <vector>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class Handle_XMLUnion
 : public IXML_Definition_Handler
{
 public:
  Handle_XMLUnion(std::ofstream&,      // target file stream
                  const bool&,         // emit "tagged" union
                  const std::string&,  // emit class qualifier (DLL import/export symbols) ?
                  const std::string&,  // type prefix
                  const std::string&); // type postfix
  virtual ~Handle_XMLUnion();

  virtual void startElement(const std::string&); // name of union
  virtual void handleData(const std::string&); // memberTypes value
  virtual void endElement();

 private:
  typedef IXML_Definition_Handler inherited;

  ACE_UNIMPLEMENTED_FUNC(Handle_XMLUnion());
  ACE_UNIMPLEMENTED_FUNC(Handle_XMLUnion(const Handle_XMLUnion&));
  ACE_UNIMPLEMENTED_FUNC(Handle_XMLUnion& operator=(const Handle_XMLUnion&));

  std::ofstream&           myOutputFile;
  bool                     myEmitTaggedUnion;
  std::string              myEmitClassQualifier;
  std::string              myTypePrefix;
  std::string              myTypePostfix;

  std::string              myUnionName;
  std::vector<std::string> myTypeList;
};

#endif
