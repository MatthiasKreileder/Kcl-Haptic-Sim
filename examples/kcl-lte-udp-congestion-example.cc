/*
 * kcl-lte-udp-example.cc
 *
 *  Created on: 19 Aug 2016
 *      Author: matthias
 */

// Idea and code to create the lte example is based on:
// https://www.nsnam.org/doxygen/lena-simple-epc-emu_8cc_source.html

#include "ns3/lte-helper.h"
#include "ns3/epc-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/config-store.h"
#include "ns3/haptic-operator-helper.h"
#include "ns3/haptic-tele-operator-helper.h"
#include "ns3/tcp-haptic-operator-helper.h"
#include "ns3/tcp-haptic-tele-operator-helper.h"
#include "ns3/flow-monitor-module.h"
#include <sstream>
//#include "ns3/gtk-config-store.h"

using namespace ns3;

/**
 * QUESTION - I set the guaranteed bit rate to 130 !?
 * 			- I set the Quality of Service class indicatior to EpsBearer::GBR_CONV_VOICE !?
 * 			- I call this function for both the HapticOperator UE and the UdoEchoClient UE !?
 */
void assignBearer(Ptr<NetDevice> netDev, Ptr<LteHelper> lteHelper){
	GbrQosInformation qos;

	int gbr = 130;
	qos.gbrDl = gbr;  // bit/s, considering IP, UDP, RLC, PDCP header size
	qos.gbrUl = gbr;
	qos.mbrDl = qos.gbrDl;
	qos.mbrUl = qos.gbrUl;

	enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
	EpsBearer bearer (q, qos);
	bearer.arp.priorityLevel = 1;
	bearer.arp.preemptionCapability = true;
	bearer.arp.preemptionVulnerability = true;
	lteHelper->ActivateDedicatedEpsBearer (netDev, bearer, EpcTft::Default ());
}

void InstallMobilityModel(NodeContainer& ueNodes, NodeContainer& enb, double distance);

/**
 * Sample simulation script for LTE+EPC. It instantiates several eNodeB,
 * attaches one UE per eNodeB starts a flow for each UE to  and from a remote host.
 * It also  starts yet another flow between each UE pair.
 */

NS_LOG_COMPONENT_DEFINE ("KclLteCongestionExample");

int
main (int argc, char *argv[])
{
  uint16_t numberOfUeTrafficGeneratorNodes = 20;
  double simTime = 30.0;
  double appStartTime = 2.0;
  double distance = 0.50;
  std::string protocol = "UDP";
  double interPacketInterval = 10;
  int maxPackets = 10000;

  // Command line arguments
  CommandLine cmd;
  cmd.AddValue("simTime", "Total duration of the simulation [s])", simTime);
  cmd.AddValue("distance", "Distance between eNBs [m]", distance);
  cmd.AddValue("protocol","The transport layer protocol for the haptic applications: \"UDP\" or \"TCP\" ",protocol);
  cmd.AddValue("InterPacket","Inter packet interval",interPacketInterval);
  cmd.AddValue("Nodes","b",numberOfUeTrafficGeneratorNodes);
  cmd.Parse(argc, argv);

  ////////////////////////////////////////////////////////////////////////////////
  //
  //	Prepare Radio Access Network and Packet Core
  //
  ////////////////////////////////////////////////////////////////////////////////

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults();

  // parse again so you can override default values from the command line
  cmd.Parse(argc, argv);

  Ptr<Node> pgw = epcHelper->GetPgwNode ();


   // Create two remote hosts - one being the HapticTeleOperator one being a UdpEchoServer
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (2);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  Ptr<Node> udpEchoRemoteHost = remoteHostContainer.Get (1);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  ////////////////////////////////////////////////////////////////////////////////
  //
  //	Prepare Internet and routes
  //
  ////////////////////////////////////////////////////////////////////////////////

  // Create the connection to the network where the HapticTeleOperator lives
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
  // interface 0 is localhost, 1 is the p2p device
  Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);

  // Create the connection to the network where the UdpEchoServer lives
  internetDevices = p2ph.Install (pgw, udpEchoRemoteHost);
  ipv4h.SetBase ("2.0.0.0","255.0.0.0");
  internetIpIfaces = ipv4h.Assign (internetDevices);
  Ipv4Address udpEchoRemoteHostAddr = internetIpIfaces.GetAddress (1);

  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  Ptr<Ipv4StaticRouting> udpEchoRemoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (udpEchoRemoteHost->GetObject<Ipv4> ());
  udpEchoRemoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  ////////////////////////////////////////////////////////////////////////////////
  //
  //	Connect the mobile nodes to LTE and set routes to remote nodes
  //
  ////////////////////////////////////////////////////////////////////////////////


  NodeContainer ueNodes;
  NodeContainer enbNode;
  enbNode.Create(1);
  ueNodes.Create(1 + numberOfUeTrafficGeneratorNodes);

  Ptr<Node> hapticOperatorNode = ueNodes.Get(0);
  //Ptr<Node> udpEchoClientNode = ueNodes.Get(1);

  InstallMobilityModel(ueNodes,enbNode, distance);


  // Install LTE Devices to the nodes
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNode);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);

  // Install the IP stack on the UEs
  internet.Install (ueNodes);
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));
  // Assign IP address to UEs, and install applications
  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

  // Attach UEs to the eNodeB
  for(size_t i = 0; i < ueNodes.GetN();i++){
	  lteHelper->Attach (ueLteDevs.Get(i), enbLteDevs.Get(0));
	  assignBearer(ueLteDevs.Get(i), lteHelper);
  }
  //lteHelper->Attach (ueLteDevs.Get(1), enbLteDevs.Get(0));
  // side effect: the default EPS bearer will be activated
