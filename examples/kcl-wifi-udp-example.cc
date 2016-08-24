/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/emu-fd-net-device-helper.h"
#include "ns3/haptic-operator-helper.h"
#include "ns3/haptic-tele-operator-helper.h"
#include "ns3/chai3d-server-helper.h"
#include "ns3/phantom-agent-helper.h"
// Default Network Topology
//
// Number of wifi or csma nodes can be increased up to 250
//                          |
//                 Rank 0   |   Rank 1
// -------------------------|----------------------------
//   Wifi 10.1.3.0
//                 AP
//  *    *    *    *
//  |    |    |    |    10.1.1.0
// n5   n6   n7   n0 -------------- n1   n2   n3   n4
//                   point-to-point  |    |    |    |
//                                   ================
//                                     LAN 10.1.2.0

// Idea and code for setting up the FdNetDevice which is used by the PhantomAgent 
// is based on: https://www2.nsnam.org/doxygen/fd-emu-onoff_8cc_source.html

// Idea and code for setting up the network topology is based on: 
// https://www.nsnam.org/doxygen/third_8cc_source.html

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("KclWifiExample");

int 
main (int argc, char *argv[])
{
  bool verbose = false;
  uint32_t nCsma = 3;
  uint32_t nWifi = 3;
  bool tracing = false;

  GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));
  GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));

  CommandLine cmd;
  cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

  cmd.Parse (argc,argv);

  // Check for valid number of csma or wifi nodes
  // 250 should be enough, otherwise IP addresses 
  // soon become an issue
  if (nWifi > 250 || nCsma > 250)
    {
      std::cout << "Too many wifi or csma nodes, no more than 250 each." << std::endl;
      return 1;
    }

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("500Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("1ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  NodeContainer csmaNodes;
  csmaNodes.Add (p2pNodes.Get (1));
  csmaNodes.Create (nCsma);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);

  /////////////////////////////////////////////////////////////////////
  std::string netmask ("255.255.255.0");
  Ipv4Mask localMask (netmask.c_str ());

  std::string server ("172.16.25.125");
  Ipv4Address localIp;
  localIp = Ipv4Address (server.c_str ());

  std::string macServer ("80:fa:5b:1c:01:49");
  Mac48AddressValue localMac;
  localMac = Mac48AddressValue (macServer.c_str ());



  NS_LOG_INFO ("Create Device");
  EmuFdNetDeviceHelper emu;
  emu.SetDeviceName ("eth0");
  NetDeviceContainer devices = emu.Install (csmaNodes.Get(0));

  Ptr<NetDevice> device = devices.Get (0);
  device->SetAttribute ("Address", localMac);

  NS_LOG_INFO ("Add Internet Stack");
  InternetStackHelper internetStackHelper;
  internetStackHelper.SetIpv4StackInstall(true);
  internetStackHelper.Install (csmaNodes.Get(0));

  NS_LOG_INFO ("Create IPv4 Interface");
  Ptr<Ipv4> ipv4 = csmaNodes.Get(0)->GetObject<Ipv4> ();
  uint32_t interface = ipv4->AddInterface (device);
  Ipv4InterfaceAddress phantomAddress = Ipv4InterfaceAddress (localIp, localMask);
  ipv4->AddAddress (interface, phantomAddress);
  ipv4->SetMetric (interface, 1);
  ipv4->SetUp (interface);
  /////////////////////////////////////////////////////////////////////

  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nWifi);
  NodeContainer wifiApNode = p2pNodes.Get (0);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());

  WifiHelper wifi = WifiHelper::Default ();
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

  NqosWifiMacHelper mac = NqosWifiMacHelper::Default ();

  Ssid ssid = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, mac, wifiStaNodes);

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, mac, wifiApNode);

  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility.Install (wifiStaNodes);

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode);

  InternetStackHelper stack;
  stack.Install (csmaNodes.Get(1));
  stack.Install (csmaNodes.Get(2));
  stack.Install (csmaNodes.Get(3));

  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);

  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer staInterfaces = address.Assign (staDevices);
  address.Assign (apDevices);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  ////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////



  /////////////////////////////////////////////////////////////
  //
  //	Application layer
  //
  /////////////////////////////////////////////////////////////

	//
	// Create a Chai3dServer application on node one.
	//

  uint16_t port = 4444;  // well-known echo port number
  Chai3dServerHelper chai3dServer (port,Address(Ipv4Address("10.1.2.1")),1234);
  chai3dServer.SetAttribute ("Chai3dWrapper", StringValue ("/home/matthias/Development/chai3d-3.0.0/bin/04-shapes"));
  ApplicationContainer apps = chai3dServer.Install (wifiStaNodes.Get(2));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (140.0));


  PhantomAgentHelper phantomAgent(localIp,1234);
  phantomAgent.SetAttribute("Chai3dWrapperAddress", AddressValue( staInterfaces.GetAddress(2)));
  phantomAgent.SetAttribute("Chai3dWrapperPort",UintegerValue( port));
  ApplicationContainer chai3Dapps = phantomAgent.Install(csmaNodes.Get (0));
  chai3Dapps.Start(Seconds(1.0));
  chai3Dapps.Stop(Seconds(120));

//  HapticOperatorHelper client (staInterfaces.GetAddress(2), port);
//  client.SetAttribute ("PositionFile", StringValue ("src/Kcl-Haptic-Sim/test/position.txt"));
//  client.SetAttribute ("VelocityFile", StringValue ("src/Kcl-Haptic-Sim/test/fakeVelocity.txt"));
//  client.SetAttribute ("SamplingIntervalSeconds", DoubleValue (0.001));
//  //client.SetAttribute ("ApplyDataReduction", BooleanValue(true));
//  //client.SetAttribute ("FileType", StringValue ("POSITION"));
//  apps = client.Install (csmaNodes.Get (0));
//  apps.Start (Seconds (0.02));
//  apps.Stop (Seconds (30.0));

  /////////////////////////////////////////////////////



  Simulator::Stop (Seconds (100.0));

  if (tracing == true)
    {
      pointToPoint.EnablePcapAll ("third");
      phy.EnablePcap ("third", apDevices.Get (0));
      csma.EnablePcap ("third", csmaDevices.Get (0), true);
    }

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
