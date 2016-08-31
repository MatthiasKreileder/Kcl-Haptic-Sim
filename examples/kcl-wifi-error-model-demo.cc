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
#include "ns3/tcp-haptic-operator-helper.h"
#include "ns3/tcp-haptic-tele-operator-helper.h"

#include "ns3/chai3d-server-helper.h"
#include "ns3/phantom-agent-helper.h"
#include "ns3/flow-monitor-module.h"
#include <string>
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
	int simTime = 10;
	  bool verbose = true;
	  uint32_t nCsma = 3;
	  uint32_t nWifi = 3;
	  bool tracing = false;

	  std::string fileName = "demo.xml";
	  std::string protocol = "UDP";
	  bool applyReduction = false;
	  bool errors = false;


	  CommandLine cmd;
	  cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
	  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
	  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
	  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);
	  cmd.AddValue ("fileName","FlowMonitor's xml output file name",fileName);
	  cmd.AddValue ("protocol","The transport layer protocol for the haptic apps",protocol);
	  cmd.AddValue ("applyReduction","Set to true if you want to apply data reduction",applyReduction);
	  cmd.AddValue ("errors","Enabling this will lead to tx and rx error on the csma net device",errors);

	  cmd.Parse (argc,argv);

	  NS_LOG_DEBUG("Simulation config:");
	  NS_LOG_DEBUG("Transport Layer protocol: " << protocol);
	  if (applyReduction){
		  NS_LOG_DEBUG("Data reduction: enabled");
	  }
	  if (errors){
		  NS_LOG_DEBUG ("Tx/Rx errors on the haptic teleoperator network device: enabled");
	  }

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

	  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

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

	  NS_LOG_DEBUG("Installing a haptic operator on: " << csmaInterfaces.GetAddress(nCsma));
	  Ptr<Ipv4> staNodeIp = wifiStaNodes.Get(nWifi -1)->GetObject<Ipv4>();

	  NS_LOG_DEBUG("Installing a haptic teleoperator on: " << staNodeIp->GetAddress(1,0).GetLocal());

	  if (errors){
		  Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
		  em->SetAttribute ("ErrorRate", DoubleValue (0.01));
		  csmaDevices.Get (nCsma)->SetAttribute ("ReceiveErrorModel", PointerValue (em));
	  }

	  if (protocol.compare("UDP") == 0){
		//
		// Create a HapticTeleOperator application on node one.
		//
		  double interPacketInterval = 0.001;
		  uint16_t port = 9;  // well-known echo port number
		  HapticTeleOperatorHelper server (port);
		  server.SetAttribute("FileName", StringValue ("src/Kcl-Haptic-Sim/test-data/force.txt"));
		  server.SetAttribute ("SamplingIntervalSeconds", DoubleValue (interPacketInterval));
		  server.SetAttribute ("ApplyDataReduction", BooleanValue (applyReduction));
		  ApplicationContainer apps = server.Install (csmaNodes.Get (nCsma));
		  apps.Start (Seconds (1.0));
		  apps.Stop (Seconds (simTime));



		//
		// Create a HapticOperator application to send UDP datagrams from node zero to
		// node one.
		//


		  HapticOperatorHelper client (csmaInterfaces.GetAddress (nCsma), port);
		  client.SetAttribute ("SamplingIntervalSeconds", DoubleValue (interPacketInterval));
		  client.SetAttribute ("PositionFile", StringValue ("src/Kcl-Haptic-Sim/test-data/position.txt"));
		  client.SetAttribute ("VelocityFile", StringValue ("src/Kcl-Haptic-Sim/test-data/velocity.txt"));
		  client.SetAttribute ("ApplyDataReduction", BooleanValue (applyReduction));
		  apps = client.Install (wifiStaNodes.Get (nWifi - 1));
		  apps.Start (Seconds (2.0));
		  apps.Stop (Seconds (simTime));


	  }
	  else if (protocol.compare("TCP") == 0){
		  int hapticPort = 8080;
		  TcpHapticTeleOperatorHelper tcpTeleHelper (hapticPort, Ipv4Address( staNodeIp->GetAddress(1,0).GetLocal()));
		  tcpTeleHelper.SetAttribute("FileName",StringValue("src/Kcl-Haptic-Sim/test/test_force.txt"));
		  tcpTeleHelper.SetAttribute ("SamplingIntervalSeconds", DoubleValue (0.001));
		  tcpTeleHelper.SetAttribute ("ApplyDataReduction", BooleanValue (applyReduction));
		  ApplicationContainer tcpTeleApps = tcpTeleHelper.Install(csmaNodes.Get (nCsma));
		  tcpTeleApps.Start (Seconds(1));
		  tcpTeleApps.Stop (Seconds (simTime));

		  TcpHapticOperatorHelper tcpHOP (Address(csmaInterfaces.GetAddress (nCsma)),hapticPort);
		  tcpHOP.SetAttribute ("PositionFile", StringValue ("src/Kcl-Haptic-Sim/test/test_pos.txt"));
		  tcpHOP.SetAttribute ("VelocityFile", StringValue ("src/Kcl-Haptic-Sim/test/test_fakeVelo.txt"));
		  tcpHOP.SetAttribute ("SamplingIntervalSeconds", DoubleValue (0.001));
		  tcpHOP.SetAttribute ("ApplyDataReduction", BooleanValue (applyReduction));
		  ApplicationContainer apps = tcpHOP.Install (wifiStaNodes.Get (nWifi - 1));
		  apps.Start (Seconds (2));
		  apps.Stop (Seconds (simTime));
	  }
	  else{
		  NS_LOG_DEBUG("Only UDP and TCP are currently supported as transport layer protocols");
		  exit(1);
	  }

	  ////////////////////////////////////////////////////////////////////////////////
	  //
	  //	Monitoring
	  //
	  ////////////////////////////////////////////////////////////////////////////////



	  // Flow monitor
	  Ptr<FlowMonitor> flowMonitor;
	  FlowMonitorHelper flowHelper;
	  NodeContainer nc;
	  nc.Add(wifiStaNodes.Get (nWifi - 1));
	  nc.Add(csmaNodes.Get (nCsma));
	  flowMonitor = flowHelper.Install(nc);

	  Simulator::Stop (Seconds (simTime));

	  if (tracing == true)
	    {
	      pointToPoint.EnablePcapAll ("third");
	      phy.EnablePcap ("third", apDevices.Get (0));
	      csma.EnablePcap ("third", csmaDevices.Get (0), true);
	    }

	  NS_LOG_DEBUG("Simulations runs ...");
	  Simulator::Run ();

	  flowMonitor->CheckForLostPackets(Seconds(0));
	  flowMonitor->SerializeToXmlFile(fileName, true, true);
	  NS_LOG_DEBUG("... DONE");
	  NS_LOG_DEBUG("For flow statistics see: " << fileName);
	  Simulator::Destroy ();
	  return 0;
}
