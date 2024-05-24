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

#ifndef RPG_NET_PACKET_HEADERS_H
#define RPG_NET_PACKET_HEADERS_H

#include "ace/OS.h"

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif

// *IMPORTANT NOTE*: this header doesn't exist on the Windows platform...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
#include <netinet/tcp.h>
#else
struct tcphdr
{
  u_short source;  // Source port
  u_short dest;    // Destination port
  u_long  seq;     // Sequence number
  u_long  ack_seq; // Acknowledged sequence number
#if defined (ACE_LITTLE_ENDIAN)
  u_short res1:4,
          doff:4,
          fin:1,
          syn:1,
          rst:1,
          psh:1,
          ack:1,
          urg:1,
          ece:1,
          cwr:1;
#elif defined (ACE_BIG_ENDIAN)
  u_short doff:4,
          res1:4,
          cwr:1,
          ece:1,
          urg:1,
          ack:1,
          psh:1,
          rst:1,
          syn:1,
          fin:1;
#else
#error "ACE_BYTE_ORDER not defined"
#endif
  u_short window;  // Window
  u_short check;   // Checksum
  u_short urg_ptr; // Urgent pointer
#ifdef __GNUC__
} __attribute__ ((__packed__));
#else
};
#endif
#endif

// *IMPORTANT NOTE*: this header doesn't exist on the Windows platform...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
#include <netinet/udp.h>
#else
struct udphdr
{
  u_short source; // Source port
  u_short dest;   // Destination port
  u_short len;    // Datagram length
  u_short check;  // Checksum
#ifdef __GNUC__
} __attribute__ ((__packed__));
#else
};
#endif
#endif

// *IMPORTANT NOTE*: this header doesn't exist on the Windows platform...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
#include <netinet/ip6.h>
#else
#include <in6addr.h>
//struct in6_addr
//{
//  union
//  {
//    uint8_t u6_addr8[16];
//    uint16_t u6_addr16[8];
//    uint32_t u6_addr32[4];
//  } in6_u;
//#define s6_addr     in6_u.u6_addr8
//#define s6_addr16   in6_u.u6_addr16
//#define s6_addr32   in6_u.u6_addr32
//#ifdef __GNUC__
//} __attribute__ ((__packed__));
//#else
//};
//#endif
//struct ip6_hdr
//{
//  union
//  {
//    struct ip6_hdrctl
//    {
//      uint32_t ip6_un1_flow;   /* 4 bits version, 8 bits TC, 20 bits flow-ID */
//      uint16_t ip6_un1_plen;   /* payload length */
//      uint8_t  ip6_un1_nxt;    /* next header */
//      uint8_t  ip6_un1_hlim;   /* hop limit */
//    } ip6_un1;
//    uint8_t ip6_un2_vfc;       /* 4 bits version, top 4 bits tclass */
//  } ip6_ctlun;
//  struct in6_addr ip6_src;     /* source address */
//  struct in6_addr ip6_dst;     /* destination address */
//#ifdef __GNUC__
//} __attribute__ ((__packed__));
//#else
//};
//#endif
#endif

// *IMPORTANT NOTE*: this header doesn't exist on the Windows platform...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
#include <netinet/ip.h>
#else
struct iphdr
{
#if defined (ACE_LITTLE_ENDIAN)
  u_char  ihl:4,          // Version (4 bits) + Internet header length (4 bits)
          version:4;
#elif defined (ACE_BIG_ENDIAN)
  u_char  version:4,      // Version (4 bits) + Internet header length (4 bits)
          ihl:4;
#else
#error ACE_BYTE_ORDER not defined
#endif
  u_char  tos;            // Type of service
  u_short tot_len;        // Total length
  u_short id;             // Identification
// *IMPORTANT NOTE*: until i can find a better way to handle this, just
// use the MACROS...
//  u_short flags:3,        // Flags (3 bits) + Fragment offset (13 bits)
//          offset:13;
  u_short frag_off;       /* fragment flags/offset field */
#define IP_RF 0x8000      /* reserved fragment flag */
#define IP_DF 0x4000      /* dont fragment flag */
#define IP_MF 0x2000      /* more fragments flag */
#define IP_OFFMASK 0x1fff /* mask for fragmenting bits */
  u_char  ttl;            // Time to live
  u_char  protocol;       // Protocol
  u_short check;          // Header checksum
  u_long  saddr;          // Source address
  u_long  daddr;          // Destination address
// *IMPORTANT NOTE*: this field is optional (i.e. it only exists if ihl > 5 !
//  u_int   op_pad;       // Option + Padding
#ifdef __GNUC__
} __attribute__ ((__packed__));
#else
};
#endif
#endif

