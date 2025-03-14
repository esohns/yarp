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

#ifndef XML_HANDLER_H
#define XML_HANDLER_H

#include <string>
#include <fstream>
#include <vector>
#include <stack>

#include "ace/Global_Macros.h"

#include "iXML_definition_handler.h"

#include "XML_handler_base.h"

// forward declarations
class Handle_XMLEnumeration;
class Handle_XMLSequence;

XERCES_CPP_NAMESPACE_USE

class XML_Handler
 : public XML_Handler_Base
// , private XMLFormatTarget
{
  typedef XML_Handler_Base inherited;

  friend class Handle_XMLEnumeration;
  friend class Handle_XMLSequence;

 public:
  XML_Handler (const std::string&,  // emit class qualifier (DLL import/export symbols) ?
               bool,                // emit string conversion utilities ?
               bool,                // emit "tagged" unions ?
               const std::string&,  // schema file
               bool,                // generate include header ?
               const std::string&,  // target output directory
               const std::string&,  // preamble
               bool,                // file-per-definition ?
               const std::string&,  // type prefix (used to generate identifiers for unions)
               const std::string&); // type postfix (used to generate type identifiers)
  inline virtual ~XML_Handler () {}

  // -----------------------------------------------------------------------
  // Implementation of the SAX ContentHandler interface
  // -----------------------------------------------------------------------
  virtual void endDocument();
  virtual void endElement (const XMLCh* const,  // uri
                           const XMLCh* const,  // localname
                           const XMLCh* const); // qname
  virtual void setDocumentLocator (const Locator* const); // locator
  virtual void startDocument ();
  virtual void startElement (const XMLCh* const, // uri
                             const XMLCh* const, // localname
                             const XMLCh* const, // qname
                             const Attributes&); // attrs

  // -----------------------------------------------------------------------
  // Implementation of the SAX ErrorHandler interface
  // -----------------------------------------------------------------------
  virtual void error (const SAXParseException&); // exception
  virtual void fatalError (const SAXParseException&); // exception
  virtual void warning (const SAXParseException&); // exception

 private:
  ACE_UNIMPLEMENTED_FUNC (XML_Handler (const XML_Handler&));
  ACE_UNIMPLEMENTED_FUNC (XML_Handler& operator= (const XML_Handler&));

  enum XMLElementType
  {
    XML_ANNOTATION = 0,
    XML_ATTRIBUTE,
    XML_CHOICE,
    XML_COMPLEXCONTENT,
    XML_COMPLEXTYPE,
    XML_DOCUMENTATION,
    XML_ELEMENT,
    XML_ENUMERATION,
    XML_EXTENSION,
    XML_INCLUDE,
    XML_RESTRICTION,
    XML_SCHEMA,
    XML_SEQUENCE,
    XML_SIMPLETYPE,
    XML_UNION,
    //
    XML_INVALID
  };

  // helper methods
  XMLElementType stringToXMLElementType (const std::string&) const; // element
  void insertPreamble (std::ofstream&); // file stream
  void insertMultipleIncludeProtection (bool,               // use "#pragma once" directive ?
                                        const std::string&, // (header) filename
                                        std::ofstream&);    // file stream
  void insertPostscript(std::ofstream&); // file stream
  typedef std::vector<std::string> XML2CppCode_Headers_t;
  void insertIncludeHeaders (const XML2CppCode_Headers_t&, // headers
                             bool,                         // include <vector> ?
                             std::ofstream&);              // file stream

  // helper types
  typedef XML2CppCode_Headers_t::const_iterator XML2CppCode_HeadersIterator_t;
  typedef std::stack<IXML_Definition_Handler*> XML2CppCode_Handlers_t;

  XML2CppCode_Handlers_t myDefinitionHandlers;

  std::ofstream          myIncludeHeaderFile;
  std::ofstream          myCurrentOutputFile;
  bool                   myCurrentHasBaseClass;
  std::string            myCurrentElementName;
  bool                   myIsFirstRelevantElement;
  std::string            myTargetDirectory;
  bool                   myFilePerDefinition;
  std::string            mySchemaFilename;
  std::string            myPreamble;
  std::string            myEmitClassQualifiers;
  bool                   myEmitStringConversionUtilities;
  bool                   myEmitTaggedUnions;
  bool                   myGenerateIncludeHeader;
  std::string            myTypePrefix;
  std::string            myTypePostfix;
  const Locator*         myLocator;
//   bool                      myIgnoreCharacters;
  XML2CppCode_Headers_t  myHeaders;
  bool                   myHeadersUseStdVector;
  unsigned int           myCurrentNestingLevel;
};

#endif