//
//  // Bearer for the HapticOperator
//  assignBearer(ueLteDevs.Get(0), lteHelper);
//  // Bearer for the UdpEchoClient
//  assignBearer(ueLteDevs.Get(1), lteHelper);

  //////////////////////////////////////////////////////////////////////////////////////
  //
  //	Haptic Applications
  //
  //////////////////////////////////////////////////////////////////////////////////////

  // constructing the file name used by the flow-monitor
  std::stringstream ss;
  ss << "kcl-lte";
  //
  //	UDP
  //
  if (protocol.compare("UDP") == 0){

	  NS_LOG_DEBUG("Installing Udp Haptic Applications");
	  ss << "-udp";

	  int hapticPort = 4444;
	  HapticTeleOperatorHelper teleOperator (hapticPort);
	  teleOperator.SetAttribute("FileName", StringValue ("src/Kcl-Haptic-Sim/test-data/force.txt"));
	  teleOperator.SetAttribute ("SamplingIntervalSeconds", DoubleValue (0.001));
	  /*
	   * Un-comment if you want to apply data reduction
	   */
	  //teleOperator.SetAttribute ("ApplyDataReduction", BooleanValue (true));
	  ApplicationContainer apps = teleOperator.Install (remoteHost);
	  apps.Start (Seconds (appStartTime - 0.5));	// Let's start the HapticTeleOperator slightly before the HapticOperator
	  apps.Stop (Seconds (simTime));

	  HapticOperatorHelper client (remoteHostAddr, hapticPort);
	  client.SetAttribute ("PositionFile", StringValue ("src/Kcl-Haptic-Sim/test-data/position.txt"));
	  client.SetAttribute ("VelocityFile", StringValue ("src/Kcl-Haptic-Sim/test-data/velocity.txt"));
	  client.SetAttribute ("SamplingIntervalSeconds", DoubleValue (0.001));
	  /*
	   * Un-comment if you want to apply data reduction
	   */
	  //client.SetAttribute ("ApplyDataReduction", BooleanValue (true));
	  apps = client.Install (ueNodes.Get(0));
	  apps.Start (Seconds (appStartTime));
	  apps.Stop (Seconds (simTime));
  }
  //
  //	TCP
  //
  else if (protocol.compare("TCP") == 0){
	  ss << "-tcp";
	  NS_LOG_DEBUG("Installing Tcp Haptic Applications");

	  int hapticPort = 4444;
	  TcpHapticTeleOperatorHelper tcpTeleHelper (hapticPort, ueIpIface.GetAddress (0));
	  tcpTeleHelper.SetAttribute("FileName",StringValue("src/Kcl-Haptic-Sim/test/force.txt"));
	  /*
	   * Un-comment if you want to apply data reduction
	   */
	  //tcpTeleHelper.SetAttribute ("ApplyDataReduction", BooleanValue (true));
	  ApplicationContainer tcpTeleApps = tcpTeleHelper.Install(remoteHost);
	  tcpTeleApps.Start (Seconds(appStartTime - 0.5));
	  tcpTeleApps.Stop (Seconds (simTime));

	  TcpHapticOperatorHelper tcpHOP (remoteHostAddr,hapticPort);
	  tcpHOP.SetAttribute ("PositionFile", StringValue ("src/Kcl-Haptic-Sim/test/position.txt"));
	  tcpHOP.SetAttribute ("VelocityFile", StringValue ("src/Kcl-Haptic-Sim/test/velocity.txt"));
	  tcpHOP.SetAttribute ("SamplingIntervalSeconds", DoubleValue (0.001));
	  /*
	   * Un-comment if you want to apply data reduction
	   */
	  //tcpHOP.SetAttribute ("ApplyDataReduction", BooleanValue (true));
	  ApplicationContainer apps = tcpHOP.Install (ueNodes.Get(0));
	  apps.Start (Seconds (appStartTime));
	  apps.Stop (Seconds (simTime));
  }
  else {
	  NS_LOG_DEBUG("Currently only UDP and TCP are supported for Haptic Applications.");
	  exit(0);
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //
  //	Udp Echo Applications
  //
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  // Install and start applications on UEs and remote host
  uint16_t dlPort = 1234;
  uint16_t ulPort = 2000;
  uint16_t otherPort = 3000;
  ApplicationContainer clientApps;
  ApplicationContainer serverApps;
  // It's important to skip the first node here!
  for (uint32_t u = 1; u < ueNodes.GetN (); ++u)
    {
      ++ulPort;
      ++otherPort;
      PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
      PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
      PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), otherPort));
      serverApps.Add (dlPacketSinkHelper.Install (ueNodes.Get(u)));
      serverApps.Add (ulPacketSinkHelper.Install (udpEchoRemoteHost));
      serverApps.Add (packetSinkHelper.Install (ueNodes.Get(u)));

      UdpClientHelper dlClient (ueIpIface.GetAddress (u), dlPort);
      dlClient.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
      dlClient.SetAttribute ("MaxPackets", UintegerValue(maxPackets));

      UdpClientHelper ulClient (udpEchoRemoteHostAddr, ulPort);
      ulClient.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
      ulClient.SetAttribute ("MaxPackets", UintegerValue(maxPackets));

      UdpClientHelper client (ueIpIface.GetAddress (u), otherPort);
      client.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
      client.SetAttribute ("MaxPackets", UintegerValue(maxPackets));

      clientApps.Add (dlClient.Install (udpEchoRemoteHost));
      clientApps.Add (ulClient.Install (ueNodes.Get(u)));
      if (u+1 < ueNodes.GetN ())
        {
          clientApps.Add (client.Install (ueNodes.Get(u+1)));
        }
      else
        {
          //clientApps.Add (client.Install (ueNodes.Get(0)));
        }
    }
  serverApps.Start (Seconds (appStartTime));
  clientApps.Start (Seconds (appStartTime));
  clientApps.Stop (Seconds (simTime));
  serverApps.Stop (Seconds (simTime));
