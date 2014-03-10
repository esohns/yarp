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

#include "xml2cppcode_common_tools.h"

#include <ace/Log_Msg.h>

const bool XML2CppCode_Common_Tools::XMLintegratedtypeToString(const std::string& typeSpecifier_in,
                                                               std::string& result_out)
{
  ACE_TRACE(ACE_TEXT("XML2CppCode_Common_Tools::XMLintegratedtypeToString"));

  if (typeSpecifier_in == ACE_TEXT_ALWAYS_CHAR("boolean"))
    result_out = ACE_TEXT_ALWAYS_CHAR("bool");
  else if (typeSpecifier_in == ACE_TEXT_ALWAYS_CHAR("byte"))
    result_out = ACE_TEXT_ALWAYS_CHAR("signed char");
  else if (typeSpecifier_in == ACE_TEXT_ALWAYS_CHAR("unsignedByte"))
    result_out = ACE_TEXT_ALWAYS_CHAR("unsigned char");
  else if (typeSpecifier_in == ACE_TEXT_ALWAYS_CHAR("short"))
    result_out = ACE_TEXT_ALWAYS_CHAR("short int");
  else if (typeSpecifier_in == ACE_TEXT_ALWAYS_CHAR("unsignedShort"))
    result_out = ACE_TEXT_ALWAYS_CHAR("unsigned short int");
  else if ((typeSpecifier_in == ACE_TEXT_ALWAYS_CHAR("integer")) ||
           (typeSpecifier_in == ACE_TEXT_ALWAYS_CHAR("int")))
    result_out = ACE_TEXT_ALWAYS_CHAR("int");
  else if (typeSpecifier_in == ACE_TEXT_ALWAYS_CHAR("unsignedInt"))
    result_out = ACE_TEXT_ALWAYS_CHAR("unsigned int");
  else if (typeSpecifier_in == ACE_TEXT_ALWAYS_CHAR("string"))
    result_out = ACE_TEXT_ALWAYS_CHAR("std::string");
  else
  {
//    ACE_DEBUG((LM_DEBUG,
//               ACE_TEXT("unknown type: \"%s\", returning as-is\n"),
//               ACE_TEXT(typeSpecifier_in.c_str())));

    result_out = typeSpecifier_in;

    return false;
  } // end ELSE

  return true;
}
