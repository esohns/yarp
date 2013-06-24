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

#ifndef HANDLE_XMLSEQUENCE_H
#define HANDLE_XMLSEQUENCE_H

#include "iXML_definition_handler.h"

#include <ace/Global_Macros.h>

#include <string>
#include <fstream>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class Handle_XMLSequence
  : public IXML_Definition_Handler
{
 public:
  Handle_XMLSequence(std::ofstream&,      // target file stream
                     const std::string&,  // base type
					 const std::string&,  // type prefix
                     const std::string&,  // type postfix
					 const std::string&); // emit class qualifier (DLL symbol import/export) ?
//                      const bool& = false); // adjust for "tagged" unions
  virtual ~Handle_XMLSequence();

  virtual void startElement(const std::string&); // name of sequence
  virtual void handleData(const std::string&); // sequence value
  virtual void endElement();

 private:
  typedef IXML_Definition_Handler inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(Handle_XMLSequence());
  ACE_UNIMPLEMENTED_FUNC(Handle_XMLSequence(const Handle_XMLSequence&));
  ACE_UNIMPLEMENTED_FUNC(Handle_XMLSequence& operator=(const Handle_XMLSequence&));

  std::ofstream& myOutputFile;
  std::string    myBaseType;
  std::string    myTypePrefix;
  std::string    myTypePostfix;
  std::string    myEmitClassQualifier;
//   bool           myAdjustForTaggedUnions;
  std::string    myStructName;
};

#endif
