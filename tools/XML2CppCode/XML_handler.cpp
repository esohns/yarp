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

// *NOTE*: workaround quirky MSVC...
#define NOMINMAX

#include "XML_handler.h"

#include "xml2cppcode.h"
#include "handle_xmlenumeration.h"
#include "handle_xmlsequence.h"
#include "handle_xmlunion.h"

#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax2/Attributes.hpp>

#include <ace/ACE.h>
#include <ace/Log_Msg.h>

#include <algorithm>
#include <sstream>
#include <locale>

// *NOTE*: need this to import correct PACKAGE_STRING !
#ifdef HAVE_CONFIG_H
#include "XML2CppCode-config.h"
#endif

XML_Handler::XML_Handler(const std::string& emitClassQualifiers_in,
						 const bool& emitStringConversionUtilities_in,
                         const bool& emitTaggedUnions_in,
                         const std::string& schemaFilename_in,
                         const bool& generateIncludeHeader_in,
                         const std::string& targetDirectory_in,
                         const std::string& preamble_in,
                         const bool& filePerDefinition_in,
                         const std::string& typePrefix_in,
                         const std::string& typePostfix_in)
 : inherited(),
   myCurrentDefinitionHandler(NULL),
//    myIncludeHeaderFile(),
//    myCurrentOutputFile(),
//    myCurrentExtension(),
//    myCurrentElementName(),
   myIsFirstRelevantElement(true),
   myTargetDirectory(targetDirectory_in),
   myFilePerDefinition(filePerDefinition_in),
   mySchemaFilename(schemaFilename_in),
   myPreamble(preamble_in),
   myEmitClassQualifiers(emitClassQualifiers_in),
   myEmitStringConversionUtilities(emitStringConversionUtilities_in),
   myEmitTaggedUnions(emitTaggedUnions_in),
   myGenerateIncludeHeader(generateIncludeHeader_in),
   myTypePrefix(typePrefix_in),
   myTypePostfix(typePostfix_in),
   myLocator(NULL),
   myHeadersUseStdVector(false)
//    myHeaders()
//    myIgnoreCharacters(true)
{
  ACE_TRACE(ACE_TEXT("XML_Handler::XML_Handler"));

}

XML_Handler::~XML_Handler()
{
  ACE_TRACE(ACE_TEXT("XML_Handler::~XML_Handler"));

}

void XML_Handler::endDocument()
{
  ACE_TRACE(ACE_TEXT("XML_Handler::endDocument"));

  // if all definitions went into one file, we need to:
  // 1. write the postscript (if any)
  // 2. close it here
  if (!myFilePerDefinition)
  {
    insertPostscript(myCurrentOutputFile);

    if (myCurrentOutputFile.is_open())
    {
      myCurrentOutputFile.close();

      // debug info
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("closed file...\n")));
    } // end IF
  } // end IF

  if (myGenerateIncludeHeader)
  {
    // write all our headers
    insertIncludeHeaders(myHeaders,
                         myHeadersUseStdVector,
                         myIncludeHeaderFile);
    myHeaders.clear();

    insertPostscript(myIncludeHeaderFile);

    if (myIncludeHeaderFile.is_open())
    {
      myIncludeHeaderFile.close();

      // debug info
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("closed header file...\n")));
    } // end IF

    myHeadersUseStdVector = false;
  } // end IF
}

