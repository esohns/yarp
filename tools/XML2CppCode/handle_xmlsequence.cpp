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

#include "handle_xmlsequence.h"

#include "xml2cppcode.h"
#include "xml2cppcode_common_tools.h"

#include "ace/Log_Msg.h"

#include <limits>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <locale>
#include <functional>

Handle_XMLSequence::Handle_XMLSequence(std::ofstream& targetFile_in,
                                       const unsigned int& nestingLevel_in,
                                       const std::string& typePrefix_in,
                                       const std::string& typePostfix_in,
                                       const std::string& emitClassQualifier_in,
//                                        const bool& adjustForTaggedUnions_in)
                                       const bool& isBaseClass_in)
 : myOutputFile(targetFile_in),
 //    myStructName(),
   myNestingLevel(nestingLevel_in),
   myTypePrefix(typePrefix_in),
   myTypePostfix(typePostfix_in),
   myEmitClassQualifier(emitClassQualifier_in),
//    myAdjustForTaggedUnions(adjustForTaggedUnions_in)
   myIsBaseClass(isBaseClass_in)
{
  ACE_TRACE(ACE_TEXT("Handle_XMLSequence::Handle_XMLSequence"));

}

Handle_XMLSequence::~Handle_XMLSequence()
{
  ACE_TRACE(ACE_TEXT("Handle_XMLSequence::~Handle_XMLSequence"));

}

void
Handle_XMLSequence::startElement(const std::string& struct_in)
{
  ACE_TRACE(ACE_TEXT("Handle_XMLSequence::startElement"));

  myStructName = struct_in;

	// sanity check
	if (myIsBaseClass)
		return; // done

  if ((myNestingLevel == 0) &&
      !myEmitClassQualifier.empty())
  {
    std::string exports_filename = myTypePrefix;
    exports_filename += ACE_TEXT_ALWAYS_CHAR("_");
    exports_filename += ACE_TEXT_ALWAYS_CHAR(XML2CPPCODE_DLL_EXPORT_INCLUDE_SUFFIX);
    exports_filename += ACE_TEXT_ALWAYS_CHAR(XML2CPPCODE_HEADER_EXTENSION);
    // transform to lowercase
    std::transform(exports_filename.begin(),
                   exports_filename.end(),
                   exports_filename.begin(),
                   std::bind2nd(std::ptr_fun(&std::tolower<char>),
                                std::locale("")));

    myOutputFile << ACE_TEXT_ALWAYS_CHAR("#include \"");
    myOutputFile << exports_filename;
    myOutputFile << ACE_TEXT_ALWAYS_CHAR("\"") << std::endl << std::endl;
  } // end IF

  if (myNestingLevel)
    myOutputFile << std::setw(XML2CPPCODE_INDENT * myNestingLevel)
                 << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("struct ");
  if ((myNestingLevel == 0) &&
      !myEmitClassQualifier.empty())
  {
    myOutputFile << myEmitClassQualifier;
    myOutputFile << ACE_TEXT_ALWAYS_CHAR(" ");
  } // end IF
  myOutputFile << myStructName
               << std::endl;
  if (myNestingLevel)
    myOutputFile << std::setw(XML2CPPCODE_INDENT * myNestingLevel)
                 << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("{")
               << std::endl;
}

void
Handle_XMLSequence::handleData(const std::string& structElement_in)
{
  ACE_TRACE(ACE_TEXT("Handle_XMLSequence::handleData"));

  std::string::size_type position = structElement_in.find(' ', 0);
  std::string type = structElement_in.substr(0, position);

  // strip leading namespace, if any
  std::string::size_type colon = type.find(':');
  if (colon != std::string::npos)
  {
    type = type.substr(colon + 1, std::string::npos);
  } // end IF
  if (!myTypePostfix.empty())
  {  // strip trailing "_Type", if any
    std::string::size_type type_position = type.rfind(myTypePostfix,
                                                      std::string::npos);
    if (type_position != std::string::npos)
      type.erase(type_position, std::string::npos);
  } // end IF
  XML2CppCode_Common_Tools::XMLintegratedtypeToString(type, type);

//   // adjust for "tagged" unions
//   if (myAdjustForTaggedUnions)
//   {
//     std::string::size_type union_position = type.rfind(ACE_TEXT_ALWAYS_CHAR(XML2CPPCODE_DEFAULTUNIONPOSTFIX), std::string::npos);
//     if (union_position != std::string::npos)
//     {
//       type.insert(union_position, ACE_TEXT_ALWAYS_CHAR(XML2CPPCODE_DEFAULTTAGGEDUNIONINFIX));
//     } // end IF
//   } // end IF

  // find name
  std::string::size_type next_position =
      structElement_in.find(' ', position + 1);
  std::string name = structElement_in.substr(position + 1,
                                             (next_position - (position + 1)));

  // find minOccurs
  position = structElement_in.find(' ',
                                   next_position + 1);
  std::string minOccurs =
      structElement_in.substr(next_position + 1,
                              (position - (next_position + 1)));
  // find maxOccurs
  std::string maxOccurs = structElement_in.substr(position + 1,
                                                  std::string::npos);

  // emit code
  int maxNumElements = 0;
  if (maxOccurs == ACE_TEXT_ALWAYS_CHAR("unbounded"))
    maxNumElements = std::numeric_limits<int>::max();
  else
  {
    std::istringstream converter(maxOccurs);
    converter >> maxNumElements;
  } // end ELSE
  // *IMPORTANT NOTE*: at this stage, cannot go back and predefine necessary
  // typedefs...
  if (maxNumElements > 1)
  {
    // (perhaps) append "s" to the name (it's a vector !)
    // (perhaps) append "ies" to the name with terminating "y" (it's a vector !)
    // (perhaps) append "es" to the name with terminating "s" (it's a vector !)
    if (name.rfind('y',
                   std::string::npos) == (name.size() - 1))
    {
      name.insert(name.size() - 1, ACE_TEXT_ALWAYS_CHAR("ies"));
      name.erase(--(name.end()));
    } // end IF
    else if (name.rfind('s',
                        std::string::npos) == (name.size() - 1))
    {
      // make sure the ending isn't already just that...
      if (name.rfind(ACE_TEXT_ALWAYS_CHAR("es"),
                     std::string::npos) != (name.size() - 2))
        name += ACE_TEXT_ALWAYS_CHAR("es");
    } // end IF
    else if (name.rfind('s',
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
Handle_XMLSequence::endElement()
{
  ACE_TRACE(ACE_TEXT("Handle_XMLSequence::endElement"));

  if (myNestingLevel)
    myOutputFile << std::setw(XML2CPPCODE_INDENT * myNestingLevel)
                 << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("};")
               << std::endl
               << std::endl;
}
