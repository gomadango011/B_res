/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 IITP RAS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Based on
 *      NS-2 AODV model developed by the CMU/MONARCH group and optimized and
 *      tuned by Samir Das and Mahesh Marina, University of Cincinnati;
 *
 *      AODV-UU implementation by Erik Nordström of Uppsala University
 *      http://core.it.uu.se/core/index.php/AODV-UU
 *
 * Authors: Elena Buchatskaia <borovkovaes@iitp.ru>
 *          Pavel Boyko <boyko@iitp.ru>m_reserved
 */
#include "aodv-packet.h"
#include "ns3/address-utils.h"
#include "ns3/packet.h"

namespace ns3 {
namespace aodv {

NS_OBJECT_ENSURE_REGISTERED (TypeHeader);

TypeHeader::TypeHeader (MessageType t)
  : m_type (t),
    m_valid (true)
{
}

TypeId
TypeHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::aodv::TypeHeader")
    .SetParent<Header> ()
    .SetGroupName ("Aodv")
    .AddConstructor<TypeHeader> ()
  ;
  return tid;
}

TypeId
TypeHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
TypeHeader::GetSerializedSize () const
{
  return 1;
}

void
TypeHeader::Serialize (Buffer::Iterator i) const
{
  i.WriteU8 ((uint8_t) m_type);
}

uint32_t
TypeHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  uint8_t type = i.ReadU8 ();
  m_valid = true;
  switch (type)
    {
    case AODVTYPE_RREQ:
    case AODVTYPE_RREP:
    case AODVTYPE_RERR:
    case AODVTYPE_RREP_ACK:
    case AODVTYPE_WHCS:
    case AODVTYPE_WHCE:
      {
        m_type = (MessageType) type;
        break;
      }
    default:
      m_valid = false;
    }
  uint32_t dist = i.GetDistanceFrom (start);
  NS_ASSERT (dist == GetSerializedSize ());
  return dist;
}

void
TypeHeader::Print (std::ostream &os) const
{
  switch (m_type)
    {
    case AODVTYPE_RREQ:
      {
        os << "RREQ";
        break;
      }
    case AODVTYPE_RREP:
      {
        os << "RREP";
        break;
      }
    case AODVTYPE_RERR:
      {
        os << "RERR";
        break;
      }
    case AODVTYPE_RREP_ACK:
      {
        os << "RREP_ACK";
        break;
      }
      case AODVTYPE_WHCS:
      {
        os << "WHCS";
        break;
      }
      case AODVTYPE_WHCE:
      {
        os << "WHCE";
        break;
      }
    default:
      os << "UNKNOWN_TYPE";
    }
}

bool
TypeHeader::operator== (TypeHeader const & o) const
{
  return (m_type == o.m_type && m_valid == o.m_valid);
}

std::ostream &
operator<< (std::ostream & os, TypeHeader const & h)
{
  h.Print (os);
  return os;
}

//-----------------------------------------------------------------------------
// RREQ
//-----------------------------------------------------------------------------
RreqHeader::RreqHeader (uint8_t flags, uint8_t reserved, uint8_t hopCount, uint32_t requestID, Ipv4Address dst,
                        uint32_t dstSeqNo, Ipv4Address origin, uint32_t originSeqNo, Ipv4Address second)
  : m_flags (flags),
    m_reserved (reserved),
    m_hopCount (hopCount),
    m_requestID (requestID),
    m_dst (dst),
    m_dstSeqNo (dstSeqNo),
    m_origin (origin),
    m_originSeqNo (originSeqNo),
    m_second (second)
{
}

NS_OBJECT_ENSURE_REGISTERED (RreqHeader);

TypeId
RreqHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::aodv::RreqHeader")
    .SetParent<Header> ()
    .SetGroupName ("Aodv")
    .AddConstructor<RreqHeader> ()
  ;
  return tid;
}

TypeId
RreqHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
RreqHeader::GetSerializedSize () const
{
 return 32;
 //return 23;
}

