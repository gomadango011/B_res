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
#ifndef AODV_RTABLE_H
#define AODV_RTABLE_H

#include <stdint.h>
#include <cassert>
#include <map>
#include <sys/types.h>
#include "ns3/ipv4.h"
#include "ns3/ipv4-route.h"
#include "ns3/timer.h"
#include "ns3/net-device.h"
#include "ns3/output-stream-wrapper.h"

namespace ns3 {
namespace aodv {

/**
 * \ingroup aodv
 * \brief Route record states
 */
enum RouteFlags
{
  VALID = 0,          //!< VALID
  INVALID = 1,        //!< INVALID
  IN_SEARCH = 2,      //!< IN_SEARCH
};

/**
 * \ingroup aodv
 * \brief Routing table entry
 */
class RoutingTableEntry //ルーティングテーブルエントリー　ルーティングテーブルの要素の操作？？
{
public:
  /**
   * constructor
   *
   * \param dev the device　デバイス
   * \param dst the destination IP address　目的地のアドレス
   * \param vSeqNo verify sequence number flag 検証シーケンス番号フラグ
   * \param seqNo the sequence number シーケンス番号
   * \param iface the interface　インターフェイス
   * \param hops the number of hops　ホップ数
   * \param nextHop the IP address of the next hopネクストホップのIPアドレス
   * \param secondHop ２つ目のホップのIPアドレス＊
   * \param lifetime エントリーの有効期間
   */
  RoutingTableEntry (Ptr<NetDevice> dev = 0,Ipv4Address dst = Ipv4Address (), bool vSeqNo = false, uint32_t seqNo = 0,
                     Ipv4InterfaceAddress iface = Ipv4InterfaceAddress (), uint16_t  hops = 0,
                     Ipv4Address nextHop = Ipv4Address (), Ipv4Address secondHop = Ipv4Address (), 
                     Time lifetime = Simulator::Now ());

  ~RoutingTableEntry ();

  ///\name Precursors management　前駆物質の管理
  //\{
  /**
   * Insert precursor in precuDelete precursorプリカーサが存在しない場合、プリカーサ・リストにプリカーサを挿入する。
   * \param id precursor address
   * \return true on success
   */
  bool InsertPrecursor (Ipv4Address id);
  /**
   * 
   * アドレスで前駆体を検索
   * \param id 前置アドレス
   * \return true on success
   */
  bool LookupPrecursor (Ipv4Address id);
  /**
   * \brief プリカーサーを削除
   * \param id precursor address
   * \return true on success
   */
  bool DeletePrecursor (Ipv4Address id);
  /// Delete all precursors
  void DeleteAllPrecursors ();
  /**
   * プリカーサーリストが空であることを確認する
   * \return true if precursor list is empty
   */
  bool IsPrecursorListEmpty () const;
  /**
   * プリカーサがまだベクトルに存在しない場合、プリカーサを出力パラメータprecに挿入する。
   * \param prec　前駆アドレスのベクトル
   */
  void GetPrecursors (std::vector<Ipv4Address> & prec) const;
  //\}

  /**
   * エントリーを "down "としてマークする（つまり無効にする）。
   * \param badLinkLifetime 無効とマークされたエントリーを保持する期間
   */
  void Invalidate (Time badLinkLifetime);

  // フィールド
  /**
   * 宛先アドレス取得機能
   * \returns IPv4宛先アドレス
   */
  Ipv4Address GetDestination () const
  {
    return m_ipv4Route->GetDestination ();
  }
  /**
   * ルート取得機能
   * \returns IPv4ルート
   */
  Ptr<Ipv4Route> GetRoute () const
  {
    return m_ipv4Route;
  }
  /**
   * Set route function
   * \param r the IPv4 route
   */
  void SetRoute (Ptr<Ipv4Route> r)
  {
    m_ipv4Route = r;
  }
  /**
   * Set next hop address
   * \param nextHop the next hop IPv4 address
   */
  void SetNextHop (Ipv4Address nextHop)
  {
    m_ipv4Route->SetGateway (nextHop);
  }

  /**
   * Set second hop address
   * \param nextHop the second hop IPv4 address
   */
    void SetSecondHop (Ipv4Address secondHop)
  {
    m_ipv4Route->SetSecond (secondHop);
  }
  /**
   * Get next hop address
   * \returns the next hop address
   */
  Ipv4Address GetNextHop () const
  {
    return m_ipv4Route->GetGateway ();
  }

  /**
   * Get second hop address
   * \returns the second hop address
   */
  Ipv4Address GetSecondHop () const
  {
    return m_ipv4Route->GetSecond ();
  }

