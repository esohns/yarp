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

#include "test_u_main.h"

#include "ace/OS_main.h"

//#include "SDL_main.h"

#include "rpg_common_macros.h"

// ******* WORKAROUND *************
#if defined (ACE_WIN32) || defined (ACE_WIN64)
FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE* __cdecl __iob_func (void)
{
  return _iob;
}
#endif // ACE_WIN32 || ACE_WIN64
// ********************************

int
ACE_TMAIN (int argc_in,
           ACE_TCHAR* argv_in[])
{
  RPG_TRACE (ACE_TEXT ("::main"));

  test_u_main main_instance;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  return main_instance.run (argc_in,
                            argv_in);
#else
  return main_instance.run_i(argc_in,
                             argv_in);
#endif
} // end main
