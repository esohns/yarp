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
#ifndef XML_HANDLER_BASE_H
#define XML_HANDLER_BASE_H

#include <ace/Global_Macros.h>

#include <xercesc/sax2/DefaultHandler.hpp>

XERCES_CPP_NAMESPACE_USE

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class XML_Handler_Base
 : public DefaultHandler//,
//    private XMLFormatTarget
{
 public:
  XML_Handler_Base();
  virtual ~XML_Handler_Base();

  // -----------------------------------------------------------------------
  // Implementation of the SAX ContentHandler interface
  // -----------------------------------------------------------------------
  virtual void characters(const XMLCh* const,  // chars
                          const unsigned int); // length
  virtual void endDocument();
  virtual void endElement(const XMLCh* const,  // uri
                          const XMLCh* const,  // localname
                          const XMLCh* const); // qname
  virtual void ignorableWhitespace(const XMLCh* const,  // chars
                                   const unsigned int); // length
  virtual void processingInstruction(const XMLCh* const,  // target
                                     const XMLCh* const); // data
  virtual void setDocumentLocator(const Locator* const); // locator
  virtual void startDocument();
  virtual void startElement(const XMLCh* const, // uri
                            const XMLCh* const, // localname
                            const XMLCh* const, // qname
                            const Attributes&); // attrs
  virtual void startPrefixMapping(const XMLCh* const,  // prefix
                                  const XMLCh* const); // uri
  virtual void endPrefixMapping(const XMLCh* const); // prefix
  virtual void skippedEntity(const XMLCh* const); // name

  virtual void resetDocument();

  // -----------------------------------------------------------------------
  // Implementation of the SAX ErrorHandler interface
  // -----------------------------------------------------------------------
  virtual void error(const SAXParseException&); // exception
  virtual void fatalError(const SAXParseException&); // exception
  virtual void warning(const SAXParseException&); // exception

  virtual void resetErrors();

  // -----------------------------------------------------------------------
  // Implementation of the SAX EntityResolver interface
  // -----------------------------------------------------------------------
  virtual InputSource* resolveEntity(const XMLCh* const,  // publicId
                                     const XMLCh* const); // systemId

  // -----------------------------------------------------------------------
  // Implementation of the SAX DTDHandler interface
  // -----------------------------------------------------------------------
  virtual void notationDecl(const XMLCh* const,  // name
                            const XMLCh* const,  // publicId
                            const XMLCh* const); // systemId
  virtual void unparsedEntityDecl(const XMLCh* const,  // name
                                  const XMLCh* const,  // publicId
                                  const XMLCh* const,  // systemId
                                  const XMLCh* const); // notationName
  virtual void resetDocType();

  // -----------------------------------------------------------------------
  // Implementation of the SAX LexicalHandler interface
  // -----------------------------------------------------------------------
  virtual void comment(const XMLCh* const,  // chars
                       const unsigned int); // length
  virtual void endCDATA();
  virtual void endDTD();
  virtual void endEntity(const XMLCh* const); // name
  virtual void startCDATA();
  virtual void startDTD(const XMLCh* const,  // name
                        const XMLCh* const,  // publicId
                        const XMLCh* const); // systemId
  virtual void startEntity(const XMLCh* const); // name

  // -----------------------------------------------------------------------
  // Implementation of the SAX DeclHandler interface
  // -----------------------------------------------------------------------
  virtual void elementDecl(const XMLCh* const,  // name
                           const XMLCh* const); // model
  virtual void attributeDecl(const XMLCh* const,  // eName
                             const XMLCh* const,  // aName
                             const XMLCh* const,  // type
                             const XMLCh* const,  // mode
                             const XMLCh* const); // value
  virtual void internalEntityDecl(const XMLCh* const,  // name
                                  const XMLCh* const); // value
  virtual void externalEntityDecl(const XMLCh* const,  // name
                                  const XMLCh* const,  // publicId
                                  const XMLCh* const); // systemId

//   // -----------------------------------------------------------------------
//   // Implementation of the XMLFormatTarget interface
//   // -----------------------------------------------------------------------
//   void writeChars(const XMLByte* const); // toWrite
//   virtual void writeChars(const XMLByte* const, // toWrite
//                           const XMLSize_t,      // count
//                           XMLFormatter* const); // formatter

 private:
  typedef DefaultHandler inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(XML_Handler_Base(const XML_Handler_Base&));
  ACE_UNIMPLEMENTED_FUNC(XML_Handler_Base& operator=(const XML_Handler_Base&));
};

#endif
