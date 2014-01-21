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

#include "rpg_common_macros.h"
#include "rpg_common_iclone.h"

template <typename TaskSynchType,
          typename TimePolicyType,
          typename ReaderTaskType,
          typename WriterTaskType>
RPG_Stream_Module_Base_t<TaskSynchType,
                         TimePolicyType,
                         ReaderTaskType,
                         WriterTaskType>::RPG_Stream_Module_Base_t(const std::string& name_in,
                                                                   WriterTaskType* writerTask_in,
                                                                   ReaderTaskType* readerTask_in,
                                                                   RPG_Stream_IRefCount* refCount_in)
 : inherited(ACE_TEXT_CHAR_TO_TCHAR(name_in.c_str()), // name
             writerTask_in,                           // initialize writer side task
             readerTask_in,                           // initialize reader side task
             refCount_in,                             // arg passed to task open()
             inherited::M_DELETE_NONE),               // don't "delete" ANYTHING during close()
   myWriter(writerTask_in),
   myReader(readerTask_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Module_Base_t::RPG_Stream_Module_Base_t"));

  // *WARNING*: apparently, we cannot use "this" at this stage
  // --> children must do this...
//   // set links to ourselves...
//   // *NOTE*: essential to enable dereferencing (name-lookups, controlled shutdown, etc)
//   myWriter->mod_ = this;
//   myReader->mod_ = this;
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename ReaderTaskType,
          typename WriterTaskType>
RPG_Stream_Module_Base_t<TaskSynchType,
                         TimePolicyType,
                         ReaderTaskType,
                         WriterTaskType>::~RPG_Stream_Module_Base_t()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Module_Base_t::~RPG_Stream_Module_Base_t"));

  // *NOTE*: the base class will invoke close() which will
  // invoke module_close() and flush on every task...
  // *WARNING*: all member tasks will be destroyed by the time that happens...
  // --> close() all modules in advance so it doesn't happen here !!!
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename ReaderTaskType,
          typename WriterTaskType>
void
RPG_Stream_Module_Base_t<TaskSynchType,
                         TimePolicyType,
                         ReaderTaskType,
                         WriterTaskType>::reset()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Module_Base_t::reset"));

  // OK: (re-)set our reader and writer tasks...
  inherited::writer(myWriter,
                    inherited::M_DELETE_NONE);

  inherited::reader(myReader,
                    inherited::M_DELETE_NONE);
}

template <typename TaskSynchType,
          typename TimePolicyType,
          typename ReaderTaskType,
          typename WriterTaskType>
ACE_Module<TaskSynchType,
           TimePolicyType>*
RPG_Stream_Module_Base_t<TaskSynchType,
                         TimePolicyType,
                         ReaderTaskType,
                         WriterTaskType>::clone()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Module_Base_t::clone"));

  // init return value(s)
  ACE_Module<TaskSynchType,
             TimePolicyType>* result = NULL;

  // need a downcast...
  typename IMODULE_TYPE::ICLONE_TYPE* iclone_handle =
      dynamic_cast<typename IMODULE_TYPE::ICLONE_TYPE*>(myWriter);
  if (!iclone_handle)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("%s: dynamic_cast<RPG_Common_IClone> failed, aborting\n"),
               ACE_TEXT_ALWAYS_CHAR(inherited::name())));

    return NULL;
  } // end IF

  try
  {
    result = iclone_handle->clone();
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("%s: caught exception in RPG_Common_IClone::clone(), aborting\n"),
               ACE_TEXT_ALWAYS_CHAR(inherited::name())));

    return NULL;
  }
  if (!result)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("%s: failed to RPG_Common_IClone::clone(), aborting\n"),
               ACE_TEXT_ALWAYS_CHAR(inherited::name())));

    return NULL;
  } // end IF

  return result;
}
