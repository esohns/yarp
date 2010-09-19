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

template <typename WriterTaskType,
          typename ReaderTaskType>
RPG_Stream_StreamModuleBase<WriterTaskType,
                        ReaderTaskType>::RPG_Stream_StreamModuleBase(const std::string& name_in,
                                                                 RPG_Stream_IRefCount* refCount_in)
  : inherited(name_in,     // name
              &myWriter,   // initialize writer side task
              &myReader,   // initialize reader side task
              refCount_in) // arg passed to task open()
{
  ACE_TRACE(ACE_TEXT("RPG_Stream_StreamModuleBase::RPG_Stream_StreamModuleBase"));

  // set links to ourselves...
  // *NOTE*: essential to enable dereferencing (name-lookups, controlled shutdown, etc)
  myWriter.mod_ = this;
  myReader.mod_ = this;
}

template <typename WriterTaskType,
          typename ReaderTaskType>
RPG_Stream_StreamModuleBase<WriterTaskType,
                        ReaderTaskType>::~RPG_Stream_StreamModuleBase()
{
  ACE_TRACE(ACE_TEXT("RPG_Stream_StreamModuleBase::~RPG_Stream_StreamModuleBase"));

  // *NOTE*: the base class will invoke close() which will
  // invoke module_close() and flush on every task...
  // *WARNING*: all member tasks will be destroyed by the time that happens...
  // --> close() all modules in advance so it doesn't happen here !!!
}
