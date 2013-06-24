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

#include "handle_xmlunion.h"

#include "xml2cppcode.h"
#include "xml2cppcode_common_tools.h"

#include <ace/Log_Msg.h>

#include <limits>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <locale>

Handle_XMLUnion::Handle_XMLUnion(std::ofstream& targetFile_in,
                                 const bool& emitTaggedUnion_in,
								 const std::string& emitClassQualifier_in,
                                 const std::string& typePrefix_in,
                                 const std::string& typePostfix_in)
 : myOutputFile(targetFile_in),
   myEmitTaggedUnion(emitTaggedUnion_in),
   myEmitClassQualifier(emitClassQualifier_in),
   myTypePrefix(typePrefix_in),
   myTypePostfix(typePostfix_in)
//    myUnionName(),
//    myTypeList()
{
  ACE_TRACE(ACE_TEXT("Handle_XMLUnion::Handle_XMLUnion"));

}

Handle_XMLUnion::~Handle_XMLUnion()
{
  ACE_TRACE(ACE_TEXT("Handle_XMLUnion::~Handle_XMLUnion"));

}

void
Handle_XMLUnion::startElement(const std::string& union_in)
{
  ACE_TRACE(ACE_TEXT("Handle_XMLUnion::startElement"));

  myUnionName = union_in;

  if (!myEmitClassQualifier.empty())
  {
	std::string exports_filename = myTypePrefix;
	exports_filename += ACE_TEXT_ALWAYS_CHAR("_");
	exports_filename += ACE_TEXT_ALWAYS_CHAR(XML2CPPCODE_DLL_EXPORT_INCLUDE_SUFFIX);
	exports_filename += ACE_TEXT_ALWAYS_CHAR(XML2CPPCODE_HEADER_EXTENSION);
	// transform to lowercase
	std::transform(exports_filename.begin(),
                   exports_filename.end(),
                   exports_filename.begin(),
                   std::bind2nd(std::ptr_fun(&std::tolower<char>), std::locale("")));

	myOutputFile << ACE_TEXT_ALWAYS_CHAR("#include \"");
	myOutputFile << exports_filename.c_str();
	myOutputFile << ACE_TEXT_ALWAYS_CHAR("\"") << std::endl << std::endl;
  } // end IF

  if (myEmitTaggedUnion)
  {
    std::string::size_type current_position = std::string::npos;
    std::string taggedunion_typename = myUnionName;
    current_position = taggedunion_typename.rfind(ACE_TEXT_ALWAYS_CHAR(XML2CPPCODE_DEFAULTUNIONPOSTFIX), std::string::npos);
    ACE_ASSERT(current_position != std::string::npos);
//     taggedunion_typename.insert(current_position, ACE_TEXT_ALWAYS_CHAR(XML2CPPCODE_DEFAULTTAGGEDUNIONINFIX));
    myOutputFile << ACE_TEXT_ALWAYS_CHAR("struct ");
	if (!myEmitClassQualifier.empty())
	{
	  myOutputFile << myEmitClassQualifier.c_str();
	  myOutputFile << ACE_TEXT_ALWAYS_CHAR(" ");
	} // end IF
	myOutputFile << taggedunion_typename
                 << std::endl
                 << ACE_TEXT_ALWAYS_CHAR("{")
                 << std::endl
                 << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ")
                 << ACE_TEXT_ALWAYS_CHAR("union")
                 << std::endl;
  } // end IF
  else
  {
    myOutputFile << ACE_TEXT_ALWAYS_CHAR("union ")
                 << myUnionName
                 << std::endl;
  } // end ELSE

  if (myEmitTaggedUnion)
    myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("{")
               << std::endl;
}

