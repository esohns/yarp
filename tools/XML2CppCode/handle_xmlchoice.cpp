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

#include "handle_xmlchoice.h"

#include <algorithm>
#include <functional>
#include <iomanip>
#include <limits>
#include <locale>
#include <sstream>

#include "ace/Log_Msg.h"

#include "xml2cppcode.h"
#include "xml2cppcode_common_tools.h"

Handle_XMLChoice::Handle_XMLChoice (std::ofstream& targetFile_in,
                                    unsigned int nestingLevel_in,
                                    const std::string& emitClassQualifier_in,
                                    const std::string& typePrefix_in,
                                    const std::string& typePostfix_in,
                                    bool isVector_in,
                                    bool emitIteratorTypes_in)
 : myOutputFile (targetFile_in),
   myNestingLevel (nestingLevel_in),
   myEmitClassQualifier (emitClassQualifier_in),
   myTypePrefix (typePrefix_in),
   myTypePostfix (typePostfix_in),
   myIsVector (isVector_in),
   myEmitIteratorTypes (emitIteratorTypes_in)
//   myTypeName()
{
  ACE_TRACE (ACE_TEXT ("Handle_XMLChoice::Handle_XMLChoice"));

}

void
Handle_XMLChoice::startElement (const std::string& typeName_in)
{
  ACE_TRACE (ACE_TEXT ("Handle_XMLChoice::startElement"));

  myTypeName = typeName_in;

  if ((myNestingLevel == 0) &&
      !myEmitClassQualifier.empty ())
  {
    std::string exports_filename = myTypePrefix;
    exports_filename += ACE_TEXT_ALWAYS_CHAR ("_");
    exports_filename += ACE_TEXT_ALWAYS_CHAR (XML2CPPCODE_DLL_EXPORT_INCLUDE_SUFFIX);
    exports_filename += ACE_TEXT_ALWAYS_CHAR (XML2CPPCODE_HEADER_EXTENSION);
    // transform to lowercase
    std::transform (exports_filename.begin (),
                    exports_filename.end (),
                    exports_filename.begin (),
                    std::bind (std::tolower<char>,
                               std::placeholders::_1,
                               std::locale ("")));

    myOutputFile << ACE_TEXT_ALWAYS_CHAR ("#include \"");
    myOutputFile << exports_filename.c_str ();
    myOutputFile << ACE_TEXT_ALWAYS_CHAR ("\"") << std::endl << std::endl;
  } // end IF

  if (myNestingLevel)
    myOutputFile << std::setw (XML2CPPCODE_INDENT * myNestingLevel) << ACE_TEXT_ALWAYS_CHAR (" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR ("union ");
  if ((myNestingLevel == 0) &&
      !myEmitClassQualifier.empty ())
  {
    myOutputFile << myEmitClassQualifier;
    myOutputFile << ACE_TEXT_ALWAYS_CHAR(" ");
  } // end IF
  std::string type_name = myTypeName;
  if (myIsVector)
    type_name += ACE_TEXT_ALWAYS_CHAR (XML2CPPCODE_DEFAULTCHOICEPOSTFIX);
  myOutputFile << type_name << std::endl;
  if (myNestingLevel)
    myOutputFile << std::setw(XML2CPPCODE_INDENT * myNestingLevel) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("{") << std::endl;
}

void
Handle_XMLChoice::handleData(const std::string& unionElement_in)
{
  ACE_TRACE (ACE_TEXT ("Handle_XMLChoice::handleData"));

  std::string::size_type position =
      unionElement_in.find(ACE_TEXT_ALWAYS_CHAR(" "), 0);
  std::string type = unionElement_in.substr(0, position);

  // strip leading namespace, if any
  std::string::size_type colon = type.find(ACE_TEXT_ALWAYS_CHAR(":"));
  if (colon != std::string::npos)
    type = type.substr(colon + 1, std::string::npos);
  if (!myTypePostfix.empty())
  {  // strip trailing "_Type", if any
    std::string::size_type type_position = type.rfind(myTypePostfix,
                                                      std::string::npos);
    if (type_position != std::string::npos)
      type.erase(type_position, std::string::npos);
  } // end IF
  XML2CppCode_Common_Tools::XMLintegratedtypeToString(type, type);

  // find name
  std::string::size_type next_position =
      unionElement_in.find(ACE_TEXT_ALWAYS_CHAR(" "), position + 1);
  std::string name = unionElement_in.substr(position + 1,
                                            (next_position - (position + 1)));

  // find minOccurs
  position = unionElement_in.find(ACE_TEXT_ALWAYS_CHAR(" "),
                                  next_position + 1);
  std::string min_occurs =
      unionElement_in.substr(next_position + 1,
                             (position - (next_position + 1)));
  // find maxOccurs
  std::string max_occurs = unionElement_in.substr(position + 1,
                                                  std::string::npos);

  // emit code
  int max_num_elements = 0;
  if (max_occurs == ACE_TEXT_ALWAYS_CHAR("unbounded"))
    max_num_elements = std::numeric_limits<int>::max();
  else
  {
    std::istringstream converter(max_occurs);
    converter >> max_num_elements;
  } // end ELSE
  // *IMPORTANT NOTE*: at this stage, cannot go back and predefine necessary
  // typedefs...
  if (max_num_elements > 1)
  {
    // (perhaps) append "s" to the name (it's a vector !)
    // (perhaps) append "ies" to the name with terminating "y" (it's a vector !)
    // (perhaps) append "es" to the name with terminating "s" (it's a vector !)
    if (name.rfind(ACE_TEXT_ALWAYS_CHAR("y"),
                   std::string::npos) == (name.size() - 1))
    {
      name.insert(name.size() - 1, ACE_TEXT_ALWAYS_CHAR("ies"));
      name.erase(--(name.end()));
    } // end IF
    else if (name.rfind(ACE_TEXT_ALWAYS_CHAR("s"),
                        std::string::npos) == (name.size() - 1))
    {
      // make sure the ending isn't already just that...
      if (name.rfind(ACE_TEXT_ALWAYS_CHAR("es"),
                     std::string::npos) != (name.size() - 2))
        name += ACE_TEXT_ALWAYS_CHAR("es");
    } // end IF
    else if (name.rfind(ACE_TEXT_ALWAYS_CHAR("s"),
                        std::string::npos) != (name.size() - 1))
      name += ACE_TEXT_ALWAYS_CHAR("s");
    myOutputFile << std::setw(XML2CPPCODE_INDENT * (myNestingLevel ? (myNestingLevel + 1)
                                                                   : 1))
                 << ACE_TEXT_ALWAYS_CHAR(" ")
                 << ACE_TEXT_ALWAYS_CHAR("std::vector<")
                 << type
                 << ACE_TEXT_ALWAYS_CHAR("> ");
  } // end IF
  else
  {
    myOutputFile << std::setw(XML2CPPCODE_INDENT * (myNestingLevel ? (myNestingLevel + 1)
                                                                   : 1))
                 << ACE_TEXT_ALWAYS_CHAR(" ")
                 << type
                 << ACE_TEXT_ALWAYS_CHAR(" ");
  } // end ELSE

  myOutputFile << name << ACE_TEXT_ALWAYS_CHAR(";") << std::endl;
}

void
Handle_XMLChoice::endElement ()
{
  ACE_TRACE (ACE_TEXT ("Handle_XMLChoice::endElement"));

  if (myNestingLevel)
    myOutputFile << std::setw(XML2CPPCODE_INDENT * myNestingLevel) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("};") << std::endl << std::endl;

  if (myIsVector)
  {
    // emit typedef(s)
    std::string type_name = ACE_TEXT_ALWAYS_CHAR("std::vector<");
    type_name += myTypeName;
    type_name += ACE_TEXT_ALWAYS_CHAR(XML2CPPCODE_DEFAULTCHOICEPOSTFIX);
    type_name += ACE_TEXT_ALWAYS_CHAR(">");
    std::string type_definition = ACE_TEXT_ALWAYS_CHAR("typedef ");
    type_definition += type_name;
    type_definition += ACE_TEXT_ALWAYS_CHAR(" ");
    type_definition += myTypeName;
    type_definition += ACE_TEXT_ALWAYS_CHAR("_t;");
    if (myNestingLevel)
      myOutputFile << std::setw(XML2CPPCODE_INDENT * myNestingLevel)
                   << ACE_TEXT_ALWAYS_CHAR(" ");
    myOutputFile << type_definition
                 << std::endl;
    if (myEmitIteratorTypes)
    {
      type_definition = ACE_TEXT_ALWAYS_CHAR("typedef ");
      type_definition += type_name;
      type_definition += ACE_TEXT_ALWAYS_CHAR("::const_iterator ");
      type_definition += myTypeName;
      type_definition += ACE_TEXT_ALWAYS_CHAR("ConstIterator_t;");
      if (myNestingLevel)
        myOutputFile << std::setw(XML2CPPCODE_INDENT * myNestingLevel)
                     << ACE_TEXT_ALWAYS_CHAR(" ");
      myOutputFile << type_definition
                   << std::endl;
      type_definition = ACE_TEXT_ALWAYS_CHAR("typedef ");
      type_definition += type_name;
      type_definition += ACE_TEXT_ALWAYS_CHAR("::iterator ");
      type_definition += myTypeName;
      type_definition += ACE_TEXT_ALWAYS_CHAR("Iterator_t;");
      if (myNestingLevel)
        myOutputFile << std::setw(XML2CPPCODE_INDENT * myNestingLevel)
                     << ACE_TEXT_ALWAYS_CHAR(" ");
      myOutputFile << type_definition
                   << std::endl;
    } // end IF
  } // end IF
}