void XML_Handler::endElement(const XMLCh* const uri_in,
                             const XMLCh* const localname_in,
                             const XMLCh* const qname_in)
{
  ACE_TRACE(ACE_TEXT("XML_Handler::endElement"));

  // debug info
//   char* uri = XMLString::transcode(uri_in);
  char* localname = XMLString::transcode(localname_in);
//   char* qname = XMLString::transcode(qname_in);
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("endElement: \"%s\", \"%s\", \"%s\"\n"),
//              uri,
//              localname,
//              qname));
//   XMLString::release(&uri);
  std::string element = localname;
  XMLString::release(&localname);
//   XMLString::release(&qname);

  XMLElementType type = stringToXMLElementType(element);
  switch (type)
  {
    case XML_SCHEMA:
    case XML_ANNOTATION:
    case XML_DOCUMENTATION:
    case XML_RESTRICTION:
    case XML_ENUMERATION:
    case XML_SEQUENCE:
    case XML_ATTRIBUTE:
    case XML_ELEMENT:
    case XML_INCLUDE:
    case XML_EXTENSION:
    {
      myCurrentExtension.resize(0);
    }
    case XML_COMPLEXCONTENT:
    case XML_UNION:
    {
//       myIgnoreCharacters = true;

      break;
    }
    case XML_SIMPLETYPE:
    case XML_COMPLEXTYPE:
    {
      // notify & delete our handler (if any)
      if (myCurrentDefinitionHandler)
      {
        myCurrentDefinitionHandler->endElement();

        delete myCurrentDefinitionHandler;
        myCurrentDefinitionHandler = NULL;
      } // end IF

      // next relevant element will be the first...
      myIsFirstRelevantElement = true;

      // reset element name
      myCurrentElementName.clear();

      if (myFilePerDefinition && myCurrentOutputFile.is_open())
      {
        insertPostscript(myCurrentOutputFile);

        myCurrentOutputFile.close();

//     // debug info
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("closed file: \"%s\"...\n"),
//                myFilename.c_str()));
      } // end IF

      break;
    }
    default:
    {
      ACE_DEBUG((LM_WARNING,
                 ACE_TEXT("unknown/invalid XML element: %d, continuing\n"),
                 type));

      break;
    }
  } // end SWITCH
}

void XML_Handler::setDocumentLocator(const Locator* const locator_in)
{
  ACE_TRACE(ACE_TEXT("XML_Handler::setDocumentLocator"));

  myLocator = locator_in;
}

void XML_Handler::startDocument()
{
  ACE_TRACE(ACE_TEXT("XML_Handler::startDocument"));

  // if all definitions will go into one file, we need to generate its name(s) here
  // replace file extension (.xsd --> .h)
  std::string extension = ACE_TEXT_ALWAYS_CHAR(XML2CPPCODE_HEADER_EXTENSION);
  std::string targetFilenameStub = mySchemaFilename;
  std::string::size_type position = std::string::npos;
  position = targetFilenameStub.rfind(ACE_TEXT_ALWAYS_CHAR("."), std::string::npos);
  if (position != std::string::npos)
  {
    targetFilenameStub.erase(position, std::string::npos);
  } // end IF
  std::string FQFilenameStub = myTargetDirectory;
  FQFilenameStub += ACE_DIRECTORY_SEPARATOR_STR_A;

  if (!myFilePerDefinition)
  {
    std::string targetFilename = targetFilenameStub;
    targetFilename += extension;

    // create/open target file
    std::string FQFilename = FQFilenameStub;
    FQFilename += targetFilename;
    myCurrentOutputFile.open(FQFilename.c_str(), (std::ios_base::out | std::ios_base::trunc));
    if (myCurrentOutputFile.fail() || !myCurrentOutputFile.is_open())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to create/open file: \"%s\", aborting\n"),
                 FQFilename.c_str()));

      XMLCh* message = XMLString::transcode(ACE_TEXT("failed to create/open file, aborting\n"));
      ACE_ASSERT(message);
      SAXParseException exception(message, *myLocator);
      fatalError(exception);

      // clean up
      XMLString::release(&message);
    } // end IF

    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("created file: \"%s\"...\n"),
               targetFilename.c_str()));

    // write preamble/include protection
    insertPreamble(myCurrentOutputFile);
    insertMultipleIncludeProtection(true,
                                    targetFilename,
                                    myCurrentOutputFile);
  } // end IF

  if (myGenerateIncludeHeader)
  {
    std::string targetFilename = targetFilenameStub;
    targetFilename += ACE_TEXT_ALWAYS_CHAR(XML2CPPCODE_HEADER_APPENDIX);
    targetFilename += extension;

    // create/open target file
    std::string FQFilename = FQFilenameStub;
    FQFilename += targetFilename;
    myIncludeHeaderFile.open(FQFilename.c_str(), (std::ios_base::out | std::ios_base::trunc));
    if (myIncludeHeaderFile.fail() || !myIncludeHeaderFile.is_open())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to create/open file: \"%s\", aborting\n"),
                 FQFilename.c_str()));

      XMLCh* message = XMLString::transcode(ACE_TEXT("failed to create/open file, aborting\n"));
      ACE_ASSERT(message);
      SAXParseException exception(message, *myLocator);
      fatalError(exception);

      // clean up
      XMLString::release(&message);
    } // end IF

    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("created file: \"%s\"...\n"),
               targetFilename.c_str()));

    // write preamble/include protection
    insertPreamble(myIncludeHeaderFile);
    insertMultipleIncludeProtection(true,
                                    targetFilename,
                                    myIncludeHeaderFile);
  } // end IF
}

