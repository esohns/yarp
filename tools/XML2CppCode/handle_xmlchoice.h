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

#ifndef HANDLE_XMLCHOICE_H
#define HANDLE_XMLCHOICE_H

#include <string>
#include <fstream>
#include <vector>

#include "ace/Global_Macros.h"

#include "iXML_definition_handler.h"

class Handle_XMLChoice
 : public IXML_Definition_Handler
{
  typedef IXML_Definition_Handler inherited;

 public:
  Handle_XMLChoice (std::ofstream&,     // target file stream
                    unsigned int,       // nesting level
                    const std::string&, // emit class qualifier (DLL import/export symbols) ?
                    const std::string&, // type prefix
                    const std::string&, // type postfix
                    bool,               // is vector-type ?
                    bool = true);       // emit iterator type(s), if applicable ?
  inline virtual ~Handle_XMLChoice () {}

  virtual void startElement (const std::string&); // type name
  virtual void handleData (const std::string&); // union item
  virtual void endElement ();

 private:
  ACE_UNIMPLEMENTED_FUNC (Handle_XMLChoice ());
  ACE_UNIMPLEMENTED_FUNC (Handle_XMLChoice (const Handle_XMLChoice&));
  ACE_UNIMPLEMENTED_FUNC (Handle_XMLChoice& operator= (const Handle_XMLChoice&));

  std::ofstream& myOutputFile;
  unsigned int   myNestingLevel;
  std::string    myEmitClassQualifier;
  std::string    myTypePrefix;
  std::string    myTypePostfix;

  bool           myIsVector;
  bool           myEmitIteratorTypes;
  std::string    myTypeName;
};

#endif
