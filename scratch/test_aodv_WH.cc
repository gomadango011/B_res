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
 * This is an example script for AODV manet routing protocol. 
 *
 * Authors: Pavel Boyko <boyko@iitp.ru>
 */

#include <iostream>
#include <cmath>
#include "ns3/aodv-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/v4ping-helper.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/flow-monitor-module.h"
//追加部分
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/netanim-module.h"
#include "//home/horie/workplace_exist/ns-3-allinone/ns-3.30/src/wifi/helper/wifi-mac-helper.h"
#include "myapp.h"
#include "/home/horie/workplace_exist/ns-3-allinone/ns-3.30/src/wifi/model/yans-wifi-phy.h"

using namespace ns3;

/**
 * \ingroup aodv-examples
 * \ingroup examples
 * \brief Test script.
 * 
 * This script creates 1-dimensional grid topology and then ping last node from the first one:
 * 
 * [10.0.0.1] <-- step --> [10.0.0.2] <-- step --> [10.0.0.3] <-- step --> [10.0.0.4]
 * 
 * ping 10.0.0.4
 *
 * When 1/3 of simulation time has elapsed, one of the nodes is moved out of
 * range, thereby breaking the topology.  By default, this will result in
 * only 34 of 100 pings being received.  If the step size is reduced
 * to cover the gap, then all pings can be received.
 */
class AodvExample 
{
public:
  AodvExample ();
  /**
   * \brief Configure script parameters
   * \param argc is the command line argument count
   * \param argv is the command line arguments
   * \return true on successful configuration
  */
  bool Configure (int argc, char **argv);
  /// Run simulation
  void Run ();
  /**
   * Report results
   * \param os the output stream
   */
  void Report (std::ostream & os);

private:

  // parameters
  /// Number of nodes
  uint32_t size;
  // parameters
  /// Number of around nodes
  uint32_t size_a;
  /// Distance between nodes, meters
  double step;
  /// Simulation time, seconds
  double totalTime;
  /// Write per-device PCAP traces if true
  bool pcap;
  /// Print routes if true
  bool printRoutes;
  
  bool enableFlowMonitor = false;

  std::string phyMode;

  // network
  /// すべてのノード
  NodeContainer nodes;

  //追加部分
  //悪意の無いノード
  NodeContainer not_malicious;
  //悪意のあるノード
  NodeContainer malicious;

  /// devices used in the example
  NetDeviceContainer devices, mal_devices;
  /// interfaces used in the example
  Ipv4InterfaceContainer interfaces;

private:
  /// Create the nodes
  void CreateNodes ();
  /// Create the devices
  void CreateDevices ();
  /// Create the network
  void InstallInternetStack ();
  /// Create the simulation applications
  void InstallApplications ();
};

//追加部分
void
ReceivePacket(Ptr<const Packet> p, const Address & addr)
{
	std::cout << Simulator::Now ().GetSeconds () << "\t" << p->GetSize() <<"\n";
}

int main (int argc, char **argv)
{
  //追加部分
  std::string phyMode ("DsssRate1Mbps");


  AodvExample test;
  if (!test.Configure (argc, argv))
    NS_FATAL_ERROR ("Configuration failed. Aborted.");

  test.Run ();
  test.Report (std::cout);
  return 0;
}

//-----------------------------------------------------------------------------
AodvExample::AodvExample () :
  size (10),
  size_a (5),
  step (50),
  totalTime (100),
  pcap (true),
  printRoutes (true)
{
}

bool
AodvExample::Configure (int argc, char **argv)
{
  // Enable AODV logs by default. Comment this if too noisy
  LogComponentEnable("AodvRoutingProtocol", LOG_LEVEL_ALL);

  SeedManager::SetSeed (12345);
  CommandLine cmd;

  cmd.AddValue ("pcap", "Write PCAP traces.", pcap);
  cmd.AddValue ("printRoutes", "Print routing table dumps.", printRoutes);
  cmd.AddValue ("size", "Number of nodes.", size);
  cmd.AddValue ("time", "Simulation time, s.", totalTime);
  cmd.AddValue ("step", "Grid step, m", step);

  //追加部分
  cmd.AddValue ("EnableMonitor", "Enable Flow Monitor", enableFlowMonitor);
  cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);

  cmd.Parse (argc, argv);
  return true;
}