void XML_Handler::startElement(const XMLCh* const uri_in,
                               const XMLCh* const localname_in,
                               const XMLCh* const qname_in,
                               const Attributes& attributes_in)
{
  ACE_TRACE(ACE_TEXT("XML_Handler::startElement"));

//   char* uri = XMLString::transcode(uri_in);
  char* localname = XMLString::transcode(localname_in);
//   char* qname = XMLString::transcode(qname_in);
  std::string element = localname;
  XMLString::release(&localname);

  XMLElementType typeOfElement = stringToXMLElementType(element);
  switch (typeOfElement)
  {
    case XML_SCHEMA:
    case XML_ANNOTATION:
    case XML_DOCUMENTATION:
    case XML_RESTRICTION:
    case XML_SEQUENCE:
    case XML_INCLUDE:
    case XML_COMPLEXCONTENT:
    {
//       myIgnoreCharacters = true;

      break;
    }
    case XML_EXTENSION:
    {
      XMLCh* base_string = XMLString::transcode(ACE_TEXT_ALWAYS_CHAR("base"));
      char* base = XMLString::transcode(attributes_in.getValue(base_string));
      XMLString::release(&base_string);
      if (base)
      {
        myCurrentExtension = base;
        XMLString::release(&base);
      } // end IF

      // strip trailing type postfix, if any
      if (!myTypePostfix.empty())
      {
        std::string::size_type position = myCurrentExtension.rfind(myTypePostfix, std::string::npos);
        if (position != std::string::npos)
        {
          myCurrentExtension.erase(position, std::string::npos);
        } // end IF
      } // end IF

      break;
    }
    // *WARNING*: falls through to the next case !
    case XML_COMPLEXTYPE:
    {
      // MAY be interesting, remember element name unless the attribute "parse=false" is set
//       for (int i = 0; i < attributes_in.getLength(); i++)
//       {
//         const XMLCh* Qname = attributes_in.getQName(i);
//         const XMLCh* URI = attributes_in.getURI(i);
//         const XMLCh* local = attributes_in.getLocalName(i);
//         const XMLCh* type = attributes_in.getType(i);
//         const XMLCh* value = attributes_in.getValue(i);
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("attribute %d: qname: %s, uri: %s, local: %s, type: %s, value: %s\n"),
//                    i,
//                    XMLString::transcode(Qname),
//                    XMLString::transcode(URI),
//                    XMLString::transcode(local),
//                    XMLString::transcode(type),
//                    XMLString::transcode(value)));
//       }

      XMLCh* id_string = XMLString::transcode(ACE_TEXT_ALWAYS_CHAR("id"));
      char* id = XMLString::transcode(attributes_in.getValue(id_string));
      XMLString::release(&id_string);
      if (id)
      {
        std::string id_temp = id;
        XMLString::release(&id);
        if (id_temp.find(ACE_TEXT_ALWAYS_CHAR("skip"), 0) == 0)
        {
          // we'll skip this one...
          XMLCh* name_string = XMLString::transcode(ACE_TEXT_ALWAYS_CHAR("name"));
          char* name = XMLString::transcode(attributes_in.getValue(name_string));
          XMLString::release(&name_string);

          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("skipping over complexType: \"%s\"...\n"),
                     name));

          XMLString::release(&name);

          break;
        } // end IF
      } // end IF
    }
    case XML_SIMPLETYPE:
    {
      // may be interesting, remember element name
      XMLCh* name_string = XMLString::transcode(ACE_TEXT_ALWAYS_CHAR("name"));
      char* name = XMLString::transcode(attributes_in.getValue(name_string));
      XMLString::release(&name_string);
      myCurrentElementName = name;
      XMLString::release(&name);

      // strip trailing type postfix, if any
      if (!myTypePostfix.empty())
      {
        std::string::size_type position = myCurrentElementName.rfind(myTypePostfix, std::string::npos);
        if (position != std::string::npos)
        {
          myCurrentElementName.erase(position, std::string::npos);
        } // end IF
      } // end IF

      if (myFilePerDefinition)
      {
        // create target filename
        std::string FQfilename = myTargetDirectory;
        FQfilename += ACE_DIRECTORY_SEPARATOR_STR_A;
        std::string extension = ACE_TEXT_ALWAYS_CHAR(XML2CPPCODE_HEADER_EXTENSION);
        std::string targetFilename = myCurrentElementName;
        targetFilename += extension;
        FQfilename += targetFilename;
        // transform to lowercase
		std::transform(FQfilename.begin(),
					   FQfilename.end(),
					   FQfilename.begin(),
					   std::bind2nd(std::ptr_fun(&std::tolower<char>), std::locale("")));

        // try to open file
        try
        {
          myCurrentOutputFile.open(FQfilename.c_str(),
                                   (std::ios_base::out | std::ios_base::trunc));
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in std::ofstream::open, returning\n")));

          return;
        }
        if (myCurrentOutputFile.is_open())
        {
//       // debug info
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("created/opened file: \"%s\"...\n"),
//                  myFilename.c_str()));

          insertPreamble(myCurrentOutputFile);
          std::string filename = ACE::basename(FQfilename.c_str());
          insertMultipleIncludeProtection(true,
                                          filename,
                                          myCurrentOutputFile);
        } // end IF
        else
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to open file: \"%s\"...\n"),
                     FQfilename.c_str()));
        } // end ELSE

        if (myGenerateIncludeHeader)
        {
          std::string filename = ACE::basename(FQfilename.c_str());
          myHeaders.push_back(filename);
        } // end IF
      } // end IF

      break;
    }
    case XML_ENUMERATION:
    {
      if (myIsFirstRelevantElement)
      {
        try
        {
          myCurrentDefinitionHandler = new Handle_XMLEnumeration(myCurrentOutputFile,
			                                                     myTypePrefix,
                                                                 myEmitStringConversionUtilities,
																 myEmitClassQualifiers);
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in ::new, aborting\n")));

          OutOfMemoryException exception;
          throw exception;
        }
        if (!myCurrentDefinitionHandler)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to allocate memory, aborting\n")));

          OutOfMemoryException exception;
          throw exception;
        } // end IF

        // init handler
        myCurrentDefinitionHandler->startElement(myCurrentElementName);

        // more interesting elements will follow...
        myIsFirstRelevantElement = false;
      } // end IF

      // extract value
      XMLCh* value_string = XMLString::transcode(ACE_TEXT_ALWAYS_CHAR("value"));
      char* value = XMLString::transcode(attributes_in.getValue(value_string));
      XMLString::release(&value_string);
      myCurrentDefinitionHandler->handleData(std::string(value));
      XMLString::release(&value);

      break;
    }
    case XML_ATTRIBUTE:
    case XML_ELEMENT:
    {
      // check if this is an instance or part of a definition
      if (myCurrentElementName.empty())
      {
        // it's an instance --> nothing to do
        break;
      } // end IF

      if (myIsFirstRelevantElement)
      {
        try
        {
          myCurrentDefinitionHandler = new Handle_XMLSequence(myCurrentOutputFile,
                                                              myCurrentExtension,
															  myTypePrefix,
                                                              myTypePostfix,
															  myEmitClassQualifiers);
//                                                               myEmitTaggedUnions);
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in ::new, aborting\n")));

          OutOfMemoryException exception;
          throw exception;
        }
        if (!myCurrentDefinitionHandler)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to allocate memory, aborting\n")));

          OutOfMemoryException exception;
          throw exception;
        } // end IF

        // init handler
        myCurrentDefinitionHandler->startElement(myCurrentElementName);

        // more interesting elements will follow...
        myIsFirstRelevantElement = false;
      } // end IF

      // extract name/type
      std::string definition;
      XMLCh* name_string = XMLString::transcode(ACE_TEXT_ALWAYS_CHAR("name"));
      char* name = XMLString::transcode(attributes_in.getValue(name_string));
      XMLString::release(&name_string);
      XMLCh* type_string = XMLString::transcode(ACE_TEXT_ALWAYS_CHAR("type"));
      char* type = XMLString::transcode(attributes_in.getValue(type_string));
      XMLString::release(&type_string);
      definition = type;
      XMLString::release(&type);
      definition += ACE_TEXT_ALWAYS_CHAR(" ");
      definition += name;
      XMLString::release(&name);

      if (typeOfElement == XML_ELEMENT)
      {
        // definition for (sequence) elements will be "type name minOccurs maxOccurs"
        XMLCh* minOccurs_string = XMLString::transcode(ACE_TEXT_ALWAYS_CHAR("minOccurs"));
        char* minOccurs = XMLString::transcode(attributes_in.getValue(minOccurs_string));
        XMLString::release(&minOccurs_string);
        XMLCh* maxOccurs_string = XMLString::transcode(ACE_TEXT_ALWAYS_CHAR("maxOccurs"));
        char* maxOccurs = XMLString::transcode(attributes_in.getValue(maxOccurs_string));
        XMLString::release(&maxOccurs_string);
        definition += ACE_TEXT_ALWAYS_CHAR(" ");
        definition += (minOccurs ? minOccurs : ACE_TEXT_ALWAYS_CHAR(XML2CPPCODE_DEFAULTMINOCCURS));
        XMLString::release(&minOccurs);
        definition += ACE_TEXT_ALWAYS_CHAR(" ");
        definition += (maxOccurs ? maxOccurs : ACE_TEXT_ALWAYS_CHAR(XML2CPPCODE_DEFAULTMAXOCCURS));
        std::string string_maxoccurs = maxOccurs;
        XMLString::release(&maxOccurs);

        // check for multiplicity --> include <vector>
        if (myGenerateIncludeHeader)
        {
          int maxNumElements = 0;
          if (string_maxoccurs == ACE_TEXT_ALWAYS_CHAR("unbounded"))
          {
            maxNumElements = std::numeric_limits<int>::max();
          } // end IF
          else
          {
            std::istringstream converter(string_maxoccurs);
            converter >> maxNumElements;
          } // end ELSE
          if (maxNumElements > 1)
            myHeadersUseStdVector = true;
        } // end IF
      } // end IF
      else
      {
        // definition for attributes will be "type name 0/1 1"
        XMLCh* use_string = XMLString::transcode(ACE_TEXT_ALWAYS_CHAR("use"));
        char* use = XMLString::transcode(attributes_in.getValue(use_string));
        XMLString::release(&use_string);
        std::string useAttribute = use;
        XMLString::release(&use);
        XMLCh* default_string = XMLString::transcode(ACE_TEXT_ALWAYS_CHAR("default"));
        char* default_charp = XMLString::transcode(attributes_in.getValue(default_string));
        XMLString::release(&default_string);
        std::string defaultAttribute = default_charp;
        XMLString::release(&default_charp);
        definition += ACE_TEXT_ALWAYS_CHAR(" ");
        if (useAttribute != ACE_TEXT_ALWAYS_CHAR("required"))
          definition += ACE_TEXT_ALWAYS_CHAR("0");
        else
          definition += ACE_TEXT_ALWAYS_CHAR("1");
        definition += ACE_TEXT_ALWAYS_CHAR(" ");
        definition += ACE_TEXT_ALWAYS_CHAR("1");
      } // end ELSE

      myCurrentDefinitionHandler->handleData(definition);

      break;
    }
    case XML_UNION:
    {
      // the union members come in the "memberTypes" attribute of this element
      try
      {
        myCurrentDefinitionHandler = new Handle_XMLUnion(myCurrentOutputFile,
                                                         myEmitTaggedUnions,
														 myEmitClassQualifiers,
                                                         myTypePrefix,
                                                         myTypePostfix);
      }
      catch (...)
      {
        ACE_DEBUG((LM_ERROR,
                    ACE_TEXT("caught exception in ::new, aborting\n")));

        OutOfMemoryException exception;
        throw exception;
      }
      if (!myCurrentDefinitionHandler)
      {
        ACE_DEBUG((LM_ERROR,
                    ACE_TEXT("failed to allocate memory, aborting\n")));

        OutOfMemoryException exception;
        throw exception;
      } // end IF

      // init handler
      myCurrentDefinitionHandler->startElement(myCurrentElementName);

      // extract memberTypes attribute
      XMLCh* memberTypes_string = XMLString::transcode(ACE_TEXT_ALWAYS_CHAR("memberTypes"));
      char* memberTypes = XMLString::transcode(attributes_in.getValue(memberTypes_string));
      XMLString::release(&memberTypes_string);

      std::string definition = memberTypes;
      XMLString::release(&memberTypes);

      myCurrentDefinitionHandler->handleData(definition);

      break;
    }
    default:
    {
      ACE_DEBUG((LM_WARNING,
                 ACE_TEXT("unknown/invalid XML element: %d, continuing\n"),
                 typeOfElement));

      break;
    }
  } // end SWITCH
}

