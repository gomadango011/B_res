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
#include "myapp.h"
//#include "/home/horie/workplace_exist/ns-3-allinone/ns-3.30/build/ns3/trace-helper.h"
#include <random>
#include "ns3/udp-echo-helper.h"
#include <iostream>
#include <fstream>

//namespace fs = std::filesystem;
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

  //保存用のファイルを返す関数
  std::string GetResultFile() const { return result_file; }

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

  //結果を保存するファイル
  std::string result_file;

  //結果を保存するモード
  int result_mode;

  //WHリンクの長さ
  int WH_size;

  //検知待機時間
  double wait_time;

  //エンド間の距離
  int end_distance;

  //追加部分
  AodvHelper aodv;
  PointToPointHelper point;

  YansWifiPhyHelper wifiPhy;

  // network
  /// nodes used in the example
  
 
  //追加部分
  NodeContainer not_malicious;
  NodeContainer malicious;
  //ここまで

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

std::string def = "p-log/packet_num.txt"; // コピー先ファイル
int counter = 1;
std::string newFilename;
std::string p_file;

std::string filename;

std::string GenerateUniqueFilename(const std::string &f) {
    // if (!fs::exists(filename)) {
    //     return filename; // ファイルが存在しない場合はそのまま使用
    // }

    std::ifstream file(f);
    if(!file.is_open())
  {
    return f;
  }

  std::string baseName = f.substr(0, f.find_last_of('.'));
  std::string extension = f.substr(f.find_last_of('.'));
  std::string newFilename;
    
    // do {
    //     newFilename = baseName + "_" + std::to_string(counter) + extension;
    //     counter++;
    // } while (fs::exists(newFilename)); // 新しい名前が存在しなくなるまで繰り返す

     while(counter <= 100)
  {
    newFilename = baseName + "_" + std::to_string(counter) + extension;
    std::ifstream newfile(newFilename);

  if(!newfile.is_open())
  {
    break;
  }

    counter++;
        
  }

    //std::cout << node->Getfile() << std::endl;

    return newFilename;
}

void
ReceivePacket(Ptr<const Packet> p, const Address & addr)
{
	std::cout << Simulator::Now ().GetSeconds () << "\t" << p->GetSize() <<"\n";
}

NodeContainer nodes;

