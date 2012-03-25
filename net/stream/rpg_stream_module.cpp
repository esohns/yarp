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

#include "rpg_stream_module.h"

#include <rpg_common_macros.h>

RPG_Stream_Module::RPG_Stream_Module(const std::string& name_in,
                             TASK_TYPE* writerTask_in,
                             TASK_TYPE* readerTask_in,
                             RPG_Stream_IRefCount* refCount_in)
 : inherited(ACE_TEXT_CHAR_TO_TCHAR(name_in.c_str()),
             writerTask_in,             // initialize writer side task
             readerTask_in,             // initialize reader side task
             refCount_in,               // arg passed to task open()
             inherited::M_DELETE_NONE), // don't "delete" ANYTHING during close()
   myWriter(writerTask_in),
   myReader(readerTask_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Module::RPG_Stream_Module"));

  // *WARNING*: apparently, we cannot use "this" at this stage
  // --> children must do this...
//   // set links to ourselves...
//   // *NOTE*: essential to enable dereferencing (name-lookups, controlled shutdown, etc)
//   myWriter->mod_ = this;
//   myReader->mod_ = this;
}

RPG_Stream_Module::~RPG_Stream_Module()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Module::~RPG_Stream_Module"));

  // *NOTE*: the base class will invoke close() which will
  // invoke module_close() and flush on every task...
  // *WARNING*: all member tasks will be destroyed by the time that happens...
  // --> close() all modules in advance so it doesn't happen here !!!
}

void
RPG_Stream_Module::resetReaderWriter()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_Module::resetReaderWriter"));

  // OK: (re-)set our reader and writer tasks...
  inherited::writer(myWriter,
                    inherited::M_DELETE_NONE);

  inherited::reader(myReader,
                    inherited::M_DELETE_NONE);
}