void XML_Handler::error(const SAXParseException& exception_in)
{
  ACE_TRACE(ACE_TEXT("XML_Handler::error"));

  char* message = XMLString::transcode(exception_in.getMessage());
  ACE_ASSERT(message);

  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("XML_Handler::error(file: \"%s\", line: %d, column: %d): \"%s\"\n"),
             exception_in.getSystemId(),
             exception_in.getLineNumber(),
             exception_in.getColumnNumber(),
             message));

  // clean up
  XMLString::release(&message);
}

void XML_Handler::fatalError(const SAXParseException& exception_in)
{
  ACE_TRACE(ACE_TEXT("XML_Handler::fatalError"));

  char* message = XMLString::transcode(exception_in.getMessage());
  ACE_ASSERT(message);

  ACE_DEBUG((LM_CRITICAL,
             ACE_TEXT("XML_Handler::fatalError(file: \"%s\", line: %d, column: %d): \"%s\"\n"),
             exception_in.getSystemId(),
             exception_in.getLineNumber(),
             exception_in.getColumnNumber(),
             message));

  // clean up
  XMLString::release(&message);
}

void XML_Handler::warning(const SAXParseException& exception_in)
{
  ACE_TRACE(ACE_TEXT("XML_Handler::warning"));

  char* message = XMLString::transcode(exception_in.getMessage());
  ACE_ASSERT(message);

  ACE_DEBUG((LM_WARNING,
             ACE_TEXT("XML_Handler::warning(file: \"%s\", line: %d, column: %d): \"%s\"\n"),
             exception_in.getSystemId(),
             exception_in.getLineNumber(),
             exception_in.getColumnNumber(),
             message));

  // clean up
  XMLString::release(&message);
}