void
AodvExample::Run ()
{
//  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", UintegerValue (1)); // enable rts cts all the time.
  CreateNodes ();
  CreateDevices ();
  InstallInternetStack ();
  InstallApplications ();

  std::cout << "Starting simulation for " << totalTime << " s ...\n";

  Simulator::Stop (Seconds (totalTime));

  //追加部分
  FlowMonitorHelper flowMonitor;
  auto monitor = flowMonitor.InstallAll();

  monitor->CheckForLostPackets ();

  Simulator::Run ();
  Simulator::Destroy ();
}

void
AodvExample::Report (std::ostream &)
{ 
}

void
AodvExample::CreateNodes ()
{

  printf("ルートノード作製\n");
  //ルートノードの作製
  std::cout << "Creating " << (unsigned)size << " nodes " << step << " m apart.\n";
  printf("Create\n");
  nodes.Create (10);
  // Name nodes
  printf("名前作製\n");
  for (uint32_t i = 0; i < size; ++i)
    {
      std::ostringstream os;
      os << "node-" << i;
      Names::Add (os.str (), nodes.Get (i));
    }

  //ランダムにノーードを配置
  

  printf("ノード作製開始\n");
  not_malicious.Add(nodes.Get(0));
  not_malicious.Add(nodes.Get(1)); //W1
  not_malicious.Add(nodes.Get(2)); //W2
  not_malicious.Add(nodes.Get(4));
  not_malicious.Add(nodes.Get(6));
  not_malicious.Add(nodes.Get(7));
  not_malicious.Add(nodes.Get(8));
  not_malicious.Add(nodes.Get(9));
  malicious.Add(nodes.Get(3));
  malicious.Add(nodes.Get(5));
  
  printf("座標確定\n");
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject <ListPositionAllocator>();
  positionAlloc ->Add(Vector(0, 0, 0)); // node0
  positionAlloc ->Add(Vector(200, 0, 0)); // node1
  positionAlloc ->Add(Vector(400, 0, 0)); // node2
  positionAlloc ->Add(Vector(-100, 200, 0)); // node3 WH
  positionAlloc ->Add(Vector(300, 200,0)); // node4
  positionAlloc ->Add(Vector(700, 200, 0)); // node5 WH
  positionAlloc ->Add(Vector(100, -200, 0)); // node6
  positionAlloc ->Add(Vector(300, -10, 0)); // node7
  positionAlloc ->Add(Vector(500, -10, 0)); // node8
  positionAlloc ->Add(Vector(600, 0, 0)); //dst 9
  // positionAlloc ->Add(Vector(200, 0, 0)); // node2
  // positionAlloc ->Add(Vector(25, 25, 0)); // node2
  // positionAlloc ->Add(Vector(75, 25, 0)); // node2
  mobility.SetPositionAllocator(positionAlloc);
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(nodes);

  printf("ノードに割当\n");
  AnimationInterface anim ("wormhole.xml"); // Mandatory
  AnimationInterface::SetConstantPosition (nodes.Get (0), 0, 0);
  AnimationInterface::SetConstantPosition (nodes.Get (1), 200, 0);
  AnimationInterface::SetConstantPosition (nodes.Get (2), 400, 0);
  AnimationInterface::SetConstantPosition (nodes.Get (3), -100, 200);  //WH
  AnimationInterface::SetConstantPosition (nodes.Get (4), 300, 200);
  AnimationInterface::SetConstantPosition (nodes.Get (5), 700, 200); //WH 
  AnimationInterface::SetConstantPosition (nodes.Get (6), 100, -200);
  AnimationInterface::SetConstantPosition (nodes.Get (7), 300, -200);
  AnimationInterface::SetConstantPosition (nodes.Get (8), 500, -200);
  AnimationInterface::SetConstantPosition (nodes.Get (9), 600, 0);

  printf("EnablePacketMetadat\n");
  anim.EnablePacketMetadata(true);

  printf("routingStream\n");
  Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("wormhole.routes", std::ios::out);
  // aodv.PrintRoutingTableAllAt (Seconds (45), routingStream);

  printf("ConnectWithoutContext\n");
  // Trace Received Packets
  Config::ConnectWithoutContext("/NodeList/*/ApplicationList/*/$ns3::PacketSink/Rx", MakeCallback (&ReceivePacket));

}

