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
 *          Pavel Boyko <boyko@iitp.ru>
 */
#ifndef AODVPACKET_H
#define AODVPACKET_H

#include <iostream>
#include "ns3/header.h"
#include "ns3/enum.h"
#include "ns3/ipv4-address.h"
#include <map>
#include "ns3/nstime.h"

namespace ns3 {
namespace aodv {

/**
* \ingroup aodv
* \brief MessageType enumeration
*/
enum MessageType
{
  AODVTYPE_RREQ  = 1,   //!< AODVTYPE_RREQ
  AODVTYPE_RREP  = 2,   //!< AODVTYPE_RREP
  AODVTYPE_RERR  = 3,   //!< AODVTYPE_RERR
  AODVTYPE_RREP_ACK = 4, //!< AODVTYPE_RREP_ACK
  AODVTYPE_WHCS  = 5,   //!< AODVTYPE_WHCS
  AODVTYPE_WHCE  = 6,   //!< AODVTYPE_WHCE
};

/**
* \ingroup aodv
* \brief AODV types
*/
class TypeHeader : public Header
{
public:
  /**
   * constructor
   * \param t the AODV RREQ type
   */
  TypeHeader (MessageType t = AODVTYPE_RREQ);

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId ();
  TypeId GetInstanceTypeId () const;
  uint32_t GetSerializedSize () const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);
  void Print (std::ostream &os) const;

  /**
   * \returns the type
   */
  MessageType Get () const
  {
    return m_type;
  }
  /**
   * Check that type if valid
   * \returns true if the type is valid
   */
  bool IsValid () const
  {
    return m_valid;
  }
  /**
   * \brief Comparison operator
   * \param o header to compare
   * \return true if the headers are equal
   */
  bool operator== (TypeHeader const & o) const;
private:
  MessageType m_type; ///< type of the message
  bool m_valid; ///< Indicates if the message is valid
};

/**
  * \brief Stream output operator
  * \param os output stream
  * \return updated stream
  */
std::ostream & operator<< (std::ostream & os, TypeHeader const & h);

/**
* \ingroup aodv
* \brief   Route Request (RREQ) Message Format
  \verbatim
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |     Type      |J|R|G|D|U|   Reserved          |   Hop Count   |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                            RREQ ID                            |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    Destination IP Address                     |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                  Destination Sequence Number                  |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    Originator IP Address                      |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                  Originator Sequence Number                   |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    Second Node IP Address                      |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  \endverbatim
*/
class RreqHeader : public Header
{
public:
  /**
   * constructor
   *
   * \param flags the message flags (0)
   * \param reserved the reserved bits (0)
   * \param hopCount the hop count
   * \param requestID the request ID
   * \param dst the destination IP address
   * \param dstSeqNo the destination sequence number
   * \param origin the origin IP address
   * \param originSeqNo the origin sequence number
   * \param second second node IP address
   */
   RreqHeader (uint8_t flags = 0, uint8_t reserved = 0, uint8_t hopCount = 0,
              uint32_t requestID = 0, Ipv4Address dst = Ipv4Address (),
              uint32_t dstSeqNo = 0, Ipv4Address origin = Ipv4Address (),
              uint32_t originSeqNo = 0, Ipv4Address second = Ipv4Address ());

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId ();
  TypeId GetInstanceTypeId () const;
  uint32_t GetSerializedSize () const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);
  void Print (std::ostream &os) const;