// *IMPORTANT NOTE*: this header doesn't exist on the Windows platform...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
#include <netinet/in.h>
#else
// these protocols seem to be missing in winsock2.h...
#define IPPROTO_IPIP  IPPROTO_IPV4 /* IPIP tunnels (older KA9Q tunnels use 94) */
#define IPPROTO_EGP   8            /* Exterior Gateway Protocol                */
#define IPPROTO_TP    29           /* SO Transport Protocol Class 4            */
#define IPPROTO_RSVP  46           /* Reservation Protocol                     */
#define IPPROTO_GRE   47           /* General Routing Encapsulation            */
#define IPPROTO_MTP   92           /* Multicast Transport Protocol             */
#define IPPROTO_ENCAP 98           /* Encapsulation Header                     */
#define IPPROTO_PIM   103          /* Protocol Independent Multicast           */
#define IPPROTO_COMP  108          /* Compression Header Protocol              */
#define IPPROTO_SCTP  132          /* Stream Control Transmission Protocol     */
#endif

// *IMPORTANT NOTE*: this header doesn't exist on the Windows platform...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
#include <net/ethernet.h>
#else
 /*
  *      IEEE 802.3 Ethernet magic constants.  The frame sizes omit the preamble
  *      and FCS/CRC (frame check sequence).
  */
#define ETH_ALEN        6               /* Octets in one ethernet addr   */
#define ETH_HLEN        14              /* Total octets in header.       */
#define ETH_ZLEN        60              /* Min. octets in frame sans FCS */
#define ETH_DATA_LEN    1500            /* Max. octets in payload        */
#define ETH_FRAME_LEN   1514            /* Max. octets in frame sans FCS */

// *INFORMATION*
//Ethernet Frame Formats
//
//  A. Ethernet II
//
//    +----+----+------+------+-----+
//    | DA | SA | Type | Data | FCS |
//    +----+----+------+------+-----+
//
//    DA      Destination MAC Address (6 bytes)
//    SA      Source MAC Address      (6 bytes)
//    Type    Protocol Type           (2 bytes)
//    Data    Protocol Data           (46 - 1500 bytes)
//    FCS     Frame Checksum          (4 bytes)
//
//  B. IEEE 802.3 and derivatives
//
//    +----+----+------+------+-----+
//    | DA | SA | Len  | Data | FCS |
//    +----+----+------+------+-----+
//
//    DA      Destination MAC Address (6 bytes)
//    SA      Source MAC Address      (6 bytes)
//    Len     Length of Data field    (2 bytes)
//    Data    Protocol Data           (46 - 1500 bytes)
//    FCS     Frame Checksum          (4 bytes)
/*
 *      This is an Ethernet frame header.
 */
struct ether_header
{
  u_char   h_dest[ETH_ALEN];   /* destination eth addr */
  u_char   h_source[ETH_ALEN]; /* source ether addr    */
  u_short  ether_type;         /* packet type ID field */
#ifdef __GNUC__
} __attribute__ ((__packed__));
#else
};
#endif
#endif

/*
 *      These are the defined Ethernet Protocol ID's.
 */