void
Handle_XMLUnion::handleData(const std::string& memberTypes_in)
{
  ACE_TRACE(ACE_TEXT("Handle_XMLUnion::handleData"));

  // step1: put all types into a list
  std::string current_type;
  std::string::size_type last_position = -1;
  std::string::size_type current_position = std::string::npos;
  for (current_position = memberTypes_in.find(ACE_TEXT_ALWAYS_CHAR(" "), last_position + 1);
       current_position != std::string::npos;
       current_position = memberTypes_in.find(ACE_TEXT_ALWAYS_CHAR(" "), last_position + 1))
  {
    current_type = memberTypes_in.substr(last_position + 1, current_position - (last_position + 1));

    // strip leading namespace, if any
    std::string::size_type colon = current_type.find(ACE_TEXT_ALWAYS_CHAR(":"), 0);
    if (colon != std::string::npos)
    {
      current_type = current_type.substr(colon + 1, std::string::npos);
    } // end IF

    if (!myTypePostfix.empty())
    { // strip trailing "_Type", if any
      std::string::size_type type_position = current_type.rfind(myTypePostfix, std::string::npos);
      if (type_position != std::string::npos)
      {
        current_type.erase(type_position, std::string::npos);
      } // end IF
    } // end IF

    myTypeList.push_back(current_type);

    // move forward
    last_position = current_position;
  } // end FOR
  current_type = memberTypes_in.substr(last_position + 1, current_position - (last_position + 1));

  // strip leading namespace, if any
  std::string::size_type colon = current_type.find(ACE_TEXT_ALWAYS_CHAR(":"), 0);
  if (colon != std::string::npos)
  {
    current_type = current_type.substr(colon + 1, std::string::npos);
  } // end IF

  if (!myTypePostfix.empty())
  { // strip trailing "_Type", if any
    std::string::size_type type_position = current_type.rfind(myTypePostfix, std::string::npos);
    if (type_position != std::string::npos)
    {
      current_type.erase(type_position, std::string::npos);
    } // end IF
  } // end IF

  myTypeList.push_back(current_type);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("union \"%s\" has %d items\n"),
             myUnionName.c_str(),
             myTypeList.size()));

  // step2: emit types (we need to generate individual identifiers...)
  bool isBaseType = false;
  std::string current_identifier;
  for (std::vector<std::string>::const_iterator iterator = myTypeList.begin();
       iterator != myTypeList.end();
       iterator++)
  {
    // convert basetypes XML --> C++
    isBaseType = XML2CppCode_Common_Tools::XMLintegratedtypeToString(*iterator,
                                                                     current_type);

    // create identifier
    current_identifier = current_type;

    if (isBaseType)
    {
      // step1: remove any whitespaces...
      std::string::size_type current_space = std::string::npos;
      while ((current_space = current_identifier.find(ACE_TEXT_ALWAYS_CHAR(" "), 0)) != std::string::npos)
        current_identifier.erase(current_space, 1);
    } // end IF
    else
    {
      // step1: remove any prefix...
      if (!myTypePrefix.empty())
      {
        std::string::size_type prefix = current_identifier.find(myTypePrefix, 0);
        if (prefix == 0)
        {
          current_identifier = current_identifier.substr(myTypePrefix.size(), std::string::npos);
        } // end IF
      } // end IF

      // *TODO*:
      // the above works, but for foreign types, this gets messy...
      // just remove any data BEFORE the LAST "_", assuming this will kill excess data...
      std::string::size_type last_underscore = current_identifier.find_last_of(ACE_TEXT_ALWAYS_CHAR("_"), std::string::npos);
      if (last_underscore != std::string::npos)
      {
        current_identifier = current_identifier.substr(last_underscore + 1, std::string::npos);
      } // end IF
    } // end ELSE

    // step2: transform to lowercase
	std::transform(current_identifier.begin(),
                   current_identifier.end(),
                   current_identifier.begin(),
                   std::bind2nd(std::ptr_fun(&std::tolower<char>), std::locale("")));

    // OK: emit the line of code
    myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
    if (myEmitTaggedUnion)
      myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
    myOutputFile << current_type
                 << ACE_TEXT_ALWAYS_CHAR(" ")
                 << current_identifier
                 << ACE_TEXT_ALWAYS_CHAR(";")
                 << std::endl;
  } // end FOR
}