void
AodvExample::CreateDevices ()
{
  printf("CreateDevices開始\n");
  WifiMacHelper wifiMac;
  wifiMac.SetType ("ns3::AdhocWifiMac");
  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);
  //YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();

  YansWifiChannelHelper wifiChannel ;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::TwoRayGroundPropagationLossModel",
	  	  	  	  	  	  	  	    "SystemLoss", DoubleValue(1),
		  	  	  	  	  	  	    "HeightAboveZ", DoubleValue(1.5));

  // For range near 250m
  wifiPhy.Set ("TxPowerStart", DoubleValue(33));
  wifiPhy.Set ("TxPowerEnd", DoubleValue(33));
  wifiPhy.Set ("TxPowerLevels", UintegerValue(1));
  wifiPhy.Set ("TxGain", DoubleValue(0));
  wifiPhy.Set ("RxGain", DoubleValue(0));
  // wifiPhy.Set ("EnergyDetectionThreshold", DoubleValue(-61.8));
  // wifiPhy.Set ("CcaMode1Threshold", DoubleValue(-64.8));

  

  wifiPhy.SetChannel (wifiChannel.Create ());
  WifiHelper wifi;
  // Set 802.11b standard
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
  // wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("OfdmRate6Mbps"), "RtsCtsThreshold", UintegerValue (0));
  
  std::string phyMode ("DsssRate1Mbps");
  
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue(phyMode),
                                "ControlMode",StringValue(phyMode));
  devices = wifi.Install (wifiPhy, wifiMac, nodes); 
  mal_devices = wifi.Install(wifiPhy, wifiMac, malicious);

  if (pcap)
    {
      wifiPhy.EnablePcapAll (std::string ("aodv"));
    }

  printf("Create Device 終了\n");

}

void
AodvExample::InstallInternetStack ()
{

  printf("InstallInternetStack開始\n");

  AodvHelper aodv;
  AodvHelper malicious_aodv; 
  // you can configure AODV attributes here using aodv.Set(name, value)
  InternetStackHelper stack;
  stack.SetRoutingHelper (aodv); // has effect on the next Install ()
  stack.Install (not_malicious);

  malicious_aodv.Set("EnableWrmAttack",BooleanValue(true)); // putting *false* instead of *true* would disable the malicious behavior of the node

  malicious_aodv.Set("FirstEndWifiWormTunnel",Ipv4AddressValue("10.0.1.1"));
  malicious_aodv.Set("FirstEndWifiWormTunnel",Ipv4AddressValue("10.0.1.2"));

  stack.SetRoutingHelper (malicious_aodv);
  stack.Install (malicious);


  Ipv4AddressHelper address;
  address.SetBase ("10.0.0.0", "255.0.0.0","0.0.0.1");
  interfaces = address.Assign (devices);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer mal_ifcont = address.Assign (mal_devices);

  if (printRoutes)
    {
      Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("aodv.routes", std::ios::out);
      aodv.PrintRoutingTableAllAt (Seconds (8), routingStream);
    }

  printf("InstallInternetStack終了\n");
  
}

void
AodvExample::InstallApplications ()
{

  printf("InstallApplications開始\n");

  // V4PingHelper ping (interfaces.GetAddress (size - 1));
  // ping.SetAttribute ("Verbose", BooleanValue (true));

  uint16_t sinkPort = size;
  Address sinkAddress (InetSocketAddress (interfaces.GetAddress (9), sinkPort)); // interface of n3
  PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), sinkPort));
  ApplicationContainer sinkApps = packetSinkHelper.Install (nodes.Get (9)); //n3 as sink
  sinkApps.Start (Seconds (0.));
  sinkApps.Stop (Seconds (100.));

  Ptr<Socket> ns3UdpSocket = Socket::CreateSocket (nodes.Get (0), UdpSocketFactory::GetTypeId ()); //source at n0

  // Create UDP application at n0
  Ptr<MyApp> app = CreateObject<MyApp> ();
  app->Setup (ns3UdpSocket, sinkAddress, 1040, 5, DataRate ("250Kbps"));
  nodes.Get (0)->AddApplication (app);
  app->SetStartTime (Seconds (40.));
  app->SetStopTime (Seconds (100.));

  // ApplicationContainer sinkApps = packetSinkHelper.Install (c.Get (3)); //n3 as sink
  // sinkApps.Start (Seconds (0.));
  // sinkApps.Stop (Seconds (100.));

  // ApplicationContainer p = ping.Install (nodes.Get (0));
  // p.Start (Seconds (0));
  // p.Stop (Seconds (totalTime) - Seconds (0.001));

  // move node away
  Ptr<Node> node = nodes.Get (size/2);
  Ptr<MobilityModel> mob = node->GetObject<MobilityModel> ();
  Simulator::Schedule (Seconds (totalTime/3), &MobilityModel::SetPosition, mob, Vector (1e5, 1e5, 1e5));
  printf("InstallApplications終了\n");
}

