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

#ifndef RPG_STREAM_DATABLOCKALLOCATORHEAP_H
#define RPG_STREAM_DATABLOCKALLOCATORHEAP_H

#include "rpg_stream_iallocator.h"

#include <ace/Malloc_Allocator.h>
#include <ace/Synch.h>
#include <ace/Atomic_Op.h>

// forward declarations
class RPG_Stream_AllocatorHeap;

class RPG_Stream_DataBlockAllocatorHeap
 : public ACE_New_Allocator,
   public RPG_Stream_IAllocator
{
 public:
  RPG_Stream_DataBlockAllocatorHeap(RPG_Stream_AllocatorHeap*); // (heap) memory allocator...
  virtual ~RPG_Stream_DataBlockAllocatorHeap();

  // overload these to do what we want
  // *NOTE*: returns a pointer to ACE_Data_Block...
  virtual void* malloc(size_t); // bytes

  // *NOTE*: returns a pointer to ACE_Data_Block...
  virtual void* calloc(size_t,       // bytes
                       char = '\0'); // initial value (not used)

  // *NOTE*: frees an ACE_Data_Block...
  virtual void free(void*); // element handle

  // *NOTE*: these return the # of online ACE_Data_Blocks...
  virtual size_t cache_depth() const;
  virtual size_t cache_size() const;

  // dump current state
  virtual void dump() const;

  // some convenience typedefs --> save us some typing...
  // *NOTE*: serialize access to ACE_Data_Block reference count which may
  // be decremented from multiple threads...
  typedef ACE_Lock_Adapter<ACE_Thread_Mutex> DATABLOCK_LOCK_TYPE;

  // locking
  // *NOTE*: currently, ALL data blocks use one static lock (OK for stream usage)...
  // *TODO*: consider using a lock-per-message strategy...
  static DATABLOCK_LOCK_TYPE myReferenceCountLock;

 private:
  typedef ACE_New_Allocator inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_DataBlockAllocatorHeap(const RPG_Stream_DataBlockAllocatorHeap&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_DataBlockAllocatorHeap& operator=(const RPG_Stream_DataBlockAllocatorHeap&));

  // these methods are ALL no-ops and will FAIL !
  // *NOTE*: this method is a no-op and just returns NULL
  // since the free list only works with fixed sized entities
  virtual void* calloc(size_t,       // # elements (not used)
                       size_t,       // bytes/element (not used)
                       char = '\0'); // initial value (not used)
  virtual int remove(void);
  virtual int bind(const char*, // name
                   void*,       // pointer
                   int = 0);    // duplicates
  virtual int trybind(const char*, // name
                      void*&);     // pointer
  virtual int find(const char*, // name
                   void*&);     // pointer
  virtual int find(const char*); // name
  virtual int unbind(const char*); // name
  virtual int unbind(const char*, // name
                     void*&);     // pointer
  virtual int sync(ssize_t = -1,   // length
                   int = MS_SYNC); // flags
  virtual int sync(void*,          // address
                   size_t,         // length
                   int = MS_SYNC); // flags
  virtual int protect(ssize_t = -1,     // length
                      int = PROT_RDWR); // protection
  virtual int protect(void*,            // address
                      size_t,           // length
                      int = PROT_RDWR); // protection

  ACE_Atomic_Op<ACE_Thread_Mutex,
                unsigned long>    myPoolSize;

  // heap memory allocator
  RPG_Stream_AllocatorHeap*           myHeapAllocator;
};

#endif