  // Fields
  /**
   * \brief Set the hop count
   * \param count the hop count
   */
  void SetHopCount (uint8_t count)
  {
    m_hopCount = count;
  }
  /**
   * \brief Get the hop count
   * \return the hop count
   */
  uint8_t GetHopCount () const
  {
    return m_hopCount;
  }
  /**
   * \brief Set the request ID
   * \param id the request ID
   */
  void SetId (uint32_t id)
  {
    m_requestID = id;
  }
  /**
   * \brief Get the request ID
   * \return the request ID
   */
  uint32_t GetId () const
  {
    return m_requestID;
  }
  /**
   * \brief Set the destination address
   * \param a the destination address
   */
  void SetDst (Ipv4Address a)
  {
    m_dst = a;
  }
  /**
   * \brief Get the destination address
   * \return the destination address
   */
  Ipv4Address GetDst () const
  {
    return m_dst;
  }
  /**
   * \brief Set the destination sequence number
   * \param s the destination sequence number
   */
  void SetDstSeqno (uint32_t s)
  {
    m_dstSeqNo = s;
  }
  /**
   * \brief Get the destination sequence number
   * \return the destination sequence number
   */
  uint32_t GetDstSeqno () const
  {
    return m_dstSeqNo;
  }
  /**
   * \brief Set the origin address
   * \param a the origin address
   */
  void SetOrigin (Ipv4Address a)
  {
    m_origin = a;
  }

    void SetSecond (Ipv4Address s)
  {
    m_second = s;
  }

  /**
   * \brief Get the origin address
   * \return the origin address
   */
  Ipv4Address GetOrigin () const
  {
    return m_origin;
  }

  Ipv4Address GetSecond () const
  {
    return m_second;
  }

  /**
   * \brief Set the origin sequence number
   * \param s the origin sequence number
   */
  void SetOriginSeqno (uint32_t s)
  {
    m_originSeqNo = s;
  }
  /**
   * \brief Get the origin sequence number
   * \return the origin sequence number
   */
  uint32_t GetOriginSeqno () const
  {
    return m_originSeqNo;
  }

  // Flags
  /**
   * \brief Set the gratuitous RREP flag
   * \param f the gratuitous RREP flag
   */
  void SetGratuitousRrep (bool f);
  /**
   * \brief Get the gratuitous RREP flag
   * \return the gratuitous RREP flag
   */
  bool GetGratuitousRrep () const;
  /**
   * \brief Set the Destination only flag
   * \param f the Destination only flag
   */
  void SetDestinationOnly (bool f);
  /**
   * \brief Get the Destination only flag
   * \return the Destination only flag
   */
  bool GetDestinationOnly () const;
  /**
   * \brief Set the unknown sequence number flag
   * \param f the unknown sequence number flag
   */
  void SetUnknownSeqno (bool f);
  /**
   * \brief Get the unknown sequence number flag
   * \return the unknown sequence number flag
   */
  bool GetUnknownSeqno () const;

  /**
   * \brief Comparison operator
   * \param o RREQ header to compare
   * \return true if the RREQ headers are equal
   */
  bool operator== (RreqHeader const & o) const;
private:
  uint8_t        m_flags;          ///< |J|R|G|D|U| bit flags, see RFC
  uint8_t        m_reserved;       ///< Not used (must be 0)
  uint8_t        m_hopCount;       ///< Hop Count
  uint32_t       m_requestID;      ///< RREQ ID
  Ipv4Address    m_dst;            ///< Destination IP Address
  uint32_t       m_dstSeqNo;       ///< Destination Sequence Number
  Ipv4Address    m_origin;         ///< Originator IP Address
  uint32_t       m_originSeqNo;    ///< Source Sequence Number
  Ipv4Address    m_second;         ///< Second node IP Address
};

/**
  * \brief Stream output operator
  * \param os output stream
  * \return updated stream
  */
std::ostream & operator<< (std::ostream & os, RreqHeader const &);



//WHCS メッセージ　作成部分