XML_Handler::XMLElementType XML_Handler::stringToXMLElementType(const std::string& elementString_in) const
{
  ACE_TRACE(ACE_TEXT("XML_Handler::stringToXMLElementType"));

  if (elementString_in == ACE_TEXT_ALWAYS_CHAR("schema"))
  {
    return XML_SCHEMA;
  } // end IF
  else if (elementString_in == ACE_TEXT_ALWAYS_CHAR("annotation"))
  {
    return XML_ANNOTATION;
  } // end IF
  else if (elementString_in == ACE_TEXT_ALWAYS_CHAR("documentation"))
  {
    return XML_DOCUMENTATION;
  } // end IF
  else if (elementString_in == ACE_TEXT_ALWAYS_CHAR("simpleType"))
  {
    return XML_SIMPLETYPE;
  } // end IF
  else if (elementString_in == ACE_TEXT_ALWAYS_CHAR("restriction"))
  {
    return XML_RESTRICTION;
  } // end IF
  else if (elementString_in == ACE_TEXT_ALWAYS_CHAR("enumeration"))
  {
    return XML_ENUMERATION;
  } // end IF
  else if (elementString_in == ACE_TEXT_ALWAYS_CHAR("complexType"))
  {
    return XML_COMPLEXTYPE;
  } // end IF
  else if (elementString_in == ACE_TEXT_ALWAYS_CHAR("complexContent"))
  {
    return XML_COMPLEXCONTENT;
  } // end IF
  else if (elementString_in == ACE_TEXT_ALWAYS_CHAR("sequence"))
  {
    return XML_SEQUENCE;
  } // end IF
  else if (elementString_in == ACE_TEXT_ALWAYS_CHAR("attribute"))
  {
    return XML_ATTRIBUTE;
  } // end IF
  else if (elementString_in == ACE_TEXT_ALWAYS_CHAR("element"))
  {
    return XML_ELEMENT;
  } // end IF
  else if (elementString_in == ACE_TEXT_ALWAYS_CHAR("include"))
  {
    return XML_INCLUDE;
  } // end IF
  else if (elementString_in == ACE_TEXT_ALWAYS_CHAR("extension"))
  {
    return XML_EXTENSION;
  } // end IF
  else if (elementString_in == ACE_TEXT_ALWAYS_CHAR("union"))
  {
    return XML_UNION;
  } // end IF
  else
  {
    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("unknown XML element type: \"%s\", aborting\n"),
               elementString_in.c_str()));
  } // end ELSE

  return XML_INVALID;
}

