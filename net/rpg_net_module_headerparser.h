//%%%FILE%%%////////////////////////////////////////////////////////////////////
// File Name: rps_flb_common_module_headerparser.h
//
// History:
//   Date   |Name | Description of modification
// ---------|-----|-------------------------------------------------------------
// 20/02/06 | soh | Creation.
//%%%FILE%%%////////////////////////////////////////////////////////////////////

#ifndef RPS_FLB_COMMON_MODULE_HEADERPARSER_H
#define RPS_FLB_COMMON_MODULE_HEADERPARSER_H

#include "rps_flb_common_taskbaseasynch.h"
#include "rps_flb_common_streammodulebase.h"
#include "rps_flb_common_networkmessage.h"

#include <map>
#include <set>

// forward declaration(s)
class ACE_Message_Block;

class RPS_FLB_Common_Module_HeaderParser
  : public RPS_FLB_Common_TaskBaseAsynch
{
 public:
  RPS_FLB_Common_Module_HeaderParser();
  virtual ~RPS_FLB_Common_Module_HeaderParser();

  // initialization
  // *IMPORTANT NOTE*: users need to pass in the type of link layer (currently, we support
  // ethernet and fddi only)
  const bool init(const bool&); // link layer is FDDI ?

  // implement (part of) RPS_FLB_Common_IModuleTaskBase
  virtual void handleDataMessage(RPS_FLB_Common_NetworkMessage*&, // data message handle
                                  bool&);                         // return value: pass message downstream ?

  // implement RPS_FLB_Common_IDumpState
  virtual void dump_state() const;

 private:
  typedef RPS_FLB_Common_TaskBaseAsynch inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPS_FLB_Common_Module_HeaderParser(const RPS_FLB_Common_Module_HeaderParser&));
  ACE_UNIMPLEMENTED_FUNC(RPS_FLB_Common_Module_HeaderParser& operator=(const RPS_FLB_Common_Module_HeaderParser&));

  // debug info
  // some convenience typedefs --> save us some typing...
  typedef std::set<unsigned short> PACKETTYPECONTAINER_TYPE;
  typedef PACKETTYPECONTAINER_TYPE::const_iterator PACKETTYPECONTAINER_CONSTITERATOR_TYPE;
  typedef std::multimap<unsigned short,
                        RPS_FLB_Common_MessageID_Type> PACKETTYPE2MESSAGEIDMAP_TYPE;
  typedef std::pair<unsigned short,
                    RPS_FLB_Common_MessageID_Type> PACKETTYPE2MESSAGEIDPAIR_TYPE;
  typedef PACKETTYPE2MESSAGEIDMAP_TYPE::const_iterator PACKETTYPE2MESSAGEIDMAP_CONSTITERATOR_TYPE;

  mutable PACKETTYPE2MESSAGEIDMAP_TYPE      myUnknownPacketTypes2MessageID; // network byte order !
  mutable RPS_FLB_Common_MessageIDList_Type myIEEE802_3Frames;
  mutable RPS_FLB_Common_MessageIDList_Type myFailedMessages; // we failed to decode these...

  bool                                      myIsInitialized;
  bool                                      myIsLinkLayerFDDI;
};

// declare module
DATASTREAM_MODULE(RPS_FLB_Common_Module_HeaderParser);

#endif