int main (int argc, char **argv)
{

  AodvExample test;
  if (!test.Configure (argc, argv))
  {
    NS_FATAL_ERROR ("Configuration failed. Aborted.");
  }

  test.Run ();

  //----------------------   ログ取得   --------------------

  //各メッセージのバイト数を保存する変数
  double RREQ_num = 0;
  double RREP_num = 0;
  double WHD_Message_num = 0;
  double WHR_Message_num = 0;

  ///通常ノードを対象とした判定回数
  double NJ_num = 0;
  //WHノードを対象とした判定回数
  double WHJ_num = 0;
  //WHノードを誤検知した回数
  double WHDM_num = 0;
  //正常なノードをWHノードと誤検知した回数
  double DM_num = 0;
  //経路作成時間の合計
  Time RT_num = Time(0);
  //経路作成時間を計測した回数
  int time_count = 0;

  Time min_time = Time(0);
  Time max_time = Time(0);


    //ログ取得
    // 各ノードのAODVルーティングプロトコルインスタンスを取得し、トレースを設定
    for (NodeContainer::Iterator it = nodes.Begin(); it != nodes.End(); ++it)
    {
        Ptr<Node> node = *it;
        RREQ_num = RREQ_num + node->GetRREQ();
        RREP_num = RREP_num + node->GetRREP();
        WHD_Message_num = WHD_Message_num + node->GetWHC();
        WHR_Message_num = WHR_Message_num + node->GetWHE();
        //DC_num = DC_num + node->GetDetCount();   //検知を行った回数

         //通常ノードを対象とした検知回数
        NJ_num = NJ_num + node->Get_Nomal_Node_Judge_Count();
        //WHノードを対象とした検知回数
        WHJ_num = WHJ_num + node->Get_WHJudge_Count();
        //WHノードを正常ノードとご検知した回数
        WHDM_num = WHDM_num + node->Get_WHDetection_miss_Count();

        std::vector<uint32_t> send_id = node->GetSendID();
        std::vector<uint32_t> recv_id = node->GetRecvID();

        //送信したIDと受信したIDを比較し，受信IDが存在しなかった場合，正常ノードを誤検知したと判定
        for(size_t i = 0; i < send_id.size(); ++i)
        {
            auto find = std::find(recv_id.begin(), recv_id.end(), send_id[i]);

            if(find == recv_id.end() /*&& i != send_id.size() - 1*/)
            {
                //送信したIDのメッセージを受信していない場合、誤検知としてカウント
                DM_num = DM_num + 1;
            }
        }

        //経路作成時間の合計を取得
        for(size_t i = 0; i < node->Get_Routing_Time().size(); i++)
        {
            RT_num = RT_num + node->Get_Routing_Time()[i];

            //最小の経路作成時間を取得
            if(min_time == Time(0))
            {
                min_time = node->Get_Routing_Time()[i];
            }
            else
            {
                min_time = std::min(min_time, node->Get_Routing_Time()[i]);
            }

            //最大の経路作成時間を取得
            if(max_time == Time(0))
            {
                max_time = node->Get_Routing_Time()[i];
            }
            else
            {
                max_time = std::max(max_time, node->Get_Routing_Time()[i]);
            }
        }

        //経路作成時間を計測した回数
        time_count = time_count + node->Get_Routing_Time_Count();
    }

    //結果を保存するためのファイルに書き込み
    std::ofstream ofs(test.GetResultFile(), std::ios::trunc);

    if (!ofs) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

    ofs << "RREQの合計バイト数：" << RREQ_num << std::endl;
    ofs << "RREPの合計バイト数：" << RREP_num << std::endl;
    ofs << "WHDの合計バイト数：" << WHD_Message_num << std::endl;
    ofs << "WHRの合計バイト数：" << WHR_Message_num  << "\n" << std::endl;

    ofs << "通常ノードを対象とした判定回数：" << NJ_num << std::endl;
    ofs << "WHノードを対象とした判定回数：" << WHJ_num << std::endl;
    ofs << "WHノードを正常に検知した回数：" << WHJ_num - WHDM_num << std::endl;
    ofs << "正常なノードをWHノードと誤検知した回数：" << DM_num << std::endl;
    ofs << "経路作成時間の合計：" << RT_num.GetSeconds() << std::endl;
    ofs << "経路作成時間を計測した回数：" << time_count << "\n" << std::endl;

    ofs << "---------------------------------------------------------------\n" << std::endl;

    ofs << "WH攻撃の検知率："<< (WHJ_num - WHDM_num) / WHJ_num << std::endl;
    ofs << "通常ノードをWH攻撃と誤検知した割合：" << DM_num / NJ_num << std::endl;
    ofs << "1回の判定にかかる検知コスト：" << (WHD_Message_num + WHR_Message_num) / (NJ_num + WHJ_num) << std::endl;

    if(time_count == 0)
    {
        ofs << "RREPがとどいていません。" << std::endl;
    }
    else
    {
        ofs << "経路作成時間の平均：" << RT_num.GetSeconds() / time_count << std::endl;
        ofs << "経路作成時間の最小値：" << min_time << std::endl;
        ofs << "経路作成時間の最大値：" << max_time << std::endl;
    }


    //p_size << "シード値" << rand << std::endl;

    ofs.close();

  test.Report (std::cout);
  return 0;
}

//-----------------------------------------------------------------------------
AodvExample::AodvExample () :
  size (300),
  size_a (5),
  step (50),
  totalTime (40),
  pcap (true),
  printRoutes (false),
  result_file("deff/p-log2.txt"), //結果を保存するファイル
  WH_size(250),
  wait_time(0.5), //検知待機時間
  end_distance(500) //エンド間の距離
{
}