#ifndef ETH_P_LOOP
#define ETH_P_LOOP            0x0060    /* Ethernet Loopback packet     */
#endif
#ifndef ETHERTYPE_GRE_ISO
#define ETHERTYPE_GRE_ISO     0x00FE    /* not really an ethertype only used in GRE */
#endif
#ifndef ETH_P_PUP
#define ETH_P_PUP             0x0200    /* Xerox PUP packet             */
#endif
#ifndef ETH_P_PUPAT
#define ETH_P_PUPAT           0x0201    /* Xerox PUP Addr Trans packet  */
#endif
#ifndef ETHERTYPE_SPRITE
#define ETHERTYPE_SPRITE      0x0500    // ???
#endif
#ifndef ETH_P_IP
#define ETH_P_IP              0x0800    /* Internet Protocol packet     */
#endif
#ifndef ETH_P_X25
#define ETH_P_X25             0x0805    /* CCITT X.25                   */
#endif
#ifndef ETH_P_ARP
#define ETH_P_ARP             0x0806    /* Address Resolution packet    */
#endif
#ifndef ETH_P_BPQ
#define ETH_P_BPQ             0x08FF    /* G8BPQ AX.25 Ethernet Packet  [ NOT AN OFFICIALLY REGISTERED ID ] */
#endif
#ifndef ETH_P_IEEEPUP
#define ETH_P_IEEEPUP         0x0a00    /* Xerox IEEE802.3 PUP packet */
#endif
#ifndef ETH_P_IEEEPUPAT
#define ETH_P_IEEEPUPAT       0x0a01    /* Xerox IEEE802.3 PUP Addr Trans packet */
#endif
#ifndef ETHERTYPE_NS
#define ETHERTYPE_NS          0x0600    // ???
#endif
#ifndef ETHERTYPE_TRAIL
#define ETHERTYPE_TRAIL       0x1000    // ???
#endif
#ifndef ETH_P_DEC
#define ETH_P_DEC             0x6000    /* DEC Assigned proto           */
#endif
#ifndef ETH_P_DNA_DL
#define ETH_P_DNA_DL          0x6001    /* DEC DNA Dump/Load            */
#endif
#ifndef ETH_P_DNA_RC
#define ETH_P_DNA_RC          0x6002    /* DEC DNA Remote Console       */
#endif
#ifndef ETH_P_DNA_RT
#define ETH_P_DNA_RT          0x6003    /* DEC DNA Routing              */
#endif
#ifndef ETH_P_LAT
#define ETH_P_LAT             0x6004    /* DEC LAT                      */
#endif
#ifndef ETH_P_DIAG
#define ETH_P_DIAG            0x6005    /* DEC Diagnostics              */
#endif
#ifndef ETH_P_CUST
#define ETH_P_CUST            0x6006    /* DEC Customer use             */
#endif
#ifndef ETH_P_SCA
#define ETH_P_SCA             0x6007    /* DEC Systems Comms Arch       */
#endif
#ifndef ETH_P_RARP
#define ETH_P_RARP            0x8035    /* Reverse Addr Res packet      */
#endif
#ifndef ETHERTYPE_LANBRIDGE
#define ETHERTYPE_LANBRIDGE   0x8038
#endif
#ifndef ETHERTYPE_DECDNS
#define ETHERTYPE_DECDNS      0x803c
#endif
#ifndef ETHERTYPE_DECDTS
#define ETHERTYPE_DECDTS      0x803e
#endif
#ifndef ETHERTYPE_VEXP
#define ETHERTYPE_VEXP        0x805b
#endif
#ifndef ETHERTYPE_VPROD
#define ETHERTYPE_VPROD       0x805c
#endif
#ifndef ETH_P_ATALK
#define ETH_P_ATALK           0x809B    /* Appletalk DDP                */
#endif
#ifndef ETH_P_AARP
#define ETH_P_AARP            0x80F3    /* Appletalk AARP               */
#endif
#ifndef ETH_P_8021Q
#define ETH_P_8021Q           0x8100    /* 802.1Q VLAN Extended Header  */
#endif
#ifndef ETH_P_IPX
#define ETH_P_IPX             0x8137    /* IPX over DIX                 */
#endif
#ifndef ETH_P_IPV6
#define ETH_P_IPV6            0x86DD    /* IPv6 over bluebook           */
#endif
#ifndef ETHERTYPE_MPCP
#define ETHERTYPE_MPCP        0x8808
#endif
#ifndef ETHERTYPE_SLOW
#define ETHERTYPE_SLOW        0x8809
#endif
#ifndef ETHERTYPE_PPP
#define ETHERTYPE_PPP         0x880b
#endif
#ifndef ETH_P_WCCP
#define ETH_P_WCCP            0x883E    /* Web-cache coordination protocol
                                         * defined in draft-wilson-wrec-wccp-v2-00.txt */
#endif
#ifndef ETH_P_MPLS_UC
#define ETH_P_MPLS_UC         0x8847    /* MPLS Unicast traffic         */
#endif
#ifndef ETH_P_MPLS_MC
#define ETH_P_MPLS_MC         0x8848    /* MPLS Multicast traffic       */
#endif
#ifndef ETH_P_ATMMPOA
#define ETH_P_ATMMPOA         0x884c    /* MultiProtocol Over ATM       */
#endif
#ifndef ETH_P_PPP_DISC
#define ETH_P_PPP_DISC        0x8863    /* PPPoE discovery messages     */
#endif
#ifndef ETH_P_PPP_SES
#define ETH_P_PPP_SES         0x8864    /* PPPoE session messages       */
#endif
#ifndef ETHERTYPE_JUMBO
#define ETHERTYPE_JUMBO       0x8870
#endif
#ifndef ETH_P_ATMFATE
#define ETH_P_ATMFATE         0x8884    /* Frame-based ATM Transport
                                         * over Ethernet */
#endif
#ifndef ETHERTYPE_EAPOL
#define ETHERTYPE_EAPOL       0x888e
#endif
#ifndef ETH_P_AOE
#define ETH_P_AOE             0x88A2    /* ATA over Ethernet            */
#endif
#ifndef ETH_P_TIPC
#define ETH_P_TIPC            0x88CA    /* TIPC                         */
#endif
#ifndef ETHERTYPE_LOOPBACK
#define ETHERTYPE_LOOPBACK    0x9000
#endif
#ifndef ETHERTYPE_VMAN
#define ETHERTYPE_VMAN        0x9100    /* Extreme VMAN Protocol */
#endif
#ifndef ETHERTYPE_ISO
#define ETHERTYPE_ISO         0xfefe    /* nonstandard - used in Cisco HDLC encapsulation */
#endif

