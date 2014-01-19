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

#ifndef RPG_STREAM_STREAMMODULE_BASE_H
#define RPG_STREAM_STREAMMODULE_BASE_H

#include "rpg_stream_module_base.h"

#include <ace/Global_Macros.h>
#include <ace/Stream_Modules.h>

#include <string>

// forward declaration(s)
class RPG_Stream_IRefCount;

template <typename TaskSynchType,
          typename ReaderTaskType,
          typename WriterTaskType>
class RPG_Stream_StreamModule_t
 : public RPG_Stream_Module_Base_t<TaskSynchType,
                                   ReaderTaskType,
                                   WriterTaskType>
{
 public:
  RPG_Stream_StreamModule_t(const std::string&,     // name
                            RPG_Stream_IRefCount*); // object counter
  virtual ~RPG_Stream_StreamModule_t();

 protected:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_StreamModule_t());
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_StreamModule_t(const RPG_Stream_StreamModule_t&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_StreamModule_t& operator=(const RPG_Stream_StreamModule_t&));

 private:
  typedef RPG_Stream_Module_Base_t<TaskSynchType,
                                   ReaderTaskType,
                                   WriterTaskType> inherited;

  ReaderTaskType myReader;
  WriterTaskType myWriter;
};

template <typename TaskSynchType,
          typename TaskType>
class RPG_Stream_StreamModuleInputOnly_t
 : public RPG_Stream_StreamModule_t<TaskSynchType,
                                    ACE_Thru_Task<TaskSynchType>, // through-task
                                    TaskType>
{
 public:
  RPG_Stream_StreamModuleInputOnly_t(const std::string&,     // name
                                     RPG_Stream_IRefCount*); // object counter
  virtual ~RPG_Stream_StreamModuleInputOnly_t();

 protected:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_StreamModuleInputOnly_t());
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_StreamModuleInputOnly_t(const RPG_Stream_StreamModuleInputOnly_t&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_StreamModuleInputOnly_t& operator=(const RPG_Stream_StreamModuleInputOnly_t&));

 private:
  typedef RPG_Stream_StreamModule_t<TaskSynchType,
                                    ACE_Thru_Task<TaskSynchType>, // through-task
                                    TaskType> inherited;
};

// include template implementation
#include "rpg_stream_streammodule_base.inl"

// *NOTE*: use this macro to instantiate the module definitions
// *IMPORTANT NOTE*: TASK_SYNCH_TYPE is [ACE_MT_SYNCH | ACE_NULL_SYNCH] and must
// correspond to the actual TASK_TYPE declaration !
#define DATASTREAM_MODULE_INPUT_ONLY(TASK_SYNCH_TYPE,\
                                     TASK_TYPE) typedef RPG_Stream_StreamModuleInputOnly_t<TASK_SYNCH_TYPE, TASK_TYPE> TASK_TYPE##_Module
#define DATASTREAM_MODULE_INPUT_ONLY_T(TASK_SYNCH_TYPE,\
                                       TASK_TYPE,\
                                       NAME) typedef RPG_Stream_StreamModuleInputOnly_t<TASK_SYNCH_TYPE, TASK_TYPE> NAME##_Module
#define DATASTREAM_MODULE_DUPLEX(TASK_SYNCH_TYPE,\
                                 READER_TYPE,\
                                 WRITER_TYPE,\
                                 NAME) typedef RPG_Stream_StreamModule_t<TASK_SYNCH_TYPE,\
                                                                         READER_TYPE,\
                                                                         WRITER_TYPE> NAME##_Module

#endif
