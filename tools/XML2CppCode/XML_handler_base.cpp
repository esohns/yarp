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

#include "XML_handler_base.h"

#include "xercesc/util/XMLUniDefs.hpp"
#include "xercesc/sax2/Attributes.hpp"

#include "ace/Log_Msg.h"

XML_Handler_Base::XML_Handler_Base()
 : inherited()
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::XML_Handler_Base"));

}

XML_Handler_Base::~XML_Handler_Base()
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::~XML_Handler_Base"));

}

void XML_Handler_Base::characters(const XMLCh* const,
                                  const unsigned int)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::characters"));

}

void XML_Handler_Base::endDocument()
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::endDocument"));

}

void XML_Handler_Base::endElement(const XMLCh* const,
                                  const XMLCh* const,
                                  const XMLCh* const)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::endElement"));

}

void XML_Handler_Base::ignorableWhitespace(const XMLCh* const,
                                           const unsigned int)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::ignorableWhitespace"));

}

void XML_Handler_Base::processingInstruction(const XMLCh* const,
                                             const XMLCh* const)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::processingInstruction"));

}

void XML_Handler_Base::setDocumentLocator(const Locator* const)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::setDocumentLocator"));

}

void XML_Handler_Base::startDocument()
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::startDocument"));

}

void XML_Handler_Base::startElement(const XMLCh* const,
                                    const XMLCh* const,
                                    const XMLCh* const,
                                    const Attributes&)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::startElement"));

  // delegate apropriately

}

void XML_Handler_Base::startPrefixMapping(const XMLCh* const,
                                          const XMLCh* const)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::startPrefixMapping"));

}

void XML_Handler_Base::endPrefixMapping(const XMLCh* const)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::endPrefixMapping"));

}

void XML_Handler_Base::skippedEntity(const XMLCh* const)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::skippedEntity"));

}

void XML_Handler_Base::resetDocument()
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::resetDocument"));

}

void XML_Handler_Base::error(const SAXParseException& exception_in)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::error"));

  char* message = XMLString::transcode(exception_in.getMessage());
  ACE_ASSERT(message);

  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("XML_Handler_Base::error(file: \"%s\", line: %d, column: %d): \"%s\"\n"),
             exception_in.getSystemId(),
             exception_in.getLineNumber(),
             exception_in.getColumnNumber(),
             message));

  // clean up
  XMLString::release(&message);
}

void XML_Handler_Base::fatalError(const SAXParseException& exception_in)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::fatalError"));

  char* message = XMLString::transcode(exception_in.getMessage());
  ACE_ASSERT(message);

  ACE_DEBUG((LM_CRITICAL,
             ACE_TEXT("XML_Handler_Base::fatalError(file: \"%s\", line: %d, column: %d): \"%s\"\n"),
             exception_in.getSystemId(),
             exception_in.getLineNumber(),
             exception_in.getColumnNumber(),
             message));

  // clean up
  XMLString::release(&message);
}

void XML_Handler_Base::warning(const SAXParseException& exception_in)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::warning"));

  char* message = XMLString::transcode(exception_in.getMessage());
  ACE_ASSERT(message);

  ACE_DEBUG((LM_WARNING,
             ACE_TEXT("XML_Handler_Base::warning(file: \"%s\", line: %d, column: %d): \"%s\"\n"),
             exception_in.getSystemId(),
             exception_in.getLineNumber(),
             exception_in.getColumnNumber(),
             message));

  // clean up
  XMLString::release(&message);
}

void XML_Handler_Base::resetErrors()
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::resetErrors"));

}

InputSource* XML_Handler_Base::resolveEntity(const XMLCh* const,
                                             const XMLCh* const)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::resolveEntity"));

  return NULL;
}

void XML_Handler_Base::notationDecl(const XMLCh* const,
                                    const XMLCh* const,
                                    const XMLCh* const)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::notationDecl"));

}

void XML_Handler_Base::unparsedEntityDecl(const XMLCh* const,
                                          const XMLCh* const,
                                          const XMLCh* const,
                                          const XMLCh* const)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::unparsedEntityDecl"));

}

void XML_Handler_Base::resetDocType()
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::resetDocType"));

}

void XML_Handler_Base::comment(const XMLCh* const,
                               const unsigned int)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::comment"));

}

void XML_Handler_Base::endCDATA()
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::endCDATA"));

}

void XML_Handler_Base::endDTD()
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::endDTD"));

}

void XML_Handler_Base::endEntity(const XMLCh* const)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::endEntity"));

}

void XML_Handler_Base::startCDATA()
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::startCDATA"));

}

void XML_Handler_Base::startDTD(const XMLCh* const,
                                const XMLCh* const,
                                const XMLCh* const)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::startDTD"));

}

void XML_Handler_Base::startEntity(const XMLCh* const)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::startEntity"));

}

void XML_Handler_Base::elementDecl(const XMLCh* const,
                                   const XMLCh* const)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::elementDecl"));

}

void XML_Handler_Base::attributeDecl(const XMLCh* const,
                                     const XMLCh* const,
                                     const XMLCh* const,
                                     const XMLCh* const,
                                     const XMLCh* const)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::attributeDecl"));

}

void XML_Handler_Base::internalEntityDecl(const XMLCh* const,
                                          const XMLCh* const)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::internalEntityDecl"));

}

void XML_Handler_Base::externalEntityDecl(const XMLCh* const,
                                          const XMLCh* const,
                                          const XMLCh* const)
{
  ACE_TRACE(ACE_TEXT("XML_Handler_Base::externalEntityDecl"));

}
