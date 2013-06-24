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

#ifndef XML_PARSER_H
#define XML_PARSER_H

#include <ace/Global_Macros.h>
#include <ace/Singleton.h>
#include <ace/Synch.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class XML_Parser
{
  // we use the singleton pattern, so we need to enable access to the ctor/dtors
  friend class ACE_Singleton<XML_Parser,
                             ACE_Thread_Mutex>;

 public:
  // init item dictionary
  void parseFile(const std::string&, // emit class qualifier (DLL import/export symbols) ?
				 const bool&,        // emit string conversion utilities ?
                 const bool&,        // emit "tagged" unions ?
                 const std::string&, // (XMLSchema) filename
                 const bool&,        // generate separate include header
                 const std::string&, // target directory
                 const std::string&, // preamble filename
                 const bool&,        // file-per-definition ?
                 const std::string&, // type prefix
                 const std::string&, // type postfix
                 const bool&);       // validate document

 private:
  // safety measures
  XML_Parser();
  virtual ~XML_Parser();
  ACE_UNIMPLEMENTED_FUNC(XML_Parser(const XML_Parser&));
  ACE_UNIMPLEMENTED_FUNC(XML_Parser& operator=(const XML_Parser&));
};

typedef ACE_Singleton<XML_Parser,
                      ACE_Thread_Mutex> XML_PARSER_SINGLETON;

#endif
