/*
 * kcl-ethernet-udp-example.cc
 *
 *  Created on: 18 Aug 2016
 *      Author: matthias
 */

#include <sstream>

// Idea and code to create the ethernet networking structure is based on:
// https://www.nsnam.org/doxygen/third_8cc_source.html

#include "ns3/core-module.h"
#include "ns3/new-module-helper.h"
#include "ns3/log.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/flow-monitor-helper.h"
#include "../helper/haptic-operator-helper.h"
#include "../helper/haptic-tele-operator-helper.h"
#include "../helper/tcp-haptic-operator-helper.h"
#include "../helper/tcp-haptic-tele-operator-helper.h"

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1   n2   n3   n4
//    point-to-point  |    |    |    |
//                    ================
//                      LAN 10.1.2.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("kcl-ethernet-udp-example");

int
main (int argc, char *argv[])
{
  LogComponentEnable("kcl-ethernet-udp-example",LOG_LEVEL_DEBUG);
  NS_LOG_DEBUG("Staring ethernet example");
  bool verbose = true;
  uint32_t nCsma = 3;
  int appStartTime = 2;
  int simTime = 10;
  bool applyDataReduction = false;

  CommandLine cmd;
  cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("dataReduction", "Set to true if you want data reduction be applied to the haptic data",applyDataReduction);

  cmd.Parse (argc,argv);

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  nCsma = nCsma == 0 ? 1 : nCsma;

  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  NodeContainer csmaNodes;
  csmaNodes.Add (p2pNodes.Get (1));
  csmaNodes.Create (nCsma);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);

//  Ptr<NetDevice> htopNetDev = csmaDevices.Get (1);
//  Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
//  //em->SetAttribute ("ErrorRate", DoubleValue (0.00001));
//  em->SetAttribute ("ErrorRate", DoubleValue (0.0001));
//  htopNetDev->SetAttribute ("ReceiveErrorModel", PointerValue (em));

  InternetStackHelper stack;
  stack.Install (p2pNodes.Get (0));
  stack.Install (csmaNodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //
  //	APPLICATION LAYER
  //
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  double interPacketInterval = 0.001;
  uint16_t port = 9;  // well-known echo port number
  HapticTeleOperatorHelper server (port);
  server.SetAttribute("FileName", StringValue ("src/Kcl-Haptic-Sim/test-data/force.txt"));
  server.SetAttribute ("SamplingIntervalSeconds", DoubleValue (interPacketInterval));
  server.SetAttribute ("ApplyDataReduction", BooleanValue (applyDataReduction));
  ApplicationContainer apps = server.Install (csmaNodes.Get (1));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (30.0));



  HapticOperatorHelper client (csmaInterfaces.GetAddress(1), port);
  client.SetAttribute ("PositionFile", StringValue ("src/Kcl-Haptic-Sim/test-data/position.txt"));
  client.SetAttribute ("VelocityFile", StringValue ("src/Kcl-Haptic-Sim/test-data/velocity.txt"));
  client.SetAttribute ("SamplingIntervalSeconds", DoubleValue (interPacketInterval));
  client.SetAttribute ("ApplyDataReduction", BooleanValue (applyDataReduction));
  apps = client.Install (p2pNodes.Get (0));
  apps.Start (Seconds (2.0));
  apps.Stop (Seconds (29.0));


//    NS_LOG_DEBUG("Installing Tcp Haptic Applications");
//
//  int hapticPort = 4444;
//  TcpHapticTeleOperatorHelper tcpTeleHelper (hapticPort, p2pInterfaces.GetAddress (0));
//  tcpTeleHelper.SetAttribute("FileName",StringValue("src/Kcl-Haptic-Sim/test-data/force.txt"));
//  /*
//   * Un-comment if you want to apply data reduction
//   */
//  //tcpTeleHelper.SetAttribute ("ApplyDataReduction", BooleanValue (true));
//  ApplicationContainer tcpTeleApps = tcpTeleHelper.Install(csmaNodes.Get (1));
//  tcpTeleApps.Start (Seconds(appStartTime - 0.5));
//  tcpTeleApps.Stop (Seconds (simTime));
//
//  TcpHapticOperatorHelper tcpHOP (csmaInterfaces.GetAddress (1),hapticPort);
//  tcpHOP.SetAttribute ("PositionFile", StringValue ("src/Kcl-Haptic-Sim/test-data/position.txt"));
//  tcpHOP.SetAttribute ("VelocityFile", StringValue ("src/Kcl-Haptic-Sim/test-data/velocity.txt"));
//  tcpHOP.SetAttribute ("SamplingIntervalSeconds", DoubleValue (0.001));
//  /*
//   * Un-comment if you want to apply data reduction
//   */
//  //tcpHOP.SetAttribute ("ApplyDataReduction", BooleanValue (true));
//  ApplicationContainer apps = tcpHOP.Install (p2pNodes.Get (0));
//  apps.Start (Seconds (appStartTime));
//  apps.Stop (Seconds (simTime));


  ////////////////////////////////////////////////////////////////


  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  NodeContainer allNodes;
  allNodes.Add(p2pNodes);
  allNodes.Add(csmaNodes);
  flowMonitor = flowHelper.Install(allNodes);

  Simulator::Stop (Seconds(simTime));
  Simulator::Run ();

  std::stringstream fileName;
  fileName << "kcl-ethernet-";
  if (applyDataReduction){
	  fileName << "data-reduction-";
  }
  fileName << "example.xml";
  flowMonitor->CheckForLostPackets(Seconds(0));
  flowMonitor->SerializeToXmlFile(fileName.str(), true, true);

  Simulator::Destroy ();
  return 0;
}

