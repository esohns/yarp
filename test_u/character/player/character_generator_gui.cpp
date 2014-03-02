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

#include <SDL.h>

//#include <ace/OS.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
//int
//ACE_TMAIN(int argc_in,
//          ACE_TCHAR* argv_in[])
int
SDL_main(int argc_in,
         char** argv_in)
{
	RPG_TRACE(ACE_TEXT("::main"));

	test_u_main main_instance;
	return main_instance.run(argc_in,
		                       argv_in);
} // end main
#ifdef __cplusplus
}
#endif /* __cplusplus */
