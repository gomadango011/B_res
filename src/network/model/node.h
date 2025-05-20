/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2006 Georgia Tech Research Corporation, INRIA
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
 * Authors: George F. Riley<riley@ece.gatech.edu>
 *          Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#ifndef NODE_H
#define NODE_H

#include <vector>

#include "ns3/object.h"
#include "ns3/callback.h"
#include "ns3/ptr.h"
#include "ns3/net-device.h"

namespace ns3 {

class Application;
class Packet;
class Address;
class Time;


/**
 * \ingroup network
 *
 * \brief A network Node.
 *
 * This class holds together:
 *   - a list of NetDevice objects which represent the network interfaces
 *     of this node which are connected to other Node instances through
 *     Channel instances.
 *   - a list of Application objects which represent the userspace
 *     traffic generation applications which interact with the Node
 *     through the Socket API.
 *   - a node Id: a unique per-node identifier.
 *   - a system Id: a unique Id used for parallel simulations.
 *
 * Every Node created is added to the NodeList automatically.
 */
class Node : public Object
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  Node();
  /**
   * \param systemId a unique integer used for parallel simulations.
   */
  Node(uint32_t systemId);

  virtual ~Node();

  /**
   * \returns the unique id of this node.
   * 
   * This unique id happens to be also the index of the Node into
   * the NodeList. 
   */
  uint32_t GetId (void) const;

  /**
   * In the future, ns3 nodes may have clock that returned a local time
   * different from the virtual time Simulator::Now().
   * This function is currently a placeholder to ease the development of this feature.
   * For now, it is only an alias to Simulator::Now()
   *
   * \return The time as seen by this node
   */
  Time GetLocalTime (void) const;

  /**
   * \returns the system id for parallel simulations associated
   *          to this node.
   */
  uint32_t GetSystemId (void) const;

  //追加部分
    void Setfile(std::string f);

    std::string Getfile() const;

    void SetRREQ(int req);

    int GetRREQ() const;

    void SetRREP(int rep);

    int GetRREP() const;

    void SetWHC(int whc);

    int GetWHC() const;

    void SetWHE(int whe);

    int GetWHE() const;

    void SetDetCount(int cou);

    int GetDetCount();

    /*
      検知率に関するデータ取得用関数
    */

    //通常ノードを判定した回数
    void Set_Nomal_Node_Judge_Count(int de);

    //通常ノードを判定した回数を取得する関数
    int Get_Nomal_Node_Judge_Count() const;

    //WH攻撃を対象とした検知回数
    void Set_WHJudge_Count(int whdc);
    
    int Get_WHJudge_Count() const;

    //WH攻撃を正常に検知した回数
    void Set_WHDetection_miss_Count(int whdm);

    int Get_WHDetection_miss_Count() const;

    //正常なリンクをWHリンクと誤検知した回数
    void Set_Detection_miss_Count(int dm);

    int Get_Detecstion_miss_Count() const;

    //経路作成時間を保持する変数
    void Set_Routing_Time(Time t);

    std::vector<Time> Get_Routing_Time() const;

    //経路作成時間を計測した回数
    void Increment_Routing_Time_Count();

    uint32_t Get_Routing_Time_Count() const;



    // //すべての検知回数を保存する関数
    // void SetDetectionEnd(int de);

    // //すべての検知回数を参照する関数
    // int GetDetectionEnd() const;

    // //WH攻撃を対象とした検知回数
    // void SetWHDetection(int whdc);
    
    // int GetWHDetection() const;

    // //WH攻撃を正常であると検知した回数
    // void SetWHDetection_miss(int whdm);

    // int GetWHDetection_miss() const;

    // //正常なリンクをWHリンクと誤検知した回数
    // void SetDetection_miss(int dm);

    // int GetDetecstion_miss() const;

    // //WHを検知しようとした回数をカウントする変数
    // void SetFlag_Count(int flag);

    // int GetFlag_Count() const;

    //送信したID
    void AddSendID(uint32_t s_id);

