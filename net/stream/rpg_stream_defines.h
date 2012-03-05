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

#ifndef RPG_STREAM_DEFINES_H
#define RPG_STREAM_DEFINES_H

// general stuff
#define RPG_STREAM_DEF_MAX_TIMESTAMP_STRING_LENGTH 64

// trace log
// *PORTABILITY*: pathnames are not portable, so we (try to) use %TEMP% for Windows...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
//#define RPG_STREAM_DEF_LOG_DIRECTORY               ACE_OS::getenv(ACE_TEXT_ALWAYS_CHAR("TEMP"))
#define RPG_STREAM_DEF_LOG_DIRECTORY               "TEMP"
#else
#define RPG_STREAM_DEF_LOG_DIRECTORY               "/var/tmp"
#endif

// queue / task / buffers
#define RPG_STREAM_DEF_GROUP_ID_TASK               11
// *IMPORTANT NOTE*: be generous here - any of these MAY seriously
// affect performance
#define RPG_STREAM_DEF_MAX_QUEUE_SLOTS             10000
// *IMPORTANT NOTE*: static heap memory consumption can probably be approximated
// as RPG_STREAM_DEF_MAX_MESSAGES * sizeof(stream-message-type) bytes
#define RPG_STREAM_DEF_MAX_MESSAGES                1000

#define RPG_STREAM_DEF_BUFFER_SIZE                 1024 // 1 kB

#endif