/**
* \ingroup aodv
* \brief   WH Check Start (WHCS) Message Format
  \verbatim
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |     Type      |J|R|G|D|U|   Reserved          |   Hop Count   |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                            WHCS ID                            |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    First Hop IP Address                     |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                  First Hop Sequence Number                  |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    Originator IP Address                      |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                  Originator Sequence Number                   |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    Second Hop IP Address                      |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    AODVの宛先IP Address                      |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  \endverbatim
*/
class WHCheckHeader : public Header
{
public:
  /**
   * constructor
   *
   * \param flags the message flags (0)
   * \param reserved the reserved bits (0)
   * \param hopCount the hop count
   * \param WHCSID the WHCS ID
   * \param fir the First Hop IP address
   * \param firSeqNo the First Node sequence number
   * \param origin the origin IP address
   * \param originSeqNo the origin sequence number
   * \param sec the Second Hopp IP address
   * \param src the sorce Hopp IP address
   */
   WHCheckHeader (uint8_t flags = 0, uint8_t reserved = 0, uint8_t hopCount = 0,
              uint32_t WHCheckID = 0, Ipv4Address fir = Ipv4Address (),
              uint32_t firSeqNo = 0, Ipv4Address origin = Ipv4Address (),
              uint32_t originSeqNo = 0, Ipv4Address sec = Ipv4Address (), Ipv4Address src = Ipv4Address (),
              Ipv4Address Dst = Ipv4Address(), uint8_t WHF = 0, uint8_t rrepid = 0);

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId ();
  TypeId GetInstanceTypeId () const;
  uint32_t GetSerializedSize () const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);
  void Print (std::ostream &os) const;

  // Fields
  /**
   * \brief Set the hop count
   * \param count the hop count
   */
  void SetHopCount (uint8_t count)
  {
    m_hopCount = count;
  }
  /**
   * \brief Get the hop count
   * \return the hop count
   */
  uint8_t GetHopCount () const
  {
    return m_hopCount;
  }
  /**
   * \brief Set the request ID
   * \param id the request ID
   */
  void SetId (uint32_t id)
  {
    m_WHCheckID = id;
  }
  /**
   * \brief Get the request ID
   * \return the request ID
   */
  uint32_t GetId () const
  {
    return m_WHCheckID;
  }
  /**
   * \brief Set the destination address
   * \param a the destination address
   */
  void SetFir (Ipv4Address a)
  {
    m_fir = a;
  }
  /**
   * \brief Get the destination address
   * \return the destination address
   */
  Ipv4Address GetFir () const
  {
    return m_fir;
  }
  /**
   * \brief Set the destination sequence number
   * \param s the destination sequence number
   */
  void SetFirSeqno (uint32_t s)
  {
    m_firSeqNo = s;
  }
  /**
   * \brief Get the destination sequence number
   * \return the destination sequence number
   */
  uint32_t GetFirSeqno () const
  {
    return m_firSeqNo;
  }
  /**
   * \brief Set the origin address
   * \param a the origin address
   */
  void SetOrigin (Ipv4Address o)
  {
    m_origin = o;
  }

  void SetSecond (Ipv4Address s)
  {
    m_sec = s;
  }

  /**
   * \brief Get the origin address
   * \return the origin address
   */
  Ipv4Address GetOrigin () const
  {
    return m_origin;
  }

  Ipv4Address GetSecond () const
  {
    return m_sec;
  }

  void SetSrc (Ipv4Address a)
  {
    m_src = a;
  }

  Ipv4Address GetSrc () const
  {
    return m_src;
  }

  void SetDst (Ipv4Address dst)
  {
    m_dst = dst;
  }

  Ipv4Address GetDst () const
  {
    return m_dst;
  }

  void SetRREPid (uint8_t rrepid)
  {
    m_rrepid = rrepid;
  }

  uint8_t GetRREPid() const
  {
    return m_rrepid;
  }

  /**
   * \brief Set the origin sequence number
   * \param s the origin sequence number
   */
  void SetOriginSeqno (uint32_t s)
  {
    m_originSeqNo = s;
  }
  /**
   * \brief Get the origin sequence number
   * \return the origin sequence number
   */
  uint32_t GetOriginSeqno () const
  {
    return m_originSeqNo;
  }

  void SetWHF(uint8_t t)
  {
    m_whf = t;
  }

  uint8_t GetWHF ()
  {
    return m_whf;
  }

  // Flags
  /**
   * \brief 無償WHCEフラグの設定
   * \param f the gratuitous RREP flag
   */
  void SetGratuitousWHCE (bool f);
  /**
   * \brief　無償のWHCEフラッグを手に入れる
   * \return the gratuitous RREP flag
   */
  bool GetGratuitousWHCE () const;
  /**
   * \brief デスティネーション専用フラグを設定
   * \param f the Destination only flag
   */
  void SetDestinationOnly (bool f);
  /**
   * \brief デスティネーション専用フラグの取得
   * \return the Destination only flag
   */
  bool GetDestinationOnly () const;
  /**
   * \brief 未知のシーケンス番号フラグを設定
   * \param f the unknown sequence number flag
   */
  void SetUnknownSeqno (bool f);
  /**
   * \brief 未知のシーケンス番号フラグを取得
   * \return the unknown sequence number flag
   */
  bool GetUnknownSeqno () const;

  /**
   * \brief 比較演算子
   * \param o 比較するWHCSヘッダー
   * \return WHCSヘッダーが等しければ真
   */
  bool operator== (WHCheckHeader const & o) const;
