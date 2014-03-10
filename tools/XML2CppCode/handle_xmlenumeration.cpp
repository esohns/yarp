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

#include "handle_xmlenumeration.h"

#include "xml2cppcode.h"

#include <ace/Log_Msg.h>

#include <iomanip>
#include <algorithm>
#include <locale>

#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
#include <bits/ios_base.h>
#endif

Handle_XMLEnumeration::Handle_XMLEnumeration(std::ofstream& targetFile_in,
																						 const std::string& typePrefix_in,
																						 const bool& emitStringConversionHelper_in,
																						 const std::string& emitClassQualifier_in)
 : myOutputFile(targetFile_in),
   myTypePrefix(typePrefix_in),
   myEmitStringConversionHelper(emitStringConversionHelper_in),
   myEmitClassQualifier(emitClassQualifier_in),
   myIsFirstElement(true)
//    myEnumName()
{
  ACE_TRACE(ACE_TEXT("Handle_XMLEnumeration::Handle_XMLEnumeration"));

}

Handle_XMLEnumeration::~Handle_XMLEnumeration()
{
  ACE_TRACE(ACE_TEXT("Handle_XMLEnumeration::~Handle_XMLEnumeration"));

}

void Handle_XMLEnumeration::startElement(const std::string& enumeration_in)
{
  ACE_TRACE(ACE_TEXT("Handle_XMLEnumeration::startElement"));

  myEnumName = enumeration_in;

  myOutputFile << ACE_TEXT_ALWAYS_CHAR("enum ")
               << myEnumName
               << std::endl
               << ACE_TEXT_ALWAYS_CHAR("{")
               << std::endl;
}

void Handle_XMLEnumeration::handleData(const std::string& enumValue_in)
{
  ACE_TRACE(ACE_TEXT("Handle_XMLEnumeration::handleData"));

  myOutputFile << std::setw(XML2CPPCODE_INDENT)
               << ACE_TEXT_ALWAYS_CHAR(" ")
               << enumValue_in;
  myOutputFile << (myIsFirstElement ? ACE_TEXT_ALWAYS_CHAR(" = 0,")
                                    : ACE_TEXT_ALWAYS_CHAR(","));
  myOutputFile << std::endl;

  if (myIsFirstElement)
    myIsFirstElement = false;

  myElements.push_back(enumValue_in);
}

void Handle_XMLEnumeration::endElement()
{
  ACE_TRACE(ACE_TEXT("Handle_XMLEnumeration::endElement"));

  std::string final_element = myEnumName;
  // transform to uppercase
  std::transform(final_element.begin(),
                 final_element.end(),
                 final_element.begin(),
                 std::bind2nd(std::ptr_fun(&std::toupper<char>),
                              std::locale("")));

  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("//") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << final_element << ACE_TEXT_ALWAYS_CHAR("_MAX,") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << final_element << ACE_TEXT_ALWAYS_CHAR("_INVALID") << std::endl;
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("};") << std::endl;
  myOutputFile << std::endl;

  if (myEmitStringConversionHelper)
  {
    emitStringConversionTable();
  } // end IF
}