void
RreqHeader::Serialize (Buffer::Iterator i) const
{
  i.WriteU8 (m_flags);
  i.WriteU8 (m_reserved);
  i.WriteU8 (m_hopCount);
  i.WriteHtonU32 (m_requestID);
  WriteTo (i, m_dst);
  i.WriteHtonU32 (m_dstSeqNo);
  WriteTo (i, m_origin);
  i.WriteHtonU32 (m_originSeqNo);
  WriteTo (i, m_second);
}

uint32_t
RreqHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_flags = i.ReadU8 ();
  m_reserved = i.ReadU8 ();
  m_hopCount = i.ReadU8 ();
  m_requestID = i.ReadNtohU32 ();
  ReadFrom (i, m_dst);
  m_dstSeqNo = i.ReadNtohU32 ();
  ReadFrom (i, m_origin);
  m_originSeqNo = i.ReadNtohU32 ();
  ReadFrom (i, m_second);

  uint32_t dist = i.GetDistanceFrom (start);
  //GetSerializedSize() = 31
  //NS_ASSERT (dist == /*GetSerializedSize() = */31);
  return dist;
}

void
RreqHeader::Print (std::ostream &os) const
{
  os << "RREQ ID " << m_requestID << " destination: ipv4 " << m_dst
     << " sequence number " << m_dstSeqNo << " source: ipv4 "
     << m_origin << " sequence number " << m_originSeqNo << " second node: ipv4 "<< m_second
     << " flags:" << " Gratuitous RREP " << (*this).GetGratuitousRrep ()
     << " Destination only " << (*this).GetDestinationOnly ()
     << " Unknown sequence number " << (*this).GetUnknownSeqno ();
}

std::ostream &
operator<< (std::ostream & os, RreqHeader const & h)
{
  h.Print (os);
  return os;
}

//Set the gratuitous RREP flag
void
RreqHeader::SetGratuitousRrep (bool f)
{
  if (f)
    {
      m_flags |= (1 << 5);
    }
  else
    {
      m_flags &= ~(1 << 5);
    }
}

bool
RreqHeader::GetGratuitousRrep () const
{
  return (m_flags & (1 << 5));
}

void
RreqHeader::SetDestinationOnly (bool f)
{
  if (f)
    {
      m_flags |= (1 << 4);
    }
  else
    {
      m_flags &= ~(1 << 4);
    }
}

bool
RreqHeader::GetDestinationOnly () const
{
  return (m_flags & (1 << 4));
}

void
RreqHeader::SetUnknownSeqno (bool f)
{
  if (f)
    {
      m_flags |= (1 << 3);
    }
  else
    {
      m_flags &= ~(1 << 3);
    }
}

bool
RreqHeader::GetUnknownSeqno () const
{
  return (m_flags & (1 << 3));
}

bool
RreqHeader::operator== (RreqHeader const & o) const
{
  return (m_flags == o.m_flags && m_reserved == o.m_reserved
          && m_hopCount == o.m_hopCount && m_requestID == o.m_requestID
          && m_dst == o.m_dst && m_dstSeqNo == o.m_dstSeqNo
          && m_origin == o.m_origin && m_originSeqNo == o.m_originSeqNo
          && m_second == o.m_second);
}


//-----------------------------------------------------------------------------
// WHCS
//-----------------------------------------------------------------------------

WHCheckHeader::WHCheckHeader (uint8_t flags, uint8_t reserved, uint8_t hopCount, uint32_t WHcheckID, Ipv4Address fir,
                        uint32_t firSeqNo, Ipv4Address origin, uint32_t originSeqNo, Ipv4Address sec, Ipv4Address src,
                        Ipv4Address dst, uint8_t whf, uint8_t rrepid, uint8_t WHflag, uint32_t rreq_id)
  : m_flags (flags),
    m_reserved (reserved),
    m_hopCount (hopCount),
    m_WHCheckID (WHcheckID),
    m_fir (fir),
    m_firSeqNo (firSeqNo),
    m_origin (origin),
    m_originSeqNo (originSeqNo),
    m_sec (sec),
    m_src (src),
    m_dst (dst),
    m_whf (whf),
    m_rrepid (rrepid),
    //WH攻撃を行ったかわかる用のフラグ
    m_WHflag (WHflag),
    m_rreq_id (rreq_id)
{
}