void XML_Handler::insertPreamble(std::ofstream& targetStream_inout)
{
  ACE_TRACE(ACE_TEXT("XML_Handler::insertPreamble"));

  // step1: insert file contents as-is
  targetStream_inout << myPreamble;
  targetStream_inout << std::endl;

  // step2: insert edit warning
  targetStream_inout << ACE_TEXT_ALWAYS_CHAR("// -------------------------------- * * * -------------------------------- //") << std::endl;
  targetStream_inout << ACE_TEXT_ALWAYS_CHAR("// PLEASE NOTE: this file was/is generated by ");
// *NOTE*: clean this up !
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define XML2CPPCODE_PACKAGE_STRING ACE_TEXT("XML2CppCode 0.1")
#endif
  targetStream_inout << XML2CPPCODE_PACKAGE_STRING << std::endl;
  targetStream_inout << ACE_TEXT_ALWAYS_CHAR("// -------------------------------- * * * -------------------------------- //") << std::endl;
  targetStream_inout << std::endl;
}

void XML_Handler::insertMultipleIncludeProtection(const bool& usePragmaOnce_in,
                                                  const std::string& filename_in,
                                                  std::ofstream& targetStream_inout)
{
  ACE_TRACE(ACE_TEXT("XML_Handler::insertMultipleIncludeProtection"));

  if (usePragmaOnce_in)
    targetStream_inout << ACE_TEXT_ALWAYS_CHAR("#pragma once") << std::endl;

  // generate a "unique" identifier
  std::string definition = filename_in;
  // replace '.' with '_'s
  std::string dot = ACE_TEXT_ALWAYS_CHAR(".");
  std::string::size_type position = std::string::npos;
  while ((position = definition.find(dot)) != std::string::npos)
  {
    definition.replace(position, 1, ACE_TEXT_ALWAYS_CHAR("_"));
  } // end WHILE
  // transform to uppercase
  std::transform(definition.begin(),
	             definition.end(),
				 definition.begin(),
				 std::bind2nd(std::ptr_fun(&std::toupper<char>), std::locale("")));

  targetStream_inout << ACE_TEXT_ALWAYS_CHAR("#ifndef ");
  targetStream_inout << definition << std::endl;
  targetStream_inout << ACE_TEXT_ALWAYS_CHAR("#define ");
  targetStream_inout << definition << std::endl;
  targetStream_inout << std::endl;
}

void XML_Handler::insertPostscript(std::ofstream& targetStream_inout)
{
  ACE_TRACE(ACE_TEXT("XML_Handler::insertPostscript"));

  targetStream_inout << ACE_TEXT_ALWAYS_CHAR("#endif");
  targetStream_inout << std::endl;
}

void XML_Handler::insertIncludeHeaders(const XML2CppCode_Headers_t& headers_in,
                                       const bool& includeStdVector_in,
                                       std::ofstream& targetStream_inout)
{
  ACE_TRACE(ACE_TEXT("XML_Handler::insertIncludeHeaders"));

  if (includeStdVector_in)
    targetStream_inout << ACE_TEXT_ALWAYS_CHAR("#include <vector>") << std::endl;

  for (XML2CppCode_HeadersIterator_t iterator = headers_in.begin();
       iterator != headers_in.end();
       iterator++)
  {
    targetStream_inout << ACE_TEXT_ALWAYS_CHAR("#include \"");
    targetStream_inout << *iterator;
    targetStream_inout << ACE_TEXT_ALWAYS_CHAR("\"");
    targetStream_inout << std::endl;
  } // end FOR
  targetStream_inout << std::endl;
}
