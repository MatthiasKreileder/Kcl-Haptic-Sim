/*
 * tcp-haptic-operator-test-suite.cc
 *
 *  Created on: 17 Aug 2016
 *      Author: matthias
 */

#include "ns3/test.h"
#include "ns3/haptic-operator-helper.h"
#include "ns3/haptic-operator.h"
#include "ns3/tcp-haptic-tele-operator-helper.h"
#include "../helper/tcp-haptic-operator-helper.h"
#include "../model/tcp-haptic-operator.h"
#include "ns3/packet.h"
#include "ns3/log.h"

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include <string>


namespace ns3 {

NS_LOG_COMPONENT_DEFINE("TcpHapticOperatorTest");

class TcpHapticOperatorBaseTestCase : public ns3::TestCase {
public:
	TcpHapticOperatorBaseTestCase ();
	virtual void DoRun (void);
};

TcpHapticOperatorBaseTestCase::TcpHapticOperatorBaseTestCase()
	: ns3::TestCase ("TcpHapticOperatorBaseTestCase")
{
}

void
TcpHapticOperatorBaseTestCase::DoRun()
{
	  NodeContainer nodes;
	  nodes.Create (2);

	  PointToPointHelper pointToPoint;
	  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
	  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

	  NetDeviceContainer devices;
	  devices = pointToPoint.Install (nodes);

	  InternetStackHelper stack;
	  stack.Install (nodes);

	  Ipv4AddressHelper address;
	  address.SetBase ("10.1.1.0", "255.255.255.252");
	  Ipv4InterfaceContainer interfaces = address.Assign (devices);

	  //////////////////////////////////////////////////////////////////////////////////////////
	  //
	  //	APPLICATION LAYER - TcpHapticOperator communicates with TcpHapticTeleOperator
	  //
	  //////////////////////////////////////////////////////////////////////////////////////////

	  uint16_t teleOperatorPort = 8080;
	  TcpHapticTeleOperatorHelper tcpTeleHelper (teleOperatorPort, interfaces.GetAddress (0));
	  tcpTeleHelper.SetAttribute("FileName",StringValue("src/Kcl-Haptic-Sim/test/test_force.txt"));
	  /*
	   * Un-comment if you want to apply data reduction
	   */
	  //tcpTeleHelper.SetAttribute ("ApplyDataReduction", BooleanValue (true));
	  ApplicationContainer tcpTeleApps = tcpTeleHelper.Install(nodes.Get(1));
	  tcpTeleApps.Start (Seconds(0));
	  tcpTeleApps.Stop (Seconds (20.));

	  TcpHapticOperatorHelper tcpHOP (Address(interfaces.GetAddress (1)),teleOperatorPort);
	  tcpHOP.SetAttribute ("PositionFile", StringValue ("src/Kcl-Haptic-Sim/test/test_pos.txt"));
	  tcpHOP.SetAttribute ("VelocityFile", StringValue ("src/Kcl-Haptic-Sim/test/test_fakeVelo.txt"));
	  tcpHOP.SetAttribute ("SamplingIntervalSeconds", DoubleValue (0.001));
	  /*
	   * Un-comment if you want to apply data reduction
	   */
	  //tcpHOP.SetAttribute ("ApplyDataReduction", BooleanValue (true));
	  ApplicationContainer apps = tcpHOP.Install (nodes.Get (0));
	  apps.Start (Seconds (2.0));
	  apps.Stop (Seconds (20.0));

	  //////////////////////////////////////////////////////////////////////////////////////////
	  //
	  //	MONITORING THE COMMUNICATION (IP - LEVEL)
	  //
	  //////////////////////////////////////////////////////////////////////////////////////////

	  // Flow monitor
	  Ptr<FlowMonitor> flowMonitor;
	  FlowMonitorHelper flowHelper;
	  flowMonitor = flowHelper.Install(nodes);

	  Simulator::Stop (Seconds (21));
	  Simulator::Run ();

	  flowMonitor->SerializeToXmlFile("TcpHapticCommStats.xml", true, true);

	  Simulator::Destroy ();
}

static class TcpHapticOperatorTestSuite : public TestSuite
{
public:
	TcpHapticOperatorTestSuite ()
    : TestSuite ("TcpHapticOperator", EXAMPLE)
  {
    AddTestCase (new TcpHapticOperatorBaseTestCase (), TestCase::QUICK);
  }
} g_tcpHapticOperatorTestSuite;


}