NS_OBJECT_ENSURE_REGISTERED (WHCheckHeader);

TypeId
WHCheckHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::aodv::WHCheckHeader")
    .SetParent<Header> ()
    .SetGroupName ("Aodv")
    .AddConstructor<WHCheckHeader> ()
  ;
  return tid;
}

TypeId
WHCheckHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
WHCheckHeader::GetSerializedSize () const
{
  //38+1(WH攻撃を行ったかわかる用のフラグ)
  //return 38;
  return 39 + 4/*RREQのID分*/;
//  return 37;
 //return 23;
}

void
WHCheckHeader::Serialize (Buffer::Iterator i) const
{
  i.WriteU8 (m_flags);
  i.WriteU8 (m_reserved);
  i.WriteU8 (m_hopCount);
  i.WriteHtonU32 (m_WHCheckID);
  WriteTo (i, m_fir);
  i.WriteHtonU32 (m_firSeqNo);
  WriteTo (i, m_origin);
  i.WriteHtonU32 (m_originSeqNo);
  WriteTo (i, m_sec);
  WriteTo (i, m_src);
  WriteTo (i, m_dst);
  i.WriteU8 (m_whf);
  i.WriteU8 (m_rrepid);
  //WH攻撃を行ったかわかる用のフラグ
  i.WriteU8 (m_WHflag);
  i.WriteHtonU32 (m_rreq_id);
}

uint32_t
WHCheckHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_flags = i.ReadU8 ();
  m_reserved = i.ReadU8 ();
  m_hopCount = i.ReadU8 ();
  m_WHCheckID = i.ReadNtohU32 ();
  ReadFrom (i, m_fir);
  m_firSeqNo = i.ReadNtohU32 ();
  ReadFrom (i, m_origin);
  m_originSeqNo = i.ReadNtohU32 ();
  ReadFrom (i, m_sec);
  ReadFrom (i, m_src);
  ReadFrom (i, m_dst);
  m_whf = i.ReadU8 ();
  m_rrepid = i.ReadU8 ();

  //WH攻撃を行ったかわかる用のフラグ
  m_WHflag = i.ReadU8 ();
  //RREQのID
  m_rreq_id = i.ReadNtohU32 ();

  uint32_t dist = i.GetDistanceFrom (start);

  //GetSerializedSize() = 31
  //NS_ASSERT (dist == /*GetSerializedSize() = */31);
  return dist;
}

void
WHCheckHeader::Print (std::ostream &os) const
{
  os << "RREQ ID " << m_WHCheckID << " destination: ipv4 " << m_fir
     << " sequence number " << m_firSeqNo << " source: ipv4 "
     << m_origin << " sequence number " << m_originSeqNo << " second node: ipv4 "<< m_sec << " src node: ipv4 "<< m_src
     <<" dst : "<< m_dst
     << " flags:" << " Gratuitous RREP " << (*this).GetGratuitousWHCE ()
     << " Destination only " << (*this).GetDestinationOnly ()
     << " Unknown sequence number " << (*this).GetUnknownSeqno ();
}

std::ostream &
operator<< (std::ostream & os, WHCheckHeader const & h)
{
  h.Print (os);
  return os;
}

//無償RREPフラグを設定する
void
WHCheckHeader::SetGratuitousWHCE (bool f)
{
  if (f)
    {
      m_flags |= (1 << 5);
    }
  else
    {
      m_flags &= ~(1 << 5);
    }
}

bool
WHCheckHeader::GetGratuitousWHCE () const
{
  return (m_flags & (1 << 5));
}

void
WHCheckHeader::SetDestinationOnly (bool f)
{
  if (f)
    {
      m_flags |= (1 << 4);
    }
  else
    {
      m_flags &= ~(1 << 4);
    }
}

bool
WHCheckHeader::GetDestinationOnly () const
{
  return (m_flags & (1 << 4));
}

void
WHCheckHeader::SetUnknownSeqno (bool f)
{
  if (f)
    {
      m_flags |= (1 << 3);
    }
  else
    {
      m_flags &= ~(1 << 3);
    }
}