bool
AodvExample::Configure (int argc, char **argv)
{
  // Enable AODV logs by default. Comment this if too noisy
  // LogComponentEnable("AodvRoutingProtocol", LOG_LEVEL_ALL);
  // LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_ALL);
  // LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_ALL);

  std::random_device randomseed;
  int rand = randomseed();
  // SeedManager::SetSeed (rand);

  SeedManager::SetSeed (rand);


  std::ofstream p_size(filename,std::ios::app);

  p_size << "シード値" << counter << std::endl;
  CommandLine cmd;

  cmd.AddValue ("pcap", "Write PCAP traces.", pcap);
  cmd.AddValue ("printRoutes", "Print routing table dumps.", printRoutes);
  cmd.AddValue ("size", "Number of nodes.", size);
  cmd.AddValue ("time", "Simulation time, s.", totalTime);
  cmd.AddValue ("step", "Grid step, m", step);

  cmd.AddValue("result_file", "result file", result_file); //結果表示ようのファイル名を取得
  cmd.AddValue("WH_size", "WH size", WH_size); //WHの長さ
  cmd.AddValue("wait_time", "Detection wait time", wait_time); //検知待機時間
  cmd.AddValue("end_distance", "end distance", end_distance); //エンド間の距離

  if(end_distance < WH_size + 100)
  {
    std::cerr << "エンド間の距離がWHリンクの長さよりも短いです。" << std::endl;
    return false;
  }

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
  //FlowMonitorHelper flowMonitor;
  // auto monitor = flowMonitor.InstallAll();

  //追加部分
  //AsciiTraceHelper ascii;
  // aodv.EnablePcapAll ("test_aodv");
  //point.EnableAsciiAll (ascii.CreateFileStream("test_point.tr"));

   //traceファイルの設定
    // AsciiTraceHelper ascii;
    // Ptr<OutputStreamWrapper> stream;  // stream:=(ファイルストリーム).

    // // 初回のときにストリーム作成．
    // if(!stream) {
    //     std::string filename = "packet1.tr";
    //     stream = ascii.CreateFileStream(filename,std::ios::app);
    // }

    // //trに書き込む
    // *stream->GetStream()<< Node_ID<<" "<<sourceid << " "<<sourcetime<<" "<<sourcepos.x <<" "<<sourcepos.y<<" "<<beforehopid <<" "<<beforehoptime << " " <<beforehoppos.x <<" " << beforehoppos.y <<" " <<senderid << " "<<sendertime<<" " << senderpos.x << " "<<senderpos.y << " "<< msg <<std::endl;     // パケットの中身．

    // Packet::EnableChecking();
    // Packet::EnablePrinting();

  //  FlowMonitorHelper flowMonitor;
  //   auto monitor = flowMonitor.InstallAll();

  //   wifiPhy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
  //   wifiPhy.EnablePcapAll(/*m_prefix + */"-packet");
  //   wifiPhy.EnableAsciiAll(/*m_prefix +*/  "-packet");

    // AsciiTraceHelper asc;
    // Ipv4RoutingHelper::PrintRoutingTableAllEvery(Seconds(1.0), asc.CreateFileStream(/*m_prefix + */"-rtable.tr"));

  


  Simulator::Run ();
  Simulator::Destroy ();



  // monitor->CheckForLostPackets ();
  // Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  // FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();
  // for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
  //   {
  //     Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);

  //     std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
  //     std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
  //     std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
  //     std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
  //     std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
  //     std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
  //     std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
  //   }
}

void
AodvExample::Report (std::ostream &)
{ 
}

void
AodvExample::CreateNodes ()
{

  //ルートノードの作製
  std::cout << "Creating " << (unsigned)size << " nodes " << step << " m apart.\n";
  nodes.Create (size);
  // Name nodes
  for (uint32_t i = 0; i < size; ++i)
    {
      std::ostringstream os;
      os << "node-" << i;
      Names::Add (os.str (), nodes.Get (i));
    }
  // Create static grid
  // MobilityHelper mobility;
  // mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
  //                                "MinX", DoubleValue (0.0),
  //                                "MinY", DoubleValue (0.0),
  //                                "DeltaX", DoubleValue (step),
  //                                "DeltaY", DoubleValue (10000),
  //                                "GridWidth", UintegerValue (size),
  //                                "LayoutType", StringValue ("RowFirst"));
  // mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  // mobility.Install (nodes);

 MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::RandomRectanglePositionAllocator",
                                  "X", StringValue("ns3::UniformRandomVariable[Min=0|Max=500]"),
                                  "Y", StringValue("ns3::UniformRandomVariable[Min=0|Max=500]")
                                 ); 
  
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);
  
