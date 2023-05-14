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

#include "XML_parser.h"

#include "ace/OS.h"
#include "ace/ACE.h"
#include "ace/Log_Msg.h"

#include "xercesc/sax2/SAX2XMLReader.hpp"
#include "xercesc/sax2/XMLReaderFactory.hpp"
#include "xercesc/util/OutOfMemoryException.hpp"

#include "XML_handler.h"

XERCES_CPP_NAMESPACE_USE

XML_Parser::XML_Parser ()
{
  ACE_TRACE (ACE_TEXT ("XML_Parser::XML_Parser"));

}

void
XML_Parser::parseFile (const std::string& emitClassQualifier_in,
                       bool emitStringConversionUtilities_in,
                       bool emitTaggedUnions_in,
                       const std::string& filename_in,
                       bool generateIncludeHeader_in,
                       const std::string& directory_in,
                       const std::string& preambleFilename_in,
                       bool filePerDefinition_in,
                       const std::string& typePrefix_in,
                       const std::string& typePostfix_in,
                       bool validate_in)
{
  ACE_TRACE (ACE_TEXT ("XML_Parser::parseFile"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("parsing (XML) file \"%s\"...\n"),
              ACE_TEXT (filename_in.c_str ())));

  // Create a SAX parser object. Then, according to what we were told on
  // the command line, set it to validate or not
  SAX2XMLReader* reader = NULL;
  try {
    reader = XMLReaderFactory::createXMLReader ();
  } catch (...) {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in XMLReaderFactory::createXMLReader, returning\n")));
    return;
  }
  if (!reader)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to XMLReaderFactory::createXMLReader, returning\n")));
    return;
  } // end IF

  // Then, according to what we were told on
  // the command line, set it to validate or not.
  if (validate_in)
  {
    // "automatic" validation...
    reader->setFeature(XMLUni::fgSAX2CoreValidation, true);
    reader->setFeature(XMLUni::fgXercesDynamic, true);
//     reader->setFeature(XMLUni::fgXercesDynamic, false);
  } // end IF
  else
  {
    reader->setFeature(XMLUni::fgSAX2CoreValidation, false);
  } // end ELSE

  reader->setFeature (XMLUni::fgSAX2CoreNameSpaces, true);
  reader->setFeature (XMLUni::fgXercesSchema, true);
//   reader->setFeature(XMLUni::fgXercesHandleMultipleImports, true);
  reader->setFeature (XMLUni::fgXercesSchemaFullChecking, true);
  reader->setFeature (XMLUni::fgSAX2CoreNameSpacePrefixes, true);

  // Create the handler object and install it as the document and error
  // handler for the parser. Then parse the file and catch any exceptions
  // that propogate out
  int errorCount = 0;
  std::string schemaFilename = ACE::basename(filename_in.c_str());
  std::string preamble;
  ACE_stat stat;
  if (!preambleFilename_in.empty() &&
      (ACE_OS::stat (preambleFilename_in.c_str (), &stat) != -1)) // exists ?
  {
    std::ifstream preambleStream (preambleFilename_in.c_str(), std::ios_base::in);
    if (!preambleStream.is_open ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to open preamble file: \"%s\", returning\n"),
                  preambleFilename_in.c_str ()));
      return;
    } // end IF
    std::string line;
    while (std::getline (preambleStream, line))
    {
      preamble += line;
      preamble += ACE_TEXT_ALWAYS_CHAR ("\n");
    } // end WHILE
    preambleStream.close ();
  } // end IF

  XML_Handler handler (emitClassQualifier_in,
                       emitStringConversionUtilities_in,
                       emitTaggedUnions_in,
                       schemaFilename,
                       generateIncludeHeader_in,
                       directory_in,
                       preamble,
                       filePerDefinition_in,
                       typePrefix_in,
                       typePostfix_in);
  reader->setContentHandler (&handler);
  reader->setErrorHandler (&handler);

  try {
    reader->parse (filename_in.c_str ());
  } catch (const OutOfMemoryException& exception_in) {
    char* message = XMLString::transcode (exception_in.getMessage ());
    ACE_ASSERT (message);
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught OutOfMemoryException: \"%s\", returning\n"),
                ACE_TEXT (message)));

    // clean up
    XMLString::release (&message);
    delete reader; reader = NULL;

    return;
  } catch (const XMLException& exception_in) {
    char* message = XMLString::transcode (exception_in.getMessage ());
    ACE_ASSERT (message);
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught XMLException: \"%s\", returning\n"),
                ACE_TEXT (message)));

    // clean up
    XMLString::release (&message);
    delete reader; reader = NULL;

    return;
  } catch (const SAXParseException& exception_in) {
    char* message = XMLString::transcode (exception_in.getMessage ());
    ACE_ASSERT (message);
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught SAXParseException: \"%s\", returning\n"),
                ACE_TEXT (message)));

    // clean up
    XMLString::release (&message);
    delete reader; reader = NULL;

    return;
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("SAX2XMLReader: caught exception, returning\n")));

    // clean up
    delete reader; reader = NULL;

    return;
  }
  errorCount = static_cast<int> (reader->getErrorCount ());

  // Delete the parser itself. Must be done prior to calling Terminate
  delete reader; reader = NULL;

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("parsing (XML) file \"%s\"...finished (%d errors)\n"),
              ACE_TEXT (filename_in.c_str ()),
              errorCount));
}