  /**
   * Set output device
   * \param dev The output device
   */
  void SetOutputDevice (Ptr<NetDevice> dev)
  {
    m_ipv4Route->SetOutputDevice (dev);
  }
  /**
   * Get output device
   * \returns the output device
   */
  Ptr<NetDevice> GetOutputDevice () const
  {
    return m_ipv4Route->GetOutputDevice ();
  }
  /**
   * Get the Ipv4InterfaceAddress
   * \returns the Ipv4InterfaceAddress
   */
  Ipv4InterfaceAddress GetInterface () const
  {
    return m_iface;
  }
  /**
   * Set the Ipv4InterfaceAddress
   * \param iface The Ipv4InterfaceAddress
   */
  void SetInterface (Ipv4InterfaceAddress iface)
  {
    m_iface = iface;
  }
  /**
   * Set the valid sequence number
   * \param s the sequence number
   */
  void SetValidSeqNo (bool s)
  {
    m_validSeqNo = s;
  }
  /**
   * Get the valid sequence number
   * \returns the valid sequence number
   */
  bool GetValidSeqNo () const
  {
    return m_validSeqNo;
  }
  /**
   * Set the sequence number
   * \param sn the sequence number
   */
  void SetSeqNo (uint32_t sn)
  {
    m_seqNo = sn;
  }
  /**
   * Get the sequence number
   * \returns the sequence number
   */
  uint32_t GetSeqNo () const
  {
    return m_seqNo;
  }
  /**
   * Set the number of hops
   * \param hop the number of hops
   */
  void SetHop (uint16_t hop)
  {
    m_hops = hop;
  }
  /**
   * Get the number of hops
   * \returns the number of hops
   */
  uint16_t GetHop () const
  {
    return m_hops;
  }
  /**
   * Set the lifetime
   * \param lt The lifetime
   */
  void SetLifeTime (Time lt)
  {
    m_lifeTime = lt + Simulator::Now ();
  }
  /**
   * Get the lifetime
   * \returns the lifetime
   */
  Time GetLifeTime () const
  {
    return m_lifeTime - Simulator::Now ();
  }
  /**
   * Set the route flags
   * \param flag the route flags
   */
  void SetFlag (RouteFlags flag)
  {
    m_flag = flag;
  }
  /**
   * Get the route flags
   * \returns the route flags
   */
  RouteFlags GetFlag () const
  {
    return m_flag;
  }
  /**
   * Set the RREQ count
   * \param n the RREQ count
   */
  void SetRreqCnt (uint8_t n)
  {
    m_reqCount = n;
  }
  /**
   * Get the RREQ count
   * \returns the RREQ count
   */
  uint8_t GetRreqCnt () const
  {
    return m_reqCount;
  }
  /**
   * Increment the RREQ count
   */
  void IncrementRreqCnt ()
  {
    m_reqCount++;
  }
  /**
   * Set the unidirectional flag
   * \param u the uni directional flag
   */
  void SetUnidirectional (bool u)
  {
    m_blackListState = u;
  }
  /**
   * Get the unidirectional flag
   * \returns the unidirectional flag
   */
  bool IsUnidirectional () const
  {
    return m_blackListState;
  }
  /**
   * Set the blacklist timeout
   * \param t the blacklist timeout value
   */
  void SetBlacklistTimeout (Time t)
  {
    m_blackListTimeout = t;
  }
  /**
   * Get the blacklist timeout value
   * \returns the blacklist timeout value
   */
  Time GetBlacklistTimeout () const
  {
    return m_blackListTimeout;
  }
  /// RREP_ACK timer
  Timer m_ackTimer;

  /**
   * \brief 宛先アドレスの比較
   * \param dst 比較するIPアドレス
   * \return true if equal
   */
  bool operator== (Ipv4Address const  dst) const
  {
    return (m_ipv4Route->GetDestination () == dst);
  }
  /**
   * Print packet to trace file
   * \param stream The output stream
   */
  void Print (Ptr<OutputStreamWrapper> stream) const;

private:
  /// 有効な宛先シーケンス番号フラグ
  bool m_validSeqNo;
  /// 宛先シーケンス番号、m_validSeqNo = trueの場合
  uint32_t m_seqNo;
  /// ホップ数（目的地に到達するのに必要なホップ数）
  uint16_t m_hops;
  /**
  * \brief Expiration or deletion time of the route
  *	Lifetime field in the routing table plays dual role:
  *	for an active route it is the expiration time, and for an invalid route
  *	it is the deletion time.
  */
  Time m_lifeTime;
  /** Ip route, include
   *   - destination address
   *   - source address
   *   - next hop address (gateway)
   *   - output device
   */
  Ptr<Ipv4Route> m_ipv4Route;
  /// Output interface address
  Ipv4InterfaceAddress m_iface;
  /// Routing flags: valid, invalid or in search
  RouteFlags m_flag;

