

#include "ns3/test.h"
#include "../helper/haptic-operator-helper.h"
#include "../helper/chai3d-server-helper.h"
#include "../model/chai3d-server.h"
#include "ns3/packet.h"
#include "ns3/log.h"

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"

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

NS_LOG_COMPONENT_DEFINE("Chai3dServerTest");

class Chai3dServerBaseTestCase : public ns3::TestCase {
public:
	Chai3dServerBaseTestCase ();
	virtual void DoRun (void);
};

Chai3dServerBaseTestCase::Chai3dServerBaseTestCase()
	: ns3::TestCase ("Chai3dServerBaseTestCase")
{
}

void
Chai3dServerBaseTestCase::DoRun()
{
	  GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));
	  GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));
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
	// Create a UdpEchoServer application on node one.
	//
	  double interPacketInterval = 0.001;
	  uint16_t port = 9;  // well-known echo port number
	  Chai3dServerHelper server (port);
	  //server.SetAttribute("FileName", StringValue ("src/Kcl-Haptic-Sim/test/test_force.txt"));
	  //server.SetAttribute ("SamplingIntervalSeconds", TimeValue (interPacketInterval));
	  ApplicationContainer apps = server.Install (n.Get (1));
	  apps.Start (Seconds (1.0));
	  apps.Stop (Seconds (10.0));



	//
	// Create a UdpEchoClient application to send UDP datagrams from node zero to
	// node one.
	//


	  HapticOperatorHelper client (serverAddress, port);
	  client.SetAttribute ("FileName", StringValue ("src/Kcl-Haptic-Sim/test/position.txt"));
	  client.SetAttribute ("SamplingIntervalSeconds", DoubleValue( interPacketInterval));
	  client.SetAttribute ("FileType", StringValue ("POSITION"));
	  apps = client.Install (n.Get (0));
	  apps.Start (Seconds (2.0));
	  apps.Stop (Seconds (10.0));



	//
	// Now, do the actual simulation.
	//
	  Simulator::Stop(Seconds(30));
	  Simulator::Run ();

	  Simulator::Destroy ();


}

static class Chai3dServerTestSuite : public TestSuite
{
public:
	Chai3dServerTestSuite ()
    : TestSuite ("Chai3dServer", EXAMPLE)
  {
    AddTestCase (new Chai3dServerBaseTestCase (), TestCase::QUICK);
  }
} g_chai3dServerTestSuite;


}


