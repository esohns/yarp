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

#ifndef RPG_STREAM_STREAMMODULE_H
#define RPG_STREAM_STREAMMODULE_H

#include "rpg_stream_module.h"

#include <ace/Global_Macros.h>

#include <string>

// forward declaration(s)
class RPG_Stream_IRefCount;

template <typename TaskType>
class RPG_Stream_StreamModule
 : public RPG_Stream_Module
{
 public:
  RPG_Stream_StreamModule(const std::string&,     // name
                          RPG_Stream_IRefCount*); // object counter
  virtual ~RPG_Stream_StreamModule();

 protected:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_StreamModule());
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_StreamModule(const RPG_Stream_StreamModule<TaskType>&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_StreamModule<TaskType>& operator=(const RPG_Stream_StreamModule<TaskType>&));

 private:
  typedef RPG_Stream_Module inherited;

  TaskType                    myTask;
  ACE_Thru_Task<ACE_MT_SYNCH> myReader;
};

// include template implementation
#include "rpg_stream_streammodule.i"

// *NOTE*: use this macro to instantiate the module definitions
// --> saves some typing...
#define DATASTREAM_MODULE(TASKTYPE) typedef RPG_Stream_StreamModule<TASKTYPE> TASKTYPE##_Module
#define DATASTREAM_MODULE_T(TASKTYPE, NAME) typedef RPG_Stream_StreamModule<TASKTYPE > NAME##_Module
#endif
