/*
 * kcl-lte-udp-example.cc
 *
 *  Created on: 19 Aug 2016
 *      Author: matthias
 */

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
#include "ns3/flow-monitor-module.h"
//#include "ns3/gtk-config-store.h"

using namespace ns3;

/**
 * Sample simulation script for LTE+EPC. It instantiates several eNodeB,
 * attaches one UE per eNodeB starts a flow for each UE to  and from a remote host.
 * It also  starts yet another flow between each UE pair.
 */

NS_LOG_COMPONENT_DEFINE ("KclLteExample");

int
main (int argc, char *argv[])
{

  uint16_t numberOfNodes = 2;
  double simTime = 31.1;
  double distance = 60.0;
  double interPacketInterval = 100;

  // Command line arguments
  CommandLine cmd;
  cmd.AddValue("numberOfNodes", "Number of eNodeBs + UE pairs", numberOfNodes);
  cmd.AddValue("simTime", "Total duration of the simulation [s])", simTime);
  cmd.AddValue("distance", "Distance between eNBs [m]", distance);
  cmd.AddValue("interPacketInterval", "Inter packet interval [ms])", interPacketInterval);
  cmd.Parse(argc, argv);

  // Enable Logging
  //LogComponentEnable ("KclLteExample", LOG_LEVEL_DEBUG);
  LogComponentEnable ("HapticOperator", LOG_LEVEL_ALL);
  //LogComponentEnable ("LteEnbNetDevice", LOG_LEVEL_ALL);
  //LogComponentEnable ("UdpClient", LOG_LEVEL_ALL);

  // There is not Udp Server in this simulation
  //LogComponentEnable ("UdpServer", LOG_LEVEL_ALL);

  ////////////////////////////////////////////////////////////////////////////////
  //
  //	Radio Access Network and Packet Core
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



   // Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  // Create the Internet
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

  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  NodeContainer ueNodes;
  NodeContainer enbNodes;
  enbNodes.Create(numberOfNodes);
  ueNodes.Create(numberOfNodes);

  // Install Mobility Model
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  for (uint16_t i = 0; i < numberOfNodes; i++)
    {
      positionAlloc->Add (Vector(distance * i, 0, 0));
    }
  MobilityHelper mobility;
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator(positionAlloc);
  mobility.Install(enbNodes);
  mobility.Install(ueNodes);

  // Install LTE Devices to the nodes
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
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

  // Attach one UE per eNodeB
  for (uint16_t i = 0; i < numberOfNodes; i++)
      {
        lteHelper->Attach (ueLteDevs.Get(i), enbLteDevs.Get(i));
        // side effect: the default EPS bearer will be activated
      }


//  // Install and start applications on UEs and remote host
//  uint16_t dlPort = 1234;
//  uint16_t ulPort = 2000;
//  uint16_t otherPort = 3000;
//  ApplicationContainer clientApps;
//  ApplicationContainer serverApps;
//  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
//    {
//      ++ulPort;
//      ++otherPort;
//      PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
//      PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
//      PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), otherPort));
//      serverApps.Add (dlPacketSinkHelper.Install (ueNodes.Get(u)));
//      serverApps.Add (ulPacketSinkHelper.Install (remoteHost));
//      serverApps.Add (packetSinkHelper.Install (ueNodes.Get(u)));
//
//      UdpClientHelper dlClient (ueIpIface.GetAddress (u), dlPort);
//      dlClient.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
//      dlClient.SetAttribute ("MaxPackets", UintegerValue(1000000));
//
//      UdpClientHelper ulClient (remoteHostAddr, ulPort);
//      ulClient.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
//      ulClient.SetAttribute ("MaxPackets", UintegerValue(1000000));
//
//      UdpClientHelper client (ueIpIface.GetAddress (u), otherPort);
//      client.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
//      client.SetAttribute ("MaxPackets", UintegerValue(1000000));
//
//      clientApps.Add (dlClient.Install (remoteHost));
//      clientApps.Add (ulClient.Install (ueNodes.Get(u)));
//      if (u+1 < ueNodes.GetN ())
//        {
//          clientApps.Add (client.Install (ueNodes.Get(u+1)));
//        }
//      else
//        {
//          clientApps.Add (client.Install (ueNodes.Get(0)));
//        }
//    }
//  serverApps.Start (Seconds (0.01));
//  clientApps.Start (Seconds (0.01));

  //////////////////////////////////////////////////////////////////////////////////////
  //
  //	Haptic Applications
  //
  //////////////////////////////////////////////////////////////////////////////////////

  int hapticPort = 4444;
  HapticTeleOperatorHelper teleOperator (hapticPort);
  teleOperator.SetAttribute("FileName", StringValue ("src/Kcl-Haptic-Sim/test/force.txt"));
  teleOperator.SetAttribute ("SamplingIntervalSeconds", DoubleValue (0.001));
  //teleOperator.SetAttribute ("ApplyDataReduction", BooleanValue (true));
  ApplicationContainer apps = teleOperator.Install (remoteHost);
  apps.Start (Seconds (0.01));
  apps.Stop (Seconds (30.0));

  HapticOperatorHelper client (remoteHostAddr, hapticPort);
  client.SetAttribute ("FileName", StringValue ("src/Kcl-Haptic-Sim/test/position.txt"));
  client.SetAttribute ("SamplingIntervalSeconds", DoubleValue (0.001));
  client.SetAttribute ("FileType", StringValue ("POSITION"));
  apps = client.Install (ueNodes.Get(0));
  apps.Start (Seconds (0.02));
  apps.Stop (Seconds (30.0));

  //lteHelper->EnableTraces ();


  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //
  //	Monitoring the Simulation
  //
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  NodeContainer allNodes;
  allNodes.Add(remoteHost);
  allNodes.Add(ueNodes.Get(0));
  flowMonitor = flowHelper.Install(allNodes);

  // Uncomment to enable PCAP tracing
  //p2ph.EnablePcapAll("lena-epc-first");

  Simulator::Stop(Seconds(simTime));
  Simulator::Run();

  flowMonitor->SerializeToXmlFile("kcl-lte-udp-example.xml", true, true);

  /*GtkConfigStore config;
  config.ConfigureAttributes();*/

  Simulator::Destroy();
  return 0;

}



