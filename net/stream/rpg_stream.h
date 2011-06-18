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

#ifndef RPG_STREAM_H
#define RPG_STREAM_H

#include "rpg_stream_istreamcontrol.h"
#include "rpg_stream_streammodule.h"

#include <rpg_common_idumpstate.h>

#include <ace/Global_Macros.h>
#include <ace/Stream.h>

#include <vector>

// forward declarations
class RPG_Stream_IAllocator;

class RPG_Stream
 : public ACE_Stream<ACE_MT_SYNCH>,
   public RPG_Stream_IStreamControl,
   public RPG_Common_IDumpState
{
 public:
  // define convenient (iterator) types
  typedef RPG_Stream_Module MODULE_TYPE;

  // *NOTE*: this will try to sanely close down the stream:
  // 1: tell all worker threads to exit gracefully
  // 2: close() all modules which have not been enqueued onto the stream (next() == NULL)
  // 3: close() the stream (closes all enqueued modules: wait for queue to flush and threads, if any, to die)
  virtual ~RPG_Stream();

  // overload this from ACE_Stream to work as a hook to pass our messagecounter as argument to the modules
  // open() method...
  //virtual int push(ACE_Module<ACE_MT_SYNCH>*); // handle to module

  // implement RPG_Stream_IStreamControl
  // *NOTE*: delegate these calls to the head module which also implements that API...
  virtual void start();
  virtual void stop();
  virtual const bool isRunning();
  virtual void pause();
  virtual void rewind();
  virtual void waitForCompletion();

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

  const bool isInitialized() const;

 protected:
  // define convenient (iterator) types
  typedef std::vector<MODULE_TYPE*> MODULE_CONTAINER_TYPE;
  typedef std::vector<MODULE_TYPE*>::iterator MODULE_CONTAINERITERATOR_TYPE;

  // *NOTE*: need to subclass this !
  RPG_Stream();

  // *NOTE*: children need to call this PRIOR to module RE-initialization
  // (i.e. in their own init()); this will:
  // - pop/close all push()ed modules, clean up default head/tail modules
  // - reset reader/writer tasks for ALL modules
  // - generate new default head/tail modules
  // *WARNING*: NEVER call this while isRunning() == true, otherwise you'll block
  // until the stream finishes (because close() of a module waits for its worker
  // thread to die...)
  const bool reset();

  // *NOTE*: children MUST call this in their dtor !
  void shutdown();

  // *NOTE*: children need to set this IF their initialization succeeded; otherwise,
  // the dtor will NOT stop all worker threads before close()ing the modules...
  bool                          myIsInitialized;

  // *NOTE*: children need to add handles to ALL of their modules to this container !
  MODULE_CONTAINER_TYPE         myAvailableModules;

  // *NOTE*: children need to set this during THEIR initialization !
  RPG_Stream_IAllocator*            myAllocator;

 private:
  typedef ACE_Stream<ACE_MT_SYNCH> inherited;

  // safety measures
//   ACE_UNIMPLEMENTED_FUNC(Stream());
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream(const RPG_Stream&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream& operator=(const RPG_Stream&));

  // helper methods
  // wrap inherited::open/close() calls
  const bool init();
  const bool fini();
  void deactivateModules();
};

#endif
