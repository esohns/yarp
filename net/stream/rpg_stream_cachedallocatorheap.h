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

#ifndef RPG_STREAM_CACHEDALLOCATORHEAP_H
#define RPG_STREAM_CACHEDALLOCATORHEAP_H

#include "rpg_stream_exports.h"
#include "rpg_stream_iallocator.h"

#include <ace/Malloc_T.h>
#include <ace/Synch.h>
#include <ace/Atomic_Op.h>

class RPG_Stream_Export RPG_Stream_CachedAllocatorHeap
 : public RPG_Stream_IAllocator,
   public ACE_Dynamic_Cached_Allocator<ACE_SYNCH_MUTEX>
{
 public:
  RPG_Stream_CachedAllocatorHeap(const unsigned long&,  // pool size
                                 const unsigned long&); // chunk size
  virtual ~RPG_Stream_CachedAllocatorHeap();

  // *IMPORTANT NOTE*: need to implement these as ACE_Dynamic_Cached_Allocator
  // doesn't implement them as virtual (BUG)
  inline virtual void* malloc(size_t numBytes_in)
  {
    return inherited::malloc(numBytes_in);
  };
  inline virtual void free(void* pointer_in)
  {
    return inherited::free(pointer_in);
  };

  // *NOTE*: these return the amount of allocated (heap) memory...
  virtual size_t cache_depth() const;
  virtual size_t cache_size() const;

 private:
  typedef ACE_Dynamic_Cached_Allocator<ACE_SYNCH_MUTEX> inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_CachedAllocatorHeap(const RPG_Stream_CachedAllocatorHeap&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_CachedAllocatorHeap& operator=(const RPG_Stream_CachedAllocatorHeap&));

  unsigned long myPoolSize;
};

#endif
