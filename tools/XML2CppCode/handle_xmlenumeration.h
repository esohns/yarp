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

#ifndef HANDLE_XMLENUMERATION_H
#define HANDLE_XMLENUMERATION_H

#include "iXML_definition_handler.h"

#include <ace/Global_Macros.h>

#include <string>
#include <vector>
#include <fstream>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class Handle_XMLEnumeration
  : public IXML_Definition_Handler
{
 public:
  Handle_XMLEnumeration(std::ofstream&,		 // existing file stream --> append
	                    const std::string&,  // type prefix
                        const bool&,		 // emit string conversion helper ?
						const std::string&); // emit class qualifier (DLL import/export symbols) ?
  virtual ~Handle_XMLEnumeration();

  virtual void startElement(const std::string&); // name of enumeration
  virtual void handleData(const std::string&);   // enumeration value
  virtual void endElement();

 private:
  typedef IXML_Definition_Handler inherited;

  typedef std::vector<std::string> Elements_t;
  typedef Elements_t::const_iterator ElementsIterator_t;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(Handle_XMLEnumeration());
  ACE_UNIMPLEMENTED_FUNC(Handle_XMLEnumeration(const Handle_XMLEnumeration&));
  ACE_UNIMPLEMENTED_FUNC(Handle_XMLEnumeration& operator=(const Handle_XMLEnumeration&));

  // helper methods
  void emitStringConversionTable();

  std::ofstream& myOutputFile;
  std::string    myTypePrefix;
  bool           myEmitStringConversionHelper;
  std::string    myEmitClassQualifier;
  bool           myIsFirstElement;
  std::string    myEnumName;
  Elements_t     myElements;
};

#endif
