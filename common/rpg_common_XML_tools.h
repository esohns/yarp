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

#ifndef RPG_COMMON_XML_TOOLS_H
#define RPG_COMMON_XML_TOOLS_H

#include <string>
#include <vector>

#include "xercesc/framework/XMLGrammarPool.hpp"
#include "xercesc/sax2/SAX2XMLReader.hpp"

#include "ace/Global_Macros.h"

// forward declarations
struct dirent;

class RPG_Common_XML_Tools
{
 public:
   static bool initialize (const std::vector<std::string>&); // schema paths
   static void finalize ();

   static ::xercesc::SAX2XMLReader* parser ();

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Common_XML_Tools ())
  ACE_UNIMPLEMENTED_FUNC (virtual ~RPG_Common_XML_Tools ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Common_XML_Tools (const RPG_Common_XML_Tools&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Common_XML_Tools& operator=(const RPG_Common_XML_Tools&))

  // helper methods
  static int dirent_selector (const dirent*);
  static int dirent_comparator (const dirent**,
                                const dirent**);

  static ::xercesc::XMLGrammarPool* grammarPool_;
  static ::xercesc::SAX2XMLReader*  parser_;
  static bool                       initialized_;
};

#endif