//   not_malicious.Add(nodes.Get(0));        //src
//   not_malicious.Add(nodes.Get(size-1));  //dst
  // not_malicious.Add(nodes.Get(3));
  // not_malicious.Add(nodes.Get(4));
  // not_malicious.Add(nodes.Get(5));
//   malicious.Add(nodes.Get(1)); //WH1
//   malicious.Add(nodes.Get(2));//WH2

   AnimationInterface anim ("wormhole.xml"); // Mandatory
  AnimationInterface::SetConstantPosition (nodes.Get (0), 0, 250);
  AnimationInterface::SetConstantPosition (nodes.Get (size-1), 500, 250);

  //WHノードを配置
  //AnimationInterface::SetConstantPosition (nodes.Get (1), 280, 280);
  AnimationInterface::SetConstantPosition (nodes.Get (1), 100, 250);
  AnimationInterface::SetConstantPosition (nodes.Get (2), 350, 250);

  malicious.Add(nodes.Get(1)); //WH1
  malicious.Add(nodes.Get(2));//WH2
  
  anim.EnablePacketMetadata(true);

}

void
AodvExample::CreateDevices ()
{
  WifiMacHelper wifiMac;
  wifiMac.SetType ("ns3::AdhocWifiMac");
  wifiPhy = YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  wifiPhy.SetChannel (wifiChannel.Create ());

  //送信電力と受信電力を設定
  //送信電力と受信電力を設定
  wifiPhy.Set("TxPowerStart", DoubleValue(24.7)); // 送信電力 20 dBm
  wifiPhy.Set("TxPowerEnd", DoubleValue(24.7));

  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("OfdmRate6Mbps"), "RtsCtsThreshold", UintegerValue (0));
  devices = wifi.Install (wifiPhy, wifiMac, nodes); 

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  // NetDeviceContainer devices;
  mal_devices = pointToPoint.Install (malicious);

  if (pcap)
    {
      wifiPhy.EnablePcapAll (std::string ("aodv"));
      pointToPoint.EnablePcapAll (std::string ("point-to-point"));
    }
}

void
AodvExample::InstallInternetStack ()
{

  // you can configure AODV attributes here using aodv.Set(name, value)
  InternetStackHelper stack;
  stack.SetRoutingHelper (aodv); // has effect on the next Install ()
  stack.Install (nodes);

  InternetStackHelper stack2;
  //IDstack2.Install (malicious);
  Ipv4AddressHelper address;
  address.SetBase ("10.0.0.0", "255.0.0.0","0.0.0.1");
  interfaces = address.Assign (devices);

   address.SetBase ("10.1.2.0", "255.255.255.0", "0.0.0.1");
   Ipv4InterfaceContainer mal_ifcont = address.Assign (mal_devices);

  if (printRoutes)
    {
      Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("aodv.routes", std::ios::out);
      aodv.PrintRoutingTableAllAt (Seconds (8), routingStream);
    }
}

void
AodvExample::InstallApplications ()
{
  // UdpEchohelper udpecho (interfaces.GetAddress (size - 1));
  // udpecho.SetAttribute ("Verbode", BooleanValue (true));
  
  // ApplicationContainer u = udpecho.Install (nodes.Get (0));
  // p.Start (Seconds (0));
  // p.Stop (Seconds (totalTime) - Seconds (0.001));

  V4PingHelper ping (interfaces.GetAddress (size - 1));
  ping.SetAttribute ("Verbose", BooleanValue (true));

  ApplicationContainer p = ping.Install (nodes.Get (0));
  p.Start (Seconds (0));
  p.Stop (Seconds (totalTime) - Seconds (0.001));

  // move node away
  Ptr<Node> node = nodes.Get (size/2);
  Ptr<MobilityModel> mob = node->GetObject<MobilityModel> ();
  Simulator::Schedule (Seconds (totalTime/3), &MobilityModel::SetPosition, mob, Vector (1e5, 1e5, 1e5));
}

