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

// Idea and code to create the wifi + ethernet network is based on:
// https://www2.nsnam.org/doxygen/third_8cc_source.html

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
#include "ns3/flow-monitor-module.h"
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

NS_LOG_COMPONENT_DEFINE ("KclWifiErrorModelExample");

int 
main (int argc, char *argv[])
{
	  bool verbose = true;
	  uint32_t nCsma = 3;
	  uint32_t nWifi = 3;
	  bool tracing = false;

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
	  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
	  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

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
	  stack.Install (csmaNodes);
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
	  address.Assign (staDevices);
	  address.Assign (apDevices);

	  ////////////////////////////////////////////////////////////////////////////////
	  //
	  //	Application layer
	  //
	  ////////////////////////////////////////////////////////////////////////////////

//	  UdpEchoServerHelper echoServer (9);
//
//	  ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (nCsma));
//	  serverApps.Start (Seconds (1.0));
//	  serverApps.Stop (Seconds (10.0));
//
//	  UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (nCsma), 9);
//	  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
//	  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
//	  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
//
//	  ApplicationContainer clientApps =
//	    echoClient.Install (wifiStaNodes.Get (nWifi - 1));
//	  clientApps.Start (Seconds (2.0));
//	  clientApps.Stop (Seconds (10.0));


		//
		// Create a HapticTeleOperator application on node one.
		//
		  double interPacketInterval = 0.001;
		  uint16_t port = 9;  // well-known echo port number
		  HapticTeleOperatorHelper server (port);
		  server.SetAttribute("FileName", StringValue ("src/Kcl-Haptic-Sim/test/test_force.txt"));
		  server.SetAttribute ("SamplingIntervalSeconds", DoubleValue (interPacketInterval));
		  server.SetAttribute ("ApplyDataReduction", BooleanValue (true));
		  ApplicationContainer apps = server.Install (csmaNodes.Get (nCsma));
		  apps.Start (Seconds (1.0));
		  apps.Stop (Seconds (3.0));



		//
		// Create a HapticOperator application to send UDP datagrams from node zero to
		// node one.
		//


		  HapticOperatorHelper client (csmaInterfaces.GetAddress (nCsma), port);
		  client.SetAttribute ("SamplingIntervalSeconds", DoubleValue (interPacketInterval));
		  client.SetAttribute ("PositionFile", StringValue ("src/Kcl-Haptic-Sim/test/test_pos.txt"));
		  client.SetAttribute ("VelocityFile", StringValue ("src/Kcl-Haptic-Sim/test/fakeVelocity.txt"));
		  server.SetAttribute ("ApplyDataReduction", BooleanValue (true));
		  apps = client.Install (wifiStaNodes.Get (nWifi - 1));
		  apps.Start (Seconds (2.0));
		  apps.Stop (Seconds (3.0));

		  Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
		  em->SetAttribute ("ErrorRate", DoubleValue (0.001));
		  csmaDevices.Get (nCsma)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

	  ////////////////////////////////////////////////////////////////////////////////
	  //
	  //	Monitoring
	  //
	  ////////////////////////////////////////////////////////////////////////////////

	  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	  // Flow monitor
	  Ptr<FlowMonitor> flowMonitor;
	  FlowMonitorHelper flowHelper;
	  NodeContainer nc;
	  nc.Add(wifiStaNodes.Get (nWifi - 1));
	  nc.Add(csmaNodes.Get (nCsma));
	  flowMonitor = flowHelper.Install(nc);

	  Simulator::Stop (Seconds (10.0));

	  if (tracing == true)
	    {
	      pointToPoint.EnablePcapAll ("third");
	      phy.EnablePcap ("third", apDevices.Get (0));
	      csma.EnablePcap ("third", csmaDevices.Get (0), true);
	    }

	  Simulator::Run ();

	  flowMonitor->CheckForLostPackets(Seconds(0));
	  flowMonitor->SerializeToXmlFile("msc-demo.xml", true, true);
	  Simulator::Destroy ();
	  return 0;
}