    std::vector<uint32_t> GetSendID() const;

    //受信したID
    void AddRecvID(uint32_t s_id);

    std::vector<uint32_t> GetRecvID() const;

  /**
   * \brief Associate a NetDevice to this node.
   *
   * \param device NetDevice to associate to this node.
   * \returns the index of the NetDevice into the Node's list of
   *          NetDevice.
   */
  uint32_t AddDevice (Ptr<NetDevice> device);
  /**
   * \brief Retrieve the index-th NetDevice associated to this node.
   *
   * \param index the index of the requested NetDevice
   * \returns the requested NetDevice.
   */
  Ptr<NetDevice> GetDevice (uint32_t index) const;
  /**
   * \returns the number of NetDevice instances associated
   *          to this Node.
   */
  uint32_t GetNDevices (void) const;

  /**
   * \brief Associate an Application to this Node.
   *
   * \param application Application to associate to this node.
   * \returns the index of the Application within the Node's list
   *          of Application.
   */
  uint32_t AddApplication (Ptr<Application> application);
  /**
   * \brief Retrieve the index-th Application associated to this node.
   *
   * \param index the index of the requested Application
   * \returns the requested Application.
   */
  Ptr<Application> GetApplication (uint32_t index) const;

  /**
   * \returns the number of Application instances associated to this Node.
   */
  uint32_t GetNApplications (void) const;

  /**
   * A protocol handler
   *
   * \param device a pointer to the net device which received the packet
   * \param packet the packet received
   * \param protocol the 16 bit protocol number associated with this packet.
   *        This protocol number is expected to be the same protocol number
   *        given to the Send method by the user on the sender side.
   * \param sender the address of the sender
   * \param receiver the address of the receiver; Note: this value is
   *                 only valid for promiscuous mode protocol
   *                 handlers.  Note:  If the L2 protocol does not use L2
   *                 addresses, the address reported here is the value of 
   *                 device->GetAddress().
   * \param packetType type of packet received
   *                   (broadcast/multicast/unicast/otherhost); Note:
   *                   this value is only valid for promiscuous mode
   *                   protocol handlers.
   */
  typedef Callback<void,Ptr<NetDevice>, Ptr<const Packet>,uint16_t,const Address &,
                   const Address &, NetDevice::PacketType> ProtocolHandler;
  /**
   * \param handler the handler to register
   * \param protocolType the type of protocol this handler is 
   *        interested in. This protocol type is a so-called
   *        EtherType, as registered here:
   *        http://standards.ieee.org/regauth/ethertype/eth.txt
   *        the value zero is interpreted as matching all
   *        protocols.
   * \param device the device attached to this handler. If the
   *        value is zero, the handler is attached to all
   *        devices on this node.
   * \param promiscuous whether to register a promiscuous mode handler
   */
  void RegisterProtocolHandler (ProtocolHandler handler, 
                                uint16_t protocolType,
                                Ptr<NetDevice> device,
                                bool promiscuous=false);
  /**
   * \param handler the handler to unregister
   *
   * After this call returns, the input handler will never
   * be invoked anymore.
   */
  void UnregisterProtocolHandler (ProtocolHandler handler);

  /**
   * A callback invoked whenever a device is added to a node.
   */
  typedef Callback<void,Ptr<NetDevice> > DeviceAdditionListener;
  /**
   * \param listener the listener to add
   *
   * Add a new listener to the list of listeners for the device-added
   * event. When a new listener is added, it is notified of the existence
   * of all already-added devices to make discovery of devices easier.
   */
  void RegisterDeviceAdditionListener (DeviceAdditionListener listener);
  /**
   * \param listener the listener to remove
   *
   * Remove an existing listener from the list of listeners for the 
   * device-added event.
   */
  void UnregisterDeviceAdditionListener (DeviceAdditionListener listener);