//  //
//  // Create a UdpEchoServer application on node one.
//  //
//    uint16_t udpEchport = 9;  // well-known echo port number
//    UdpEchoServerHelper server (udpEchport);
//    ApplicationContainer apps = server.Install (udpEchoRemoteHost);
//    apps.Start (Seconds (appStartTime - 0.5));
//    apps.Stop (Seconds (simTime));
//
//  //
//  // Create a UdpEchoClient application to send UDP datagrams from node zero to
//  // node one.
//  //
//    uint32_t packetSize = 1024;
//    uint32_t maxPacketCount = 100000;
//    Time interPacketIntervalUdpEcho = Seconds (0.0001);
//    UdpEchoClientHelper client (udpEchoRemoteHostAddr, udpEchport);
//    client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
//    client.SetAttribute ("Interval", TimeValue (interPacketIntervalUdpEcho));
//    client.SetAttribute ("PacketSize", UintegerValue (packetSize));
//    apps = client.Install (udpEchoClientNode);
//    apps.Start (Seconds (appStartTime));
//    apps.Stop (Seconds (simTime));

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //
  //	Monitoring the Simulation
  //
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  NodeContainer allNodes;
  allNodes.Add(remoteHost);
  allNodes.Add(udpEchoRemoteHost);
  allNodes.Add(ueNodes);
  flowMonitor = flowHelper.Install(allNodes);

  Simulator::Stop(Seconds(simTime));
  Simulator::Run();

  ss << "InterPacketMilliseconds=" << interPacketInterval;
  ss << "-Nodes=" << numberOfUeTrafficGeneratorNodes;
  ss << "-example.xml";
  flowMonitor->SerializeToXmlFile(ss.str(), true, true);

  Simulator::Destroy();
  return 0;

}

void InstallMobilityModel(NodeContainer& ueNodes, NodeContainer& enb, double distance){

	  // Install Mobility Model
	  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
	  for (uint16_t i = 0; i < 2; i++)
	    {
	      positionAlloc->Add (Vector(distance * i, 0, 0));
	    }
	  MobilityHelper mobility;
	  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	  mobility.SetPositionAllocator(positionAlloc);
	  mobility.Install(ueNodes);


	  // Install Mobility Model
	  Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator> ();
	  enbPositionAlloc->Add (Vector(distance * 0.5, 0, 0));
	  mobility.SetPositionAllocator(enbPositionAlloc);
	  mobility.Install(enb);
}