/*
 *      Non DIX types. Won't clash for 1500 types.
 */
#ifndef ETH_P_802_3
#define ETH_P_802_3     0x0001          /* Dummy type for 802.3 frames  */
#endif
#ifndef ETH_P_AX25
#define ETH_P_AX25      0x0002          /* Dummy protocol id for AX.25  */
#endif
#ifndef ETH_P_ALL
#define ETH_P_ALL       0x0003          /* Every packet (be careful!!!) */
#endif
#ifndef ETH_P_802_2
#define ETH_P_802_2     0x0004          /* 802.2 frames                 */
#endif
#ifndef ETH_P_SNAP
#define ETH_P_SNAP      0x0005          /* Internal only                */
#endif
#ifndef ETH_P_DDCMP
#define ETH_P_DDCMP     0x0006          /* DEC DDCMP: Internal only     */
#endif
#ifndef ETH_P_WAN_PPP
#define ETH_P_WAN_PPP   0x0007          /* Dummy type for WAN PPP frames*/
#endif
#ifndef ETH_P_PPP_MP
#define ETH_P_PPP_MP    0x0008          /* Dummy type for PPP MP frames */
#endif
#ifndef ETH_P_LOCALTALK
#define ETH_P_LOCALTALK 0x0009          /* Localtalk pseudo type        */
#endif
#ifndef ETH_P_PPPTALK
#define ETH_P_PPPTALK   0x0010          /* Dummy type for Atalk over PPP*/
#endif
#ifndef ETH_P_TR_802_2
#define ETH_P_TR_802_2  0x0011          /* 802.2 frames                 */
#endif
#ifndef ETH_P_MOBITEX
#define ETH_P_MOBITEX   0x0015          /* Mobitex (kaz@cafe.net)       */
#endif
#ifndef ETH_P_CONTROL
#define ETH_P_CONTROL   0x0016          /* Card specific control frames */
#endif
#ifndef ETH_P_IRDA
#define ETH_P_IRDA      0x0017          /* Linux-IrDA                   */
#endif
#ifndef ETH_P_ECONET
#define ETH_P_ECONET    0x0018          /* Acorn Econet                 */
#endif
#ifndef ETH_P_HDLC
#define ETH_P_HDLC      0x0019          /* HDLC frames                  */
#endif
#ifndef ETH_P_ARCNET
#define ETH_P_ARCNET    0x001A          /* 1A for ArcNet :-)            */
#endif

// *IMPORTANT NOTE*: this header doesn't exist on the Windows platform...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
#include <netinet/if_fddi.h>
#else
// *TODO*: FDDI LLC/SNAP/...
struct fddi_8022_1_hdr
{
  u_char dsap; /* destination service access point */
  u_char ssap; /* source service access point */
  u_char ctrl; /* control byte #1 */
#ifdef __GNUC__
} __attribute__ ((__packed__));
#else
};
#endif

/* Define 802.2 Type 2 header */
struct fddi_8022_2_hdr
{
  u_char dsap;   /* destination service access point */
  u_char ssap;   /* source service access point */
  u_char ctrl_1; /* control byte #1 */
  u_char ctrl_2; /* control byte #2 */
#ifdef __GNUC__
} __attribute__ ((__packed__));
#else
};
#endif

/* Define 802.2 SNAP header */
#define FDDI_K_OUI_LEN	 3
struct fddi_snap_hdr
{
  u_char  dsap;                /* always 0xAA */
  u_char  ssap;					       /* always 0xAA */
  u_char  ctrl;					       /* always 0x03 */
  u_char  oui[FDDI_K_OUI_LEN]; /* organizational universal id */
  u_short	ethertype;				   /* packet type ID field */
#ifdef __GNUC__
} __attribute__ ((__packed__));
#else
};
#endif

/* Define FDDI LLC frame header */
#define FDDI_K_ALEN      6               /* Octets in one fddi addr      */
struct fddi_header
{
  u_char fddi_fc;                 /* Frame Control (FC) value */
  u_char fddi_dhost[FDDI_K_ALEN]; /* Destination host */
  u_char fddi_shost[FDDI_K_ALEN]; /* Source host */
  union
  {
    struct fddi_8022_1_hdr llc_8022_1;
    struct fddi_8022_2_hdr llc_8022_2;
    struct fddi_snap_hdr   llc_snap;
  } hdr;
#ifdef __GNUC__
} __attribute__ ((__packed__));
#else
};
#endif

#ifdef _MSC_VER
#pragma pack(pop)
#endif

#define FDDI_K_SNAP_HLEN 21             /* Total octets in header.       */
#endif
#endif