private:
  uint8_t        m_flags;          ///< |J|R|G|D|U| bit flags, see RFC
  uint8_t        m_reserved;       ///< Not used (must be 0)
  uint8_t        m_hopCount;       ///< Hop Count
  uint32_t       m_WHCheckID;      ///< WHCS ID
  Ipv4Address    m_fir;            ///< First Hop IP Address
  uint32_t       m_firSeqNo;       ///< First Hop Sequence Number
  Ipv4Address    m_origin;         ///< Originator IP Address
  uint32_t       m_originSeqNo;    ///< Source Sequence Number
  Ipv4Address    m_sec;            ///< Second node IP Address
  Ipv4Address    m_src;            ///< Sorece node IP Address
  Ipv4Address  m_dst;
  uint8_t m_whf;
  uint8_t        m_rrepid;
};

/**
  * \brief Stream output operator
  * \param os output stream
  * \return updated stream
  */
std::ostream & operator<< (std::ostream & os, RreqHeader const &);


/**
* \ingroup aodv
* \brief Route Reply (RREP) Message Format
  \verbatim
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |     Type      |R|A|    Reserved     |Prefix Sz|   Hop Count   |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                     Destination IP address                    |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                  Destination Sequence Number                  |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    Originator IP address                      |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                           Lifetime                            |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  \endverbatim
*/
class RrepHeader : public Header
{
public:
  /**
   * constructor
   *
   * \param prefixSize the prefix size (0)
   * \param hopCount the hop count (0)
   * \param dst the destination IP address
   * \param dstSeqNo the destination sequence number
   * \param origin the origin IP address
   * \param lifetime the lifetime
   */
  RrepHeader (uint8_t prefixSize = 0, uint8_t hopCount = 0, Ipv4Address dst =
                Ipv4Address (), uint32_t dstSeqNo = 0, Ipv4Address origin =
                Ipv4Address (), Time lifetime = MilliSeconds (0), uint8_t id = 0);
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId ();
  TypeId GetInstanceTypeId () const;
  uint32_t GetSerializedSize () const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);
  void Print (std::ostream &os) const;

  // Fields
  /**
   * \brief Set the hop count
   * \param count the hop count
   */
  void SetHopCount (uint8_t count)
  {
    m_hopCount = count;
  }
  /**
   * \brief Get the hop count
   * \return the hop count
   */
  uint8_t GetHopCount () const
  {
    return m_hopCount;
  }
  /**
   * \brief Set the destination address
   * \param a the destination address
   */
  void SetDst (Ipv4Address a)
  {
    m_dst = a;
  }
  /**
   * \brief Get the destination address
   * \return the destination address
   */
  Ipv4Address GetDst () const
  {
    return m_dst;
  }
  /**
   * \brief Set the destination sequence number
   * \param s the destination sequence number
   */
  void SetDstSeqno (uint32_t s)
  {
    m_dstSeqNo = s;
  }
  /**
   * \brief Get the destination sequence number
   * \return the destination sequence number
   */
  uint32_t GetDstSeqno () const
  {
    return m_dstSeqNo;
  }
  /**
   * \brief Set the origin address
   * \param a the origin address
   */
  void SetOrigin (Ipv4Address a)
  {
    m_origin = a;
  }
  /**
   * \brief Get the origin address
   * \return the origin address
   */
  Ipv4Address GetOrigin () const
  {
    return m_origin;
  }
  /**
   * \brief Set the lifetime
   * \param t the lifetime
   */
  void SetLifeTime (Time t);
  /**
   * \brief Get the lifetime
   * \return the lifetime
   */
  Time GetLifeTime () const;

  void Setid (uint8_t id)
  {
    m_id = id;
  }

  uint8_t Getid ()
  {
    return m_id;
  }

  // Flags
  /**
   * \brief Set the ack required flag
   * \param f the ack required flag
   */
  void SetAckRequired (bool f);
  /**
   * \brief get the ack required flag
   * \return the ack required flag
   */
  bool GetAckRequired () const;
  /**
   * \brief Set the prefix size
   * \param sz the prefix size
   */
  void SetPrefixSize (uint8_t sz);
  /**
   * \brief Set the pefix size
   * \return the prefix size
   */
  uint8_t GetPrefixSize () const;

  /**
   * Configure RREP to be a Hello message
   *
   * \param src the source IP address
   * \param srcSeqNo the source sequence number
   * \param lifetime the lifetime of the message
   */
  void SetHello (Ipv4Address src, uint32_t srcSeqNo, Time lifetime);

  /**
   * \brief Comparison operator
   * \param o RREP header to compare
   * \return true if the RREP headers are equal
   */
  bool operator== (RrepHeader const & o) const;
