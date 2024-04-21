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

#ifndef RPG_COMMON_XSDERRORHANDLER_H
#define RPG_COMMON_XSDERRORHANDLER_H

#include "xercesc/sax/ErrorHandler.hpp"
#include "xercesc/sax/SAXParseException.hpp"

#include "xsd/cxx/xml/error-handler.hxx"

class RPG_Common_XSDErrorHandler
 : public ::xsd::cxx::xml::error_handler<char>
{
  typedef ::xsd::cxx::xml::error_handler<char> inherited;

 public:
  RPG_Common_XSDErrorHandler ();
  inline virtual ~RPG_Common_XSDErrorHandler () {}

  inline void reset () { myFailed = false; }
  inline bool failed () { return myFailed; }

  virtual bool handle (const std::string&,                             // id
                       unsigned long,                                  // line
                       unsigned long,                                  // column
                       ::xsd::cxx::xml::error_handler<char>::severity, // severity
                       const std::string&);                            // message

 private:
  bool myFailed;
};

static RPG_Common_XSDErrorHandler RPG_XSDErrorHandler;

// ******************************************************************** //

class RPG_Common_XercesErrorHandler
 : public ::xercesc::ErrorHandler
{
  typedef ::xercesc::ErrorHandler inherited;

 public:
  RPG_Common_XercesErrorHandler ();
  inline virtual ~RPG_Common_XercesErrorHandler() {}

  inline bool failed () { return myFailed; }

  virtual void warning (const ::xercesc::SAXParseException&);
  virtual void error (const ::xercesc::SAXParseException&);
  virtual void fatalError (const ::xercesc::SAXParseException&);
  inline virtual void resetErrors () { myFailed = false; }

 private:
  bool myFailed;
};

static RPG_Common_XercesErrorHandler RPG_XercesErrorHandler;

#endif
