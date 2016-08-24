/*
 * kcl-ethernet-udp-example.cc
 *
 *  Created on: 18 Aug 2016
 *      Author: matthias
 */

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
  bool verbose = true;
  uint32_t nCsma = 3;

  CommandLine cmd;
  cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);

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
  server.SetAttribute("FileName", StringValue ("src/Kcl-Haptic-Sim/test/force.txt"));
  server.SetAttribute ("SamplingIntervalSeconds", DoubleValue (interPacketInterval));
  server.SetAttribute ("ApplyDataReduction", BooleanValue (true));
  ApplicationContainer apps = server.Install (csmaNodes.Get (1));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (30.0));

  HapticOperatorHelper client (csmaInterfaces.GetAddress(1), port);
  client.SetAttribute ("FileName", StringValue ("src/Kcl-Haptic-Sim/test/position.txt"));
  client.SetAttribute ("SamplingIntervalSeconds", DoubleValue (interPacketInterval));
  client.SetAttribute ("FileType", StringValue ("POSITION"));
  client.SetAttribute ("ApplyDataReduction", BooleanValue (true));
  apps = client.Install (p2pNodes.Get (0));
  apps.Start (Seconds (2.0));
  apps.Stop (Seconds (29.0));


  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  NodeContainer allNodes;
  allNodes.Add(p2pNodes);
  allNodes.Add(csmaNodes);
  flowMonitor = flowHelper.Install(allNodes);

  Simulator::Stop (Seconds(31.0));
  Simulator::Run ();

  flowMonitor->SerializeToXmlFile("kcl-ethernet-udp-example.xml", true, true);

  Simulator::Destroy ();
  return 0;
}