private:
  uint8_t       m_flags;                  ///< A - acknowledgment required flag
  uint8_t       m_prefixSize;         ///< Prefix Size
  uint8_t       m_hopCount;         ///< Hop Count
  Ipv4Address   m_dst;              ///< Destination IP Address
  uint32_t      m_dstSeqNo;         ///< Destination Sequence Number
  Ipv4Address   m_origin;           ///< Source IP Address
  uint32_t      m_lifeTime;         ///< Lifetime (in milliseconds)
  uint8_t       m_id;
};

/**
  * \brief Stream output operator
  * \param os output stream
  * \return updated stream
  */
std::ostream & operator<< (std::ostream & os, RrepHeader const &);


//WHCE
/**
* \ingroup aodv
* \brief WHCheckEnd Message Format
  \verbatim
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |     Type      |R|A|    Reserved     |Prefix Sz|   Hop Count   |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                            WHCE ID                            |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                     Destination IP address                    |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                  Destination Sequence Number                  |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    Originator IP address                      |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    source IP address                      |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                           Lifetime                            |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  \endverbatim
*/
class WHEndHeader : public Header
{
public:
  /**
   * constructor
   *
   * \param prefixSize プリフィックスのサイズ (0)
   * \param WHEndID WHCE のID
   * \param hopCount ホップ数 (0)
   * \param dst 宛先IPアドレス
   * \param dstSeqNo 宛先シーケンス番号
   * \param origin WHC発信元IPアドレス
   * \param source AODVの発信元のIPアドレス
   * \param lifetime 生存期間
   */
  WHEndHeader (uint8_t prefixSize = 0, uint8_t hopCount = 0, uint32_t WHEndID = 0,
                Ipv4Address dst =Ipv4Address (),uint32_t dstSeqNo = 0, Ipv4Address origin =Ipv4Address (),
                Ipv4Address source =Ipv4Address (),Ipv4Address aodv_dst = Ipv4Address(), 
                Time lifetime = MilliSeconds (0), uint8_t rrepid = 0);
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId ();
  TypeId GetInstanceTypeId () const;
  uint32_t GetSerializedSize () const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);
  void Print (std::ostream &os) const;

  // Fields
  /**
   * \brief Set the hop count
   * \param count the hop count
   */
  void SetHopCount (uint8_t count)
  {
    m_hopCount = count;
  }
  /**
   * \brief Get the hop count
   * \return the hop count
   */
  uint8_t GetHopCount () const
  {
    return m_hopCount;
  }

  void SetId (uint32_t id)
  {
    m_WHEndID = id;
  }
  /**
   * \brief Get the request ID
   * \return the request ID
   */
  uint32_t GetId () const
  {
    return m_WHEndID;
  }

  /**
   * \brief Set the destination address
   * \param a the destination address
   */
  void SetDst (Ipv4Address a)
  {
    m_dst = a;
  }
  /**
   * \brief Get the destination address
   * \return the destination address
   */
  Ipv4Address GetDst () const
  {
    return m_dst;
  }
  /**
   * \brief Set the destination sequence number
   * \param s the destination sequence number
   */
  void SetDstSeqno (uint32_t s)
  {
    m_dstSeqNo = s;
  }
  /**
   * \brief Get the destination sequence number
   * \return the destination sequence number
   */
  uint32_t GetDstSeqno () const
  {
    return m_dstSeqNo;
  }
  /**
   * \brief Set the origin address
   * \param a the origin address
   */
  void SetOrigin (Ipv4Address a)
  {
    m_origin = a;
  }
  /**
   * \brief Get the origin address
   * \return the origin address
   */
  Ipv4Address GetOrigin () const
  {
    return m_origin;
  }

    /**
   * \brief Set the origin address
   * \param a the origin address
   */
  void SetSrc (Ipv4Address src)
  {
    m_source = src;
  }
  /**
   * \brief Get the origin address
   * \return the origin address
   */
  Ipv4Address GetSrc () const
  {
    return m_source;
  }

  void SetAodv_Dst (Ipv4Address aodv_dst)
  {
    m_aodv_dst = aodv_dst;
  }
  /**
   * \brief Get the origin address
   * \return the origin address
   */
  Ipv4Address GetAov_Dst () const
  {
    return m_aodv_dst;
  }

  void SetRREPid(uint8_t rrepid)
  {
    m_rrepid = rrepid;
  }

  uint8_t GetRREPid ()
  {
    return m_rrepid;
  }

  /**
   * \brief Set the lifetime
   * \param t the lifetime
   */
  void SetLifeTime (Time t);
  /**
   * \brief Get the lifetime
   * \return the lifetime
   */
  Time GetLifeTime () const;

  // Flags
  /**
   * \brief Set the ack required flag
   * \param f the ack required flag
   */
  void SetAckRequired (bool f);
  /**
   * \brief get the ack required flag
   * \return the ack required flag
   */
  bool GetAckRequired () const;
  /**
   * \brief Set the prefix size
   * \param sz the prefix size
   */
  void SetPrefixSize (uint8_t sz);
  /**
   * \brief Set the pefix size
   * \return the prefix size
   */
  uint8_t GetPrefixSize () const;

  /**
   * Configure RREP to be a Hello message
   *
   * \param src the source IP address
   * \param srcSeqNo the source sequence number
   * \param lifetime the lifetime of the message
   */
  void SetHello (Ipv4Address ori, Ipv4Address src, uint32_t srcSeqNo, Time lifetime);

  /**
   * \brief Comparison operator
   * \param o RREP header to compare
   * \return true if the RREP headers are equal
   */
  bool operator== (WHEndHeader const & o) const;
