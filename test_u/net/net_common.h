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

#ifndef NET_COMMON_H
#define NET_COMMON_H

#include <glade/glade.h>

#include <ace/Synch.h>

struct Net_GTK_CBData_t
{
 inline Net_GTK_CBData_t()
  : lock(NULL, NULL),
//    gtk_main_quit_invoked(false),
    xml(NULL)
 { };

  mutable ACE_Thread_Mutex lock;
//  bool                     gtk_main_quit_invoked;
  GladeXML*                xml;
};

#endif
