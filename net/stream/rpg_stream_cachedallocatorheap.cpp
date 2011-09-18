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

#include "rpg_stream_cachedallocatorheap.h"

#include <rpg_common_macros.h>

RPG_Stream_CachedAllocatorHeap::RPG_Stream_CachedAllocatorHeap(const unsigned long& poolSize_in,
                                                               const unsigned long& chunkSize_in)
 : inherited(poolSize_in, chunkSize_in),
   myPoolSize(poolSize_in * chunkSize_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedAllocatorHeap::RPG_Stream_CachedAllocatorHeap"));

}

RPG_Stream_CachedAllocatorHeap::~RPG_Stream_CachedAllocatorHeap()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedAllocatorHeap::~RPG_Stream_CachedAllocatorHeap"));

}

size_t
RPG_Stream_CachedAllocatorHeap::cache_depth() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedAllocatorHeap::cache_depth"));

  return const_cast<RPG_Stream_CachedAllocatorHeap*>(this)->pool_depth();
}

size_t
RPG_Stream_CachedAllocatorHeap::cache_size() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_CachedAllocatorHeap::cache_size"));

  return myPoolSize;
}