private:
  uint8_t       m_flags;                  ///< A - acknowledgment required flag
  uint8_t       m_prefixSize;         ///< Prefix Size
  uint8_t       m_hopCount;         ///< Hop Count
  uint32_t       m_WHEndID;         ///< WHCE ID
  Ipv4Address   m_dst;              ///< Destination IP Address
  uint32_t      m_dstSeqNo;         ///< Destination Sequence Number
  Ipv4Address   m_origin;           ///< origin IP Address
  Ipv4Address   m_source;           ///< Source IP Address
  Ipv4Address   m_aodv_dst;
  uint32_t      m_lifeTime;         ///< Lifetime (in milliseconds)
  uint8_t       m_rrepid;
};

/**
  * \brief Stream output operator
  * \param os output stream
  * \return updated stream
  */
std::ostream & operator<< (std::ostream & os, WHEndHeader const &);


/**
* \ingroup aodv
* \brief Route Reply Acknowledgment (RREP-ACK) Message Format
  \verbatim
  0                   1
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |     Type      |   Reserved    |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  \endverbatim
*/
class RrepAckHeader : public Header
{
public:
  /// constructor
  RrepAckHeader ();

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId ();
  TypeId GetInstanceTypeId () const;
  uint32_t GetSerializedSize () const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);
  void Print (std::ostream &os) const;

  /**
   * \brief Comparison operator
   * \param o RREP header to compare
   * \return true if the RREQ headers are equal
   */
  bool operator== (RrepAckHeader const & o) const;
