/*
 * haptic-operator-test-suite.cc
 *
 *  Created on: 13 Jul 2016
 *      Author: matthias
 */

#include "ns3/test.h"
#include "../helper/haptic-operator-helper.h"
#include "../helper/haptic-tele-operator-helper.h"
#include "../model/haptic-operator.h"
#include "ns3/packet.h"
#include "ns3/log.h"

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"

#include <string>

// Network topology
//
//       n0    n1   n2   n3
//       |     |    |    |
//       =================
//              LAN
//
// - UDP flows from n0 to n1 and back
// - DropTail queues

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("HapticTeleOperatorTest");

class HapticTeleOperatorBaseTestCase : public ns3::TestCase {
public:
	HapticTeleOperatorBaseTestCase ();
	virtual void DoRun (void);
};

HapticTeleOperatorBaseTestCase::HapticTeleOperatorBaseTestCase()
	: ns3::TestCase ("HapticTeleOperatorBaseTestCase")
{
}

void
HapticTeleOperatorBaseTestCase::DoRun()
{


	//
	// Allow the user to override any of the defaults and the above Bind() at
	// run-time, via command-line arguments
	//
	  bool useV6 = false;
	  Address serverAddress;

	// Explicitly create the nodes required by the topology (shown above).
	//

	  NodeContainer n;
	  n.Create (4);

	  InternetStackHelper internet;
	  internet.Install (n);


	//
	// Explicitly create the channels required by the topology (shown above).
	//
	  CsmaHelper csma;
	  csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate (5000000)));
	  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
	  csma.SetDeviceAttribute ("Mtu", UintegerValue (1400));
	  NetDeviceContainer d = csma.Install (n);

	//
	// We've got the "hardware" in place.  Now we need to add IP addresses.
	//

	  if (useV6 == false)
	    {
	      Ipv4AddressHelper ipv4;
	      ipv4.SetBase ("10.1.1.0", "255.255.255.0");
	      Ipv4InterfaceContainer i = ipv4.Assign (d);
	      serverAddress = Address(i.GetAddress (1));
	    }
	  else
	    {
	      Ipv6AddressHelper ipv6;
	      ipv6.SetBase ("2001:0000:f00d:cafe::", Ipv6Prefix (64));
	      Ipv6InterfaceContainer i6 = ipv6.Assign (d);
	      serverAddress = Address(i6.GetAddress (1,1));
	    }


	//
	// Create a HapticTeleOperator application on node one.
	//
	  double interPacketInterval = 0.001;
	  uint16_t port = 9;  // well-known echo port number
	  HapticTeleOperatorHelper server (port);
	  server.SetAttribute("FileName", StringValue ("src/Kcl-Haptic-Sim/test/test_force.txt"));
	  server.SetAttribute ("SamplingIntervalSeconds", DoubleValue (interPacketInterval));
	  server.SetAttribute ("ApplyDataReduction", BooleanValue (true));
	  ApplicationContainer apps = server.Install (n.Get (1));
	  apps.Start (Seconds (1.0));
	  apps.Stop (Seconds (3.0));



	//
	// Create a HapticOperator application to send UDP datagrams from node zero to
	// node one.
	//


	  HapticOperatorHelper client (serverAddress, port);
	  client.SetAttribute ("SamplingIntervalSeconds", DoubleValue (interPacketInterval));
	  client.SetAttribute ("PositionFile", StringValue ("src/Kcl-Haptic-Sim/test/test_position.txt"));
	  client.SetAttribute ("VelocityFile", StringValue ("src/Kcl-Haptic-Sim/test/fakeVelocity.txt"));
	  server.SetAttribute ("ApplyDataReduction", BooleanValue (true));
	  apps = client.Install (n.Get (0));
	  apps.Start (Seconds (2.0));
	  apps.Stop (Seconds (3.0));

	  // Flow monitor
	  Ptr<FlowMonitor> flowMonitor;
	  FlowMonitorHelper flowHelper;
	  NodeContainer nc;
	  nc.Add(n.Get(0));
	  nc.Add(n.Get(1));
	  flowMonitor = flowHelper.Install(nc);

	//
	// Now, do the actual simulation.
	//
	  Simulator::Stop (Seconds (4.0));
	  Simulator::Run ();

	  flowMonitor->SerializeToXmlFile("NameOfFile1.xml", true, true);

	  Simulator::Destroy ();


}

static class HapticTeleOperatorTestSuite : public TestSuite
{
public:
	HapticTeleOperatorTestSuite ()
    : TestSuite ("HapticTeleOperator", EXAMPLE)
  {
    AddTestCase (new HapticTeleOperatorBaseTestCase (), TestCase::QUICK);
  }
} g_hapticTeleOperatorTestSuite;


}