void Handle_XMLEnumeration::emitStringConversionTable()
{
  ACE_TRACE(ACE_TEXT("Handle_XMLEnumeration::emitStringConversionTable"));

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
                   std::bind2nd(std::ptr_fun(&std::tolower<char>),
                                std::locale("")));

		myOutputFile << ACE_TEXT_ALWAYS_CHAR("#include \"");
		myOutputFile << exports_filename.c_str();
		myOutputFile << ACE_TEXT_ALWAYS_CHAR("\"") << std::endl << std::endl;
  } // end IF

  myOutputFile << ACE_TEXT_ALWAYS_CHAR("#include <ace/Global_Macros.h>") << std::endl;
  myOutputFile << std::endl;
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("#include <map>") << std::endl;
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("#include <string>") << std::endl;
  myOutputFile << std::endl;

  std::string tableTypeName = myEnumName;
  tableTypeName += ACE_TEXT_ALWAYS_CHAR("ToStringTable_t");
  std::string tableTypeIteratorName = myEnumName;
  tableTypeIteratorName += ACE_TEXT_ALWAYS_CHAR("ToStringTableIterator_t");

  myOutputFile << ACE_TEXT_ALWAYS_CHAR("typedef std::map<") << myEnumName << ACE_TEXT_ALWAYS_CHAR(", std::string> ") << tableTypeName << ACE_TEXT_ALWAYS_CHAR(";") << std::endl;
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("typedef ") << tableTypeName << ACE_TEXT_ALWAYS_CHAR("::const_iterator ") << tableTypeIteratorName << ACE_TEXT_ALWAYS_CHAR(";") << std::endl;
  myOutputFile << std::endl;

  std::string helperClass = myEnumName;
  helperClass += ACE_TEXT_ALWAYS_CHAR("Helper");
  std::string tableVariable = ACE_TEXT_ALWAYS_CHAR("my");
  tableVariable += myEnumName;
  tableVariable += ACE_TEXT_ALWAYS_CHAR("ToStringTable");
  std::string invalid_element = myEnumName;
  // transform to uppercase
  std::transform(invalid_element.begin(),
                 invalid_element.end(),
                 invalid_element.begin(),
                 std::bind2nd(std::ptr_fun(&std::toupper<char>),
                              std::locale("")));
  invalid_element += ACE_TEXT_ALWAYS_CHAR("_INVALID");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("class ");
  if (!myEmitClassQualifier.empty())
  {
    myOutputFile << myEmitClassQualifier.c_str();
    myOutputFile << ACE_TEXT_ALWAYS_CHAR(" ");
  } // end IF
  myOutputFile << helperClass << std::endl;
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("{") << std::endl;
  myOutputFile << ACE_TEXT_ALWAYS_CHAR(" public:") << std::endl;

  // emit initialization code
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("inline static void init()") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("{") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << tableVariable << ACE_TEXT_ALWAYS_CHAR(".clear();") << std::endl;
  for (ElementsIterator_t iterator = myElements.begin();
       iterator != myElements.end();
       iterator++)
  {
    myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
    myOutputFile << tableVariable << ACE_TEXT_ALWAYS_CHAR(".insert(std::make_pair(") << *iterator << ACE_TEXT_ALWAYS_CHAR(", ACE_TEXT_ALWAYS_CHAR(\"") << *iterator << ACE_TEXT_ALWAYS_CHAR("\")));") << std::endl;
  } // end FOR
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("};") << std::endl;
  myOutputFile << std::endl;

  // emit type2string functionality
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("inline static std::string ") << myEnumName << ACE_TEXT_ALWAYS_CHAR("ToString(const ") << myEnumName << ACE_TEXT_ALWAYS_CHAR("& element_in)") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("{") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("std::string result;") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << tableTypeIteratorName << ACE_TEXT_ALWAYS_CHAR(" iterator = ") << tableVariable << ACE_TEXT_ALWAYS_CHAR(".find(element_in);") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("if (iterator != ") << tableVariable << ACE_TEXT_ALWAYS_CHAR(".end())") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("result = iterator->second;") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("else") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");;
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("result = ACE_TEXT_ALWAYS_CHAR(\"") << invalid_element << ACE_TEXT_ALWAYS_CHAR("\");") << std::endl;
  myOutputFile << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("return result;") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("};") << std::endl;
  myOutputFile << std::endl;

  // emit string2type functionality
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("inline static ") << myEnumName << ACE_TEXT_ALWAYS_CHAR(" stringTo") << myEnumName << ACE_TEXT_ALWAYS_CHAR("(const std::string& string_in)") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("{") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");

  myOutputFile << tableTypeIteratorName << ACE_TEXT_ALWAYS_CHAR(" iterator = ") << tableVariable << ACE_TEXT_ALWAYS_CHAR(".begin();") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("do") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("{") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("if (iterator->second == string_in)") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("return iterator->first;") << std::endl;
  myOutputFile << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("iterator++;") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("} while (iterator != ") << tableVariable << ACE_TEXT_ALWAYS_CHAR(".end());") << std::endl;
  myOutputFile << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ") << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("return ") << invalid_element << ACE_TEXT_ALWAYS_CHAR(";") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("};") << std::endl;
  myOutputFile << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("static ") << tableTypeName << ACE_TEXT_ALWAYS_CHAR(" ") << tableVariable << ACE_TEXT_ALWAYS_CHAR(";") << std::endl;
  myOutputFile << std::endl;

  myOutputFile << ACE_TEXT_ALWAYS_CHAR(" private:") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("ACE_UNIMPLEMENTED_FUNC(") << helperClass << ACE_TEXT_ALWAYS_CHAR("());") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("ACE_UNIMPLEMENTED_FUNC(") << helperClass << ACE_TEXT_ALWAYS_CHAR("(const ") << helperClass << ACE_TEXT_ALWAYS_CHAR("&));") << std::endl;
  myOutputFile << std::setw(XML2CPPCODE_INDENT) << ACE_TEXT_ALWAYS_CHAR(" ");
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("ACE_UNIMPLEMENTED_FUNC(") << helperClass << ACE_TEXT_ALWAYS_CHAR("& operator=(const ") << helperClass << ACE_TEXT_ALWAYS_CHAR("&));") << std::endl;
  myOutputFile << ACE_TEXT_ALWAYS_CHAR("};") << std::endl;
  myOutputFile << std::endl;

//   // declare static table
//   myOutputFile << tableTypeName << ACE_TEXT_ALWAYS_CHAR(" ") << helperClass << ACE_TEXT_ALWAYS_CHAR("::") << tableVariable << ACE_TEXT_ALWAYS_CHAR(";") << std::endl;
//   myOutputFile << std::endl;
}