private:
  uint8_t       m_reserved; ///< Not used (must be 0)
};

/**
  * \brief Stream output operator
  * \param os output stream
  * \return updated stream
  */
std::ostream & operator<< (std::ostream & os, RrepAckHeader const &);


/**
* \ingroup aodv
* \brief Route Error (RERR) Message Format
  \verbatim
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |     Type      |N|          Reserved           |   DestCount   |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |            Unreachable Destination IP Address (1)             |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |         Unreachable Destination Sequence Number (1)           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-|
  |  Additional Unreachable Destination IP Addresses (if needed)  |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |Additional Unreachable Destination Sequence Numbers (if needed)|
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  \endverbatim
*/
class RerrHeader : public Header
{
public:
  /// constructor
  RerrHeader ();

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId ();
  TypeId GetInstanceTypeId () const;
  uint32_t GetSerializedSize () const;
  void Serialize (Buffer::Iterator i) const;
  uint32_t Deserialize (Buffer::Iterator start);
  void Print (std::ostream &os) const;

  // No delete flag
  /**
   * \brief Set the no delete flag
   * \param f the no delete flag
   */
  void SetNoDelete (bool f);
  /**
   * \brief Get the no delete flag
   * \return the no delete flag
   */
  bool GetNoDelete () const;

  /**
   * \brief Add unreachable node address and its sequence number in RERR header
   * \param dst unreachable IPv4 address
   * \param seqNo unreachable sequence number
   * \return false if we already added maximum possible number of unreachable destinations
   */
  bool AddUnDestination (Ipv4Address dst, uint32_t seqNo);
  /**
   * \brief Delete pair (address + sequence number) from REER header, if the number of unreachable destinations > 0
   * \param un unreachable pair (address + sequence number)
   * \return true on success
   */
  bool RemoveUnDestination (std::pair<Ipv4Address, uint32_t> & un);
  /// Clear header
  void Clear ();
  /**
   * \returns number of unreachable destinations in RERR message
   */
  uint8_t GetDestCount () const
  {
    return (uint8_t)m_unreachableDstSeqNo.size ();
  }

  /**
   * \brief Comparison operator
   * \param o RERR header to compare
   * \return true if the RERR headers are equal
   */
  bool operator== (RerrHeader const & o) const;
private:
  uint8_t m_flag;            ///< No delete flag
  uint8_t m_reserved;        ///< Not used (must be 0)

  /// List of Unreachable destination: IP addresses and sequence numbers
  std::map<Ipv4Address, uint32_t> m_unreachableDstSeqNo;
};

/**
  * \brief Stream output operator
  * \param os output stream
  * \return updated stream
  */
std::ostream & operator<< (std::ostream & os, RerrHeader const &);

}  // namespace aodv
}  // namespace ns3

#endif /* AODVPACKET_H */