  /**
   * \returns true if checksums are enabled, false otherwise.
   */
  static bool ChecksumEnabled (void);


protected:
  /**
   * The dispose method. Subclasses must override this method
   * and must chain up to it by calling Node::DoDispose at the
   * end of their own DoDispose method.
   */
  virtual void DoDispose (void);
  virtual void DoInitialize (void);
private:

  /**
   * \brief Notifies all the DeviceAdditionListener about the new device added.
   * \param device the added device to notify.
   */
  void NotifyDeviceAdded (Ptr<NetDevice> device);

  /**
   * \brief Receive a packet from a device in non-promiscuous mode.
   * \param device the device
   * \param packet the packet
   * \param protocol the protocol
   * \param from the sender
   * \returns true if the packet has been delivered to a protocol handler.
   */
  bool NonPromiscReceiveFromDevice (Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t protocol, const Address &from);
  /**
   * \brief Receive a packet from a device in promiscuous mode.
   * \param device the device
   * \param packet the packet
   * \param protocol the protocol
   * \param from the sender
   * \param to the destination
   * \param packetType the packet type
   * \returns true if the packet has been delivered to a protocol handler.
   */
  bool PromiscReceiveFromDevice (Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t protocol,
                                 const Address &from, const Address &to, NetDevice::PacketType packetType);
  /**
   * \brief Receive a packet from a device.
   * \param device the device
   * \param packet the packet
   * \param protocol the protocol
   * \param from the sender
   * \param to the destination
   * \param packetType the packet type
   * \param promisc true if received in promiscuous mode
   * \returns true if the packet has been delivered to a protocol handler.
   */
  bool ReceiveFromDevice (Ptr<NetDevice> device, Ptr<const Packet>, uint16_t protocol,
                          const Address &from, const Address &to, NetDevice::PacketType packetType, bool promisc);

  /**
   * \brief Finish node's construction by setting the correct node ID.
   */
  void Construct (void);

  /**
   * \brief Protocol handler entry.
   * This structure is used to demultiplex all the protocols.
   */
  struct ProtocolHandlerEntry {
    ProtocolHandler handler; //!< the protocol handler
    Ptr<NetDevice> device;   //!< the NetDevice
    uint16_t protocol;       //!< the protocol number
    bool promiscuous;        //!< true if it is a promiscuous handler
  };

  /// Typedef for protocol handlers container
  typedef std::vector<struct Node::ProtocolHandlerEntry> ProtocolHandlerList;
  /// Typedef for NetDevice addition listeners container
  typedef std::vector<DeviceAdditionListener> DeviceAdditionListenerList;

  uint32_t    m_id;         //!< Node id for this node
  uint32_t    m_sid;        //!< System id for this node
  std::vector<Ptr<NetDevice> > m_devices; //!< Devices associated to this node
  std::vector<Ptr<Application> > m_applications; //!< Applications associated to this node
  ProtocolHandlerList m_handlers; //!< Protocol handlers in the node
  DeviceAdditionListenerList m_deviceAdditionListeners; //!< Device addition listeners in the node

  //追加部分
  int RREQ_num;
  int RREP_num;
  int WHC_num;
  int WHE_num;
  int DC_num;

  //検知率に関する変数
     //通常ノードを判定した回数
    int NJC_num;

    //WH攻撃を対象とした検知回数
    int WHJC_num;

    //WH攻撃を正常に検知した回数
    int WHD_count;

    //正常なリンクをWHリンクと誤検知した回数
    int DM_num;

    //WHを検知しようとした回数をカウントする変数
    int Flag_Count_num;

    //正常にWH攻撃を検知した回数
    int Detection_WH;

    //WHを検知しようとした回数をカウントする変数
    //int Flag_Count_num;

    //送信した検知メッセージのID
    std::vector<uint32_t> send_ID;
    //受信した検知メッセージのID
    std::vector<uint32_t> recv_ID;

    //経路作成時間を保持する変数
    std::vector<Time> m_routing_time;

    //経路作成時間を計測した回数
    uint32_t m_routing_time_count;
};

} // namespace ns3

#endif /* NODE_H */