bool
WHCheckHeader::GetUnknownSeqno () const
{
  return (m_flags & (1 << 3));
}

bool
WHCheckHeader::operator== (WHCheckHeader const & o) const
{
  return (m_flags == o.m_flags && m_reserved == o.m_reserved
          && m_hopCount == o.m_hopCount && m_WHCheckID == o.m_WHCheckID
          && m_fir == o.m_fir && m_firSeqNo == o.m_firSeqNo
          && m_origin == o.m_origin && m_originSeqNo == o.m_originSeqNo
          && m_sec == o.m_sec && m_src == o.m_src && m_dst == o.m_dst);
}


//-----------------------------------------------------------------------------
// WHCE
//-----------------------------------------------------------------------------

WHEndHeader::WHEndHeader (uint8_t prefixSize, uint8_t hopCount, uint32_t WHEndID,
                        Ipv4Address dst, uint32_t dstSeqNo, Ipv4Address origin, 
                        Ipv4Address src, Ipv4Address aodv_dst,
                        Time lifeTime, uint8_t rrepid, 
                        //WH攻撃を行ったかわかる用のフラグ
                        uint8_t WHflag, /*RREQのID*/uint32_t rreq_id)
  : m_flags (0),
    m_prefixSize (prefixSize),
    m_hopCount (hopCount),
    m_WHEndID (WHEndID),
    m_dst (dst),
    m_dstSeqNo (dstSeqNo),
    m_origin (origin),
    m_source (src),
    m_aodv_dst (aodv_dst),
    m_rrepid (rrepid),
    m_WHflag (WHflag),
    m_rreq_id (rreq_id)
    {
    m_lifeTime = uint32_t (lifeTime.GetMilliSeconds ());
    }

NS_OBJECT_ENSURE_REGISTERED (WHEndHeader);

TypeId
WHEndHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::aodv::WHEndHeader")
    .SetParent<Header> ()
    .SetGroupName ("Aodv")
    .AddConstructor<WHEndHeader> ()
  ;
  return tid;
}

TypeId
WHEndHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
WHEndHeader::GetSerializedSize () const
{
  //return 32;
  //32+1(フラグ用)
  return 33 + 4/*RREQのID分*/;
  // return 31;
  //return 27;
}

void
WHEndHeader::Serialize (Buffer::Iterator i) const
{
  i.WriteU8 (m_flags);
  i.WriteU8 (m_prefixSize);
  i.WriteU8 (m_hopCount);
  i.WriteHtonU32 (m_WHEndID);
  WriteTo (i, m_dst);
  i.WriteHtonU32 (m_dstSeqNo);
  WriteTo (i, m_origin);
  WriteTo (i, m_source);
  WriteTo (i, m_aodv_dst);
  i.WriteHtonU32 (m_lifeTime);
  i.WriteU8 (m_rrepid);

  //WH攻撃を行ったかわかる用のフラグ
  i.WriteU8 (m_WHflag);
  //RREQのID
  i.WriteHtonU32 (m_rreq_id);
}

uint32_t
WHEndHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_flags = i.ReadU8 ();
  m_prefixSize = i.ReadU8 ();
  m_hopCount = i.ReadU8 ();
  m_WHEndID = i.ReadNtohU32 ();
  ReadFrom (i, m_dst);
  m_dstSeqNo = i.ReadNtohU32 ();
  ReadFrom (i, m_origin);
  ReadFrom (i, m_source);
  ReadFrom (i, m_aodv_dst);
  m_lifeTime = i.ReadNtohU32 ();
  m_rrepid = i.ReadU8 ();

  //WH攻撃を行ったかわかる用のフラグ
  m_WHflag = i.ReadU8 ();
  //RREQのID
  m_rreq_id = i.ReadNtohU32 ();

  uint32_t dist = i.GetDistanceFrom (start);
  NS_ASSERT (dist == GetSerializedSize ());
  return dist;
}

