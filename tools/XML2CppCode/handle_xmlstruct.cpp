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
#include "stdafx.h"

#include "handle_xmlstruct.h"

#include <limits>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <locale>
#include <functional>

#include "ace/Log_Msg.h"

#include "xml2cppcode.h"
#include "xml2cppcode_common_tools.h"

Handle_XMLStruct::Handle_XMLStruct(std::ofstream& targetFile_in,
                                   const std::string& typePrefix_in,
                                   const std::string& typePostfix_in,
                                   const std::string& emitClassQualifier_in)
 : inherited (targetFile_in,
             0,
             typePrefix_in,
             typePostfix_in,
             emitClassQualifier_in)//,
   //myBaseType()
{
  ACE_TRACE(ACE_TEXT("Handle_XMLStruct::Handle_XMLStruct"));

}

void
Handle_XMLStruct::startElement (const std::string& struct_in)
{
  ACE_TRACE (ACE_TEXT ("Handle_XMLStruct::startElement"));

  inherited::myStructName = struct_in;

  if (!inherited::myEmitClassQualifier.empty ())
  {
    std::string exports_filename = myTypePrefix;
    exports_filename += ACE_TEXT_ALWAYS_CHAR ("_");
    exports_filename += ACE_TEXT_ALWAYS_CHAR (XML2CPPCODE_DLL_EXPORT_INCLUDE_SUFFIX);
    exports_filename += ACE_TEXT_ALWAYS_CHAR (XML2CPPCODE_HEADER_EXTENSION);
    // transform to lowercase
    std::transform (exports_filename.begin (),
                    exports_filename.end (),
                    exports_filename.begin (),
                    std::bind2nd (std::ptr_fun (&std::tolower<char>),
                                  std::locale ("")));

    myOutputFile << ACE_TEXT_ALWAYS_CHAR("#include \"");
    myOutputFile << exports_filename;
    myOutputFile << ACE_TEXT_ALWAYS_CHAR("\"") << std::endl << std::endl;
  } // end IF

  myOutputFile << ACE_TEXT_ALWAYS_CHAR("struct ");
  if (!inherited::myEmitClassQualifier.empty())
  {
    myOutputFile << inherited::myEmitClassQualifier;
    myOutputFile << ACE_TEXT_ALWAYS_CHAR(" ");
  } // end IF
  myOutputFile << inherited::myStructName << std::endl;
}

void
Handle_XMLStruct::handleData (const std::string& element_in)
{
  ACE_TRACE (ACE_TEXT ("Handle_XMLStruct::handleData"));

  // sanity check
  ACE_ASSERT (!element_in.empty() && element_in.compare (ACE_TEXT_ALWAYS_CHAR(" ")));

  // step0: strip any leading / trailing whitespace
  std::string element = XML2CppCode_Common_Tools::strip (element_in);

  // step1: determine if it's a base class
  bool is_base_class = false;
  std::stringstream converter;
  unsigned int cardinality = 0;
  std::string::size_type position_1, position_2;
  position_2 = element.rfind (' ', std::string::npos);
  if (position_2 == std::string::npos)
    is_base_class = true;
  else
  {
    converter << element.substr (position_2 + 1, std::string::npos);
    converter >> cardinality;
    if (converter.fail ())
      is_base_class = true;
    else
    {
      position_1 = element.rfind (' ', position_2 - 1);
      if (position_1 == std::string::npos)
        is_base_class = true;
      else
      {
        //converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter.clear ();
        converter << element.substr (position_1 + 1, element.size () - position_2 - 1);
        converter >> cardinality;
        if (converter.fail())
          is_base_class = true;
      } // end ELSE
    } // end ELSE
  } // end ELSE

  if (is_base_class)
    myOutputFile << ACE_TEXT_ALWAYS_CHAR(" : public ")
                 << element_in
                 << std::endl
                 << ACE_TEXT_ALWAYS_CHAR("{")
                 << std::endl;
  else
    inherited::handleData (element_in);
}

void
Handle_XMLStruct::endElement ()
{
  ACE_TRACE (ACE_TEXT ("Handle_XMLStruct::endElement"));

  myOutputFile << ACE_TEXT_ALWAYS_CHAR("};") << std::endl << std::endl;
}
