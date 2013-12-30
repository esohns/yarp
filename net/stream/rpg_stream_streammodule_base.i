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

#include "rpg_common_macros.h"

template <typename TaskSynchType,
          typename ReaderTaskType,
          typename WriterTaskType>
RPG_Stream_StreamModule_t<TaskSynchType,
                          ReaderTaskType,
                          WriterTaskType>::RPG_Stream_StreamModule_t(const std::string& name_in,
                                                                     RPG_Stream_IRefCount* refCount_in)
 : inherited(name_in,
             &myWriter,   // initialize writer side task
             &myReader,   // initialize reader side task
             refCount_in) // arg passed to task open()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_StreamModule_t::RPG_Stream_StreamModule_t"));

  // set task links to the module...
  // *NOTE*: essential for dereferencing (name-lookups, controlled shutdown, etc)
  myWriter.mod_ = this;
  myReader.mod_ = this;
  //myReader.flags_ |= ACE_Task_Flags::ACE_READER;
}

template <typename TaskSynchType,
          typename ReaderTaskType,
          typename WriterTaskType>
RPG_Stream_StreamModule_t<TaskSynchType,
                          ReaderTaskType,
                          WriterTaskType>::~RPG_Stream_StreamModule_t()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_StreamModule_t::~RPG_Stream_StreamModule_t"));

  // *NOTE*: the base class will invoke close() which will
  // invoke module_close() and flush on every task...
  // *WARNING*: all member tasks will be destroyed by the time that happens...
  // --> close() all modules in advance so it doesn't happen here !!!
}

// ----------------------------------------------------------------------------

template <typename TaskSynchType,
          typename TaskType>
RPG_Stream_StreamModuleInputOnly_t<TaskSynchType,
                                   TaskType>::RPG_Stream_StreamModuleInputOnly_t(const std::string& name_in,
                                                                                 RPG_Stream_IRefCount* refCount_in)
 : inherited(name_in,     // name
             refCount_in) // arg passed to task open()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_StreamModuleInputOnly_t::RPG_Stream_StreamModuleInputOnly_t"));

}

template <typename TaskSynchType,
          typename TaskType>
RPG_Stream_StreamModuleInputOnly_t<TaskSynchType,
                                   TaskType>::~RPG_Stream_StreamModuleInputOnly_t()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_StreamModuleInputOnly_t::~RPG_Stream_StreamModuleInputOnly_t"));

}