void
WHEndHeader::Print (std::ostream &os) const
{
  os << "WHCS ID " << m_WHEndID << "destination: ipv4 " << m_dst << " sequence number " << m_dstSeqNo;
  if (m_prefixSize != 0)
    {
      os << " prefix size " << m_prefixSize;
    }
  os << " origin ipv4 " << m_origin << " source ipv4 " << m_source << " dst ipv4 " << m_aodv_dst 
  << " lifetime " << m_lifeTime
  << " acknowledgment required flag " << (*this).GetAckRequired ();
}

void
WHEndHeader::SetLifeTime (Time t)
{
  m_lifeTime = t.GetMilliSeconds ();
}

Time
WHEndHeader::GetLifeTime () const
{
  Time t (MilliSeconds (m_lifeTime));
  return t;
}

void
WHEndHeader::SetAckRequired (bool f)
{
  if (f)
    {
      m_flags |= (1 << 6);
    }
  else
    {
      m_flags &= ~(1 << 6);
    }
}

bool
WHEndHeader::GetAckRequired () const
{
  return (m_flags & (1 << 6));
}

void
WHEndHeader::SetPrefixSize (uint8_t sz)
{
  m_prefixSize = sz;
}

uint8_t
WHEndHeader::GetPrefixSize () const
{
  return m_prefixSize;
}

bool
WHEndHeader::operator== (WHEndHeader const & o) const
{
  return (m_flags == o.m_flags && m_prefixSize == o.m_prefixSize
          && m_hopCount == o.m_hopCount && m_WHEndID == o.m_WHEndID&& m_dst == o.m_dst && m_dstSeqNo == o.m_dstSeqNo
          && m_origin == o.m_origin && m_source == o.m_source && m_aodv_dst == o.m_aodv_dst 
          && m_lifeTime == o.m_lifeTime);
}

// void
// WHEndHeader::SetHello (Ipv4Address origin, Ipv4Address source, uint32_t srcSeqNo, Time lifetime)
// {
//   m_flags = 0;
//   m_prefixSize = 0;
//   m_hopCount = 0;
//   m_dst = origin;
//   m_dstSeqNo = srcSeqNo;
//   m_origin = origin;
//   m_source = source;
//   m_lifeTime = lifetime.GetMilliSeconds ();
// }

std::ostream &
operator<< (std::ostream & os, WHEndHeader const & h)
{
  h.Print (os);
  return os;
}


//-----------------------------------------------------------------------------
// RREP
//-----------------------------------------------------------------------------

RrepHeader::RrepHeader (uint8_t prefixSize, uint8_t hopCount, Ipv4Address dst,
                        uint32_t dstSeqNo, Ipv4Address origin, Time lifeTime, uint8_t id, uint32_t rreqid)
  : m_flags (0),
    m_prefixSize (prefixSize),
    m_hopCount (hopCount),
    m_dst (dst),
    m_dstSeqNo (dstSeqNo),
    m_origin (origin),
    m_id (id),
    m_rreqid (rreqid)
      
{
  m_lifeTime = uint32_t (lifeTime.GetMilliSeconds ());
}

NS_OBJECT_ENSURE_REGISTERED (RrepHeader);

TypeId
RrepHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::aodv::RrepHeader")
    .SetParent<Header> ()
    .SetGroupName ("Aodv")
    .AddConstructor<RrepHeader> ()
  ;
  return tid;
}

TypeId
RrepHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
RrepHeader::GetSerializedSize () const
{
  return 24;
  //return 20;
  // return 19;
}

void
RrepHeader::Serialize (Buffer::Iterator i) const
{
  i.WriteU8 (m_flags);
  i.WriteU8 (m_prefixSize);
  i.WriteU8 (m_hopCount);
  WriteTo (i, m_dst);
  i.WriteHtonU32 (m_dstSeqNo);
  WriteTo (i, m_origin);
  i.WriteHtonU32 (m_lifeTime);
  i.WriteU8 (m_id);
  i.WriteU8 (m_rreqid);
}