void
Handle_XMLUnion::endElement()
{
  ACE_TRACE(ACE_TEXT("Handle_XMLUnion::endElement"));

  if (myEmitTaggedUnion)
  {
    // close union declaration
    myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ")
                 << ACE_TEXT_ALWAYS_CHAR("};") << std::endl
                 << std::endl;

    // emit discriminator enum type declaration
    std::string discriminator_typename = ACE_TEXT_ALWAYS_CHAR("Discriminator_t");
    myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ")
                 << ACE_TEXT_ALWAYS_CHAR("enum ")
                 << discriminator_typename
                 << std::endl
                 << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ")
                 << ACE_TEXT_ALWAYS_CHAR("{")
                 << std::endl;

    // emit types (we need to generate individual identifiers...)
    // *TODO*: ensure no two element types are equal...
    bool isBaseType = false;
    std::string current_identifier;
    for (std::vector<std::string>::const_iterator iterator = myTypeList.begin();
         iterator != myTypeList.end();
         iterator++)
    {
      // convert basetypes XML --> C++
      isBaseType = XML2CppCode_Common_Tools::XMLintegratedtypeToString(*iterator,
                                                                       current_identifier);
      if (isBaseType)
      {
        // step1: remove any whitespaces...
        std::string::size_type current_space = std::string::npos;
        while ((current_space = current_identifier.find(ACE_TEXT_ALWAYS_CHAR(" "), 0)) != std::string::npos)
          current_identifier.erase(current_space, 1);
      } // end IF
      else
      {
        // step1: remove any prefix...
        if (!myTypePrefix.empty())
        {
          std::string::size_type prefix = current_identifier.find(myTypePrefix, 0);
          if (prefix == 0)
          {
            current_identifier = current_identifier.substr(myTypePrefix.size(), std::string::npos);
          } // end IF
        } // end IF

        // *TODO*:
        // the above works, but for foreign types, this gets messy...
        // just remove any data BEFORE the LAST "_", assuming this will kill excess data...
        std::string::size_type last_underscore = current_identifier.find_last_of(ACE_TEXT_ALWAYS_CHAR("_"), std::string::npos);
        if (last_underscore != std::string::npos)
        {
          current_identifier = current_identifier.substr(last_underscore + 1, std::string::npos);
        } // end IF
      } // end ELSE

      // step2: transform to uppercase
	  std::transform(current_identifier.begin(),
                     current_identifier.end(),
			         current_identifier.begin(),
				     std::bind2nd(std::ptr_fun(&std::toupper<char>), std::locale("")));

      // *PORTABILITY*: "DOMAIN" seems to be a constant (see math.h)
      // --> provide a (temporary) workaround here...
#if defined __GNUC__ || defined _MSC_VER
#pragma message("applying quirk code for this compiler...")
      if (current_identifier == ACE_TEXT_ALWAYS_CHAR("DOMAIN"))
        current_identifier.insert(0, ACE_TEXT_ALWAYS_CHAR("__QUIRK__"));
#else
#pragma error("re-check code for this compiler")
#endif

      // OK: emit the line of code
      myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ")
                   << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ")
                   << current_identifier;
//       if ((iterator + 1) != myTypeList.end())
      myOutputFile << ACE_TEXT_ALWAYS_CHAR(",");
      myOutputFile << std::endl;
    } // end FOR

    myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ")
                 << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ")
                 << ACE_TEXT_ALWAYS_CHAR("INVALID") << std::endl
                 << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ")
                 << ACE_TEXT_ALWAYS_CHAR("};") << std::endl
                 << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ")
                 << discriminator_typename
                 << ACE_TEXT_ALWAYS_CHAR(" discriminator;") << std::endl;
  } // end IF

  myOutputFile << ACE_TEXT_ALWAYS_CHAR("};")
               << std::endl;
  myOutputFile << std::endl;
}
