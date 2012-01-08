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

#ifndef RPG_STREAM_MODULE_H
#define RPG_STREAM_MODULE_H

#include "rpg_stream_exports.h"

#include <ace/Global_Macros.h>
#include <ace/Module.h>
#include <ace/Synch.h>

#include <string>

// forward declaration(s)
class RPG_Stream_IRefCount;

class RPG_Stream_Export RPG_Stream_Module
 : public ACE_Module<ACE_MT_SYNCH>
{
 public:
  // define convenient types
  typedef ACE_Module<ACE_MT_SYNCH> MODULE_TYPE;
  typedef ACE_Task<ACE_MT_SYNCH> TASK_TYPE;

  RPG_Stream_Module(const std::string&, // name
                TASK_TYPE*,         // handle to writer task
                TASK_TYPE*,         // handle to reader task
                RPG_Stream_IRefCount*); // object counter
  virtual ~RPG_Stream_Module();

  // *NOTE*: streams may call this to reset writer/reader tasks
  // and re-use existing modules
  // --> needed after call to MODULE_TYPE::close(), which cannot be
  // overriden (not "virtual")
  // *WARNING*: DON'T call this from within module_closed()
  // --> creates endless loops (and eventually, stack overflows)...
  void resetReaderWriter();

 protected:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_Module());
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_Module(const RPG_Stream_Module&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_Module& operator=(const RPG_Stream_Module&));

 private:
  typedef ACE_Module<ACE_MT_SYNCH> inherited;

  TASK_TYPE* myWriter;
  TASK_TYPE* myReader;
};

#endif