uint32_t
RrepHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_flags = i.ReadU8 ();
  m_prefixSize = i.ReadU8 ();
  m_hopCount = i.ReadU8 ();
  ReadFrom (i, m_dst);
  m_dstSeqNo = i.ReadNtohU32 ();
  ReadFrom (i, m_origin);
  m_lifeTime = i.ReadNtohU32 ();
  m_id = i.ReadU8 ();
  m_rreqid = i.ReadNtohU32 ();

  uint32_t dist = i.GetDistanceFrom (start);
  NS_ASSERT (dist == GetSerializedSize ());
  return dist;
}

void
RrepHeader::Print (std::ostream &os) const
{
  os << "destination: ipv4 " << m_dst << " sequence number " << m_dstSeqNo;
  if (m_prefixSize != 0)
    {
      os << " prefix size " << m_prefixSize;
    }
  os << " source ipv4 " << m_origin << " lifetime " << m_lifeTime
     << " acknowledgment required flag " << (*this).GetAckRequired ();
}

void
RrepHeader::SetLifeTime (Time t)
{
  m_lifeTime = t.GetMilliSeconds ();
}

Time
RrepHeader::GetLifeTime () const
{
  Time t (MilliSeconds (m_lifeTime));
  return t;
}

void
RrepHeader::SetAckRequired (bool f)
{
  if (f)
    {
      m_flags |= (1 << 6);
    }
  else
    {
      m_flags &= ~(1 << 6);
    }
}

bool
RrepHeader::GetAckRequired () const
{
  return (m_flags & (1 << 6));
}

void
RrepHeader::SetPrefixSize (uint8_t sz)
{
  m_prefixSize = sz;
}

uint8_t
RrepHeader::GetPrefixSize () const
{
  return m_prefixSize;
}

bool
RrepHeader::operator== (RrepHeader const & o) const
{
  return (m_flags == o.m_flags && m_prefixSize == o.m_prefixSize
          && m_hopCount == o.m_hopCount && m_dst == o.m_dst && m_dstSeqNo == o.m_dstSeqNo
          && m_origin == o.m_origin && m_lifeTime == o.m_lifeTime);
}

void
RrepHeader::SetHello (Ipv4Address origin, uint32_t srcSeqNo, Time lifetime)
{
  m_flags = 0;
  m_prefixSize = 0;
  m_hopCount = 0;
  m_dst = origin;
  m_dstSeqNo = srcSeqNo;
  m_origin = origin;
  m_lifeTime = lifetime.GetMilliSeconds ();
  m_id = 0;
  m_rreqid = 0;
}

std::ostream &
operator<< (std::ostream & os, RrepHeader const & h)
{
  h.Print (os);
  return os;
}

//-----------------------------------------------------------------------------
// RREP-ACK
//-----------------------------------------------------------------------------

RrepAckHeader::RrepAckHeader ()
  : m_reserved (0)
{
}

NS_OBJECT_ENSURE_REGISTERED (RrepAckHeader);

TypeId
RrepAckHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::aodv::RrepAckHeader")
    .SetParent<Header> ()
    .SetGroupName ("Aodv")
    .AddConstructor<RrepAckHeader> ()
  ;
  return tid;
}

TypeId
RrepAckHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
RrepAckHeader::GetSerializedSize () const
{
  return 1;
}

void
RrepAckHeader::Serialize (Buffer::Iterator i ) const
{
  i.WriteU8 (m_reserved);
}

uint32_t
RrepAckHeader::Deserialize (Buffer::Iterator start )
{
  Buffer::Iterator i = start;
  m_reserved = i.ReadU8 ();
  uint32_t dist = i.GetDistanceFrom (start);
  NS_ASSERT (dist == GetSerializedSize ());
  return dist;
}

void
RrepAckHeader::Print (std::ostream &os ) const
{
}

bool
RrepAckHeader::operator== (RrepAckHeader const & o ) const
{
  return m_reserved == o.m_reserved;
}

std::ostream &
operator<< (std::ostream & os, RrepAckHeader const & h )
{
  h.Print (os);
  return os;
}

//-----------------------------------------------------------------------------
// RERR
//-----------------------------------------------------------------------------
RerrHeader::RerrHeader ()
  : m_flag (0),
    m_reserved (0)
{
}

NS_OBJECT_ENSURE_REGISTERED (RerrHeader);