  /// List of precursors
  std::vector<Ipv4Address> m_precursorList;
  /// 次のリクエストを送信できる時間
  Time m_routeRequestTimout;
  /// Number of route requests
  uint8_t m_reqCount;
  /// このエントリーが "ブラックリスト "にあるかどうかを示す。
  bool m_blackListState;
  /// ノードがブラックリストに登録された時間
  Time m_blackListTimeout;
};

/**
 * \ingroup aodv
 * \brief The Routing table used by AODV protocol
 */
class RoutingTable //AODVプロトコルで使用されるルーティングテーブル　ルーティングテーブル全体の操作？
{
public:
  /**
   * constructor
   * \param t the routing table entry lifetime ルーティングテーブルエントリーの寿命
   */
  RoutingTable (Time t);
  ///\name 無効なルートのライフタイムの処理
  //\{
  Time GetBadLinkLifetime () const
  {
    return m_badLinkLifetime;
  }
  void SetBadLinkLifetime (Time t)
  {
    m_badLinkLifetime = t;
  }
  //\}
  /**
   * ルーティングテーブルにまだ存在しない場合は、ルーティングテーブルエントリーを追加する。
   * \param r routing table entry
   * \return true in success
   */
  bool AddRoute (RoutingTableEntry & r);
  /**
   * Delete routing table entry with destination address dst, if it exists.
   * \param dst destination address
   * \return true on success
   */
  bool DeleteRoute (Ipv4Address dst);
  /**
   * Lookup routing table entry with destination address dst
   * \param dst destination address
   * \param rt entry with destination address dst, if exists
   * \return true on success
   */
  bool LookupRoute (Ipv4Address dst, RoutingTableEntry & rt);
  /**
   * Lookup route in VALID state
   * \param dst destination address
   * \param rt entry with destination address dst, if exists
   * \return true on success
   */
  bool LookupValidRoute (Ipv4Address dst, RoutingTableEntry & rt);
  /**
   * Update routing table
   * \param rt entry with destination address dst, if exists
   * \return true on success
   */
  bool Update (RoutingTableEntry & rt);
  /**
   * Set routing table entry flags
   * \param dst destination address
   * \param state the routing flags
   * \return true on success
   */
  bool SetEntryState (Ipv4Address dst, RouteFlags state);
  /**
   * ネクストホップのアドレスが dst で、かつ前駆体のリストが空でないルーティングエントリを検索する。
   *
   * \param nextHop the next hop IP address
   * \param unreachable
   */
  void GetListOfDestinationWithNextHop (Ipv4Address nextHop, std::map<Ipv4Address, uint32_t> & unreachable);
  /**
   *   この宛先を持つルーティングエントリーを以下のように更新する：
   *  1. このルーティングエントリの宛先シーケンス番号(もし存在し有効であれば)は、
   * 　　インクリメントされる。が存在し、有効であれば、インクリメントされる。
   *  2. ルートエントリーを無効としてマークすることで、エントリーは無効になる。
   *  3. Lifetimeフィールドは、現在の時刻にDELETE_PERIODを加えたものに更新される。
   *  \param unreachable 無効にするルート
   */
  void InvalidateRoutesWithDst (std::map<Ipv4Address, uint32_t> const & unreachable);
  /**
   * ifaceアドレスのインターフェースからすべてのルートを削除する。
   * \param iface インターフェースIPアドレス
   */
  void DeleteAllRoutesFromInterface (Ipv4InterfaceAddress iface);
  /// ルーティングテーブルからすべてのエントリを削除する
  void Clear ()
  {
    m_ipv4AddressEntry.clear ();
  }
  /// すべての古いエントリーを削除し、有効期限が切れている場合は有効なエントリーを無効にする。
  void Purge ();
  /** エントリーを単方向としてマークする
   * (例えば、このネイバーをblacklistTimeout期間の間 "ブラックリスト "に追加する)
   * \param neighbor - 単一方向と仮定される隣接アドレスリンク
   * \param blacklistTimeout - 隣接ノードがブラックリストに登録される時間
   * \return true on success
   */
  bool MarkLinkAsUnidirectional (Ipv4Address neighbor, Time blacklistTimeout);
  /**
   * Print routing table
   * \param stream the output stream
   */
  void Print (Ptr<OutputStreamWrapper> stream) const;

private:
  /// ルーティングテーブル
  //(key,value)のペアで構成される標準的なコンテナで、キーに基づいて対数時間で検索することができる。
  std::map<Ipv4Address, RoutingTableEntry> m_ipv4AddressEntry;
  /// 無効なルートの削除時間
  Time m_badLinkLifetime;
  /**
   * Print() メソッドで使用する、Purge の const バージョン。
   * \param table パージするルーティングテーブルエントリ
   */
  void Purge (std::map<Ipv4Address, RoutingTableEntry> &table) const;
};

}  // namespace aodv
}  // namespace ns3

#endif /* AODV_RTABLE_H */
