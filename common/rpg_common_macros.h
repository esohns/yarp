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

#ifndef RPG_COMMON_MACROS_H
#define RPG_COMMON_MACROS_H

#define RPG_TRACE_IMPL(X) ACE_Trace ____ (ACE_TEXT (X), __LINE__, ACE_TEXT (__FILE__))

// by default tracing is turned off
#if !defined (RPG_NTRACE)
#  define RPG_NTRACE 1
#endif /* RPG_NTRACE */

#if (RPG_NTRACE == 1)
#  define RPG_TRACE(X)
#else
#  if !defined (RPG_HAS_TRACE)
#    define RPG_HAS_TRACE
#  endif /* RPG_HAS_TRACE */
#  define RPG_TRACE(X) RPG_TRACE_IMPL(X)
#  include <ace/Trace.h>
#endif /* RPG_NTRACE */

#endif