TypeId
RerrHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::aodv::RerrHeader")
    .SetParent<Header> ()
    .SetGroupName ("Aodv")
    .AddConstructor<RerrHeader> ()
  ;
  return tid;
}

TypeId
RerrHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
RerrHeader::GetSerializedSize () const
{
  return (3 + 8 * GetDestCount ());
}

void
RerrHeader::Serialize (Buffer::Iterator i ) const
{
  i.WriteU8 (m_flag);
  i.WriteU8 (m_reserved);
  i.WriteU8 (GetDestCount ());
  std::map<Ipv4Address, uint32_t>::const_iterator j;
  for (j = m_unreachableDstSeqNo.begin (); j != m_unreachableDstSeqNo.end (); ++j)
    {
      WriteTo (i, (*j).first);
      i.WriteHtonU32 ((*j).second);
    }
}

uint32_t
RerrHeader::Deserialize (Buffer::Iterator start )
{
  Buffer::Iterator i = start;
  m_flag = i.ReadU8 ();
  m_reserved = i.ReadU8 ();
  uint8_t dest = i.ReadU8 ();
  m_unreachableDstSeqNo.clear ();
  Ipv4Address address;
  uint32_t seqNo;
  for (uint8_t k = 0; k < dest; ++k)
    {
      ReadFrom (i, address);
      seqNo = i.ReadNtohU32 ();
      m_unreachableDstSeqNo.insert (std::make_pair (address, seqNo));
    }

  uint32_t dist = i.GetDistanceFrom (start);
  NS_ASSERT (dist == GetSerializedSize ());
  return dist;
}

void
RerrHeader::Print (std::ostream &os ) const
{
  os << "Unreachable destination (ipv4 address, seq. number):";
  std::map<Ipv4Address, uint32_t>::const_iterator j;
  for (j = m_unreachableDstSeqNo.begin (); j != m_unreachableDstSeqNo.end (); ++j)
    {
      os << (*j).first << ", " << (*j).second;
    }
  os << "No delete flag " << (*this).GetNoDelete ();
}

void
RerrHeader::SetNoDelete (bool f )
{
  if (f)
    {
      m_flag |= (1 << 0);
    }
  else
    {
      m_flag &= ~(1 << 0);
    }
}

bool
RerrHeader::GetNoDelete () const
{
  return (m_flag & (1 << 0));
}

bool
RerrHeader::AddUnDestination (Ipv4Address dst, uint32_t seqNo )
{
  if (m_unreachableDstSeqNo.find (dst) != m_unreachableDstSeqNo.end ())
    {
      return true;
    }

  NS_ASSERT (GetDestCount () < 255); // can't support more than 255 destinations in single RERR
  m_unreachableDstSeqNo.insert (std::make_pair (dst, seqNo));
  return true;
}

bool
RerrHeader::RemoveUnDestination (std::pair<Ipv4Address, uint32_t> & un )
{
  if (m_unreachableDstSeqNo.empty ())
    {
      return false;
    }
  std::map<Ipv4Address, uint32_t>::iterator i = m_unreachableDstSeqNo.begin ();
  un = *i;
  m_unreachableDstSeqNo.erase (i);
  return true;
}

void
RerrHeader::Clear ()
{
  m_unreachableDstSeqNo.clear ();
  m_flag = 0;
  m_reserved = 0;
}

bool
RerrHeader::operator== (RerrHeader const & o ) const
{
  if (m_flag != o.m_flag || m_reserved != o.m_reserved || GetDestCount () != o.GetDestCount ())
    {
      return false;
    }

  std::map<Ipv4Address, uint32_t>::const_iterator j = m_unreachableDstSeqNo.begin ();
  std::map<Ipv4Address, uint32_t>::const_iterator k = o.m_unreachableDstSeqNo.begin ();
  for (uint8_t i = 0; i < GetDestCount (); ++i)
    {
      if ((j->first != k->first) || (j->second != k->second))
        {
          return false;
        }

      j++;
      k++;
    }
  return true;
}

std::ostream &
operator<< (std::ostream & os, RerrHeader const & h )
{
  h.Print (os);
  return os;
}
}
}
