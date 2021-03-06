
 // Idea and code to create the lte network is based on:
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
#include "ns3/chai3d-server-helper.h"
#include "ns3/haptic-operator-helper.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/emu-fd-net-device-helper.h"
#include "ns3/phantom-agent-helper.h"
#include <sstream>
//#include "ns3/gtk-config-store.h"

using namespace ns3;

/**
 * Sample simulation script for LTE+EPC. It instantiates several eNodeB,
 * attaches one UE per eNodeB starts a flow for each UE to  and from a remote host.
 * It also  starts yet another flow between each UE pair.
 */

NS_LOG_COMPONENT_DEFINE ("KclLteChai3dExample");

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
  LogComponentEnable ("KclLteChai3dExample", LOG_LEVEL_DEBUG);
  //LogComponentEnable ("HapticOperator", LOG_LEVEL_ALL);
  //LogComponentEnable ("LteEnbNetDevice", LOG_LEVEL_ALL);
  //LogComponentEnable ("UdpClient", LOG_LEVEL_ALL);

  // There is not Udp Server in this simulation
  //LogComponentEnable ("UdpServer", LOG_LEVEL_ALL);

  GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));
  GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));

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

  /////////////////////////////////////////////////////////////////////////////////////////////

  std::string netmask ("255.255.0.0");
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
  NetDeviceContainer devices = emu.Install (ueNodes.Get(0));

  Ptr<NetDevice> device = devices.Get (0);
  device->SetAttribute ("Address", localMac);
  /////////////////////////////////////////////////////////////////////////////////////////////

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


  //////////////////////////////////////////////////////////////////////////////////////
  //
  //	Haptic Applications
  //
  //////////////////////////////////////////////////////////////////////////////////////

  NS_LOG_INFO ("Create IPv4 Interface");
  Ptr<Ipv4> ipv4 = ueNodes.Get(0)->GetObject<Ipv4> ();
  uint32_t interface = ipv4->AddInterface (device);
  Ipv4InterfaceAddress address = Ipv4InterfaceAddress (localIp, localMask);
  ipv4->AddAddress (interface, address);
  ipv4->SetMetric (interface, 1);
  ipv4->SetUp (interface);

		//
		// Create a Chai3dServer application on node one.
		//

		  uint16_t port = 1234;  // well-known echo port number
		  Chai3dServerHelper chai3dServer (port,ueIpIface.GetAddress(0),port);
		  chai3dServer.SetAttribute ("Chai3dWrapper", StringValue ("/home/matthias/Development/chai3d-3.0.0/bin/04-shapes"));
		  ApplicationContainer apps = chai3dServer.Install (remoteHost);
		  apps.Start (Seconds (1.0));
		  apps.Stop (Seconds (140.0));


//		//
//		// Create a HapticOperator application to send UDP datagrams from node zero to
//		// node one.
//		//
//		  HapticOperatorHelper client (remoteHostAddr, port);
//		  //client.SetAttribute ("FileName", StringValue ("src/Kcl-Haptic-Sim/test/position.txt"));
//		  client.SetAttribute ("SamplingIntervalSeconds", DoubleValue( 0.001));
//		  client.SetAttribute ("PositionFile", StringValue ("src/Kcl-Haptic-Sim/test-data/position.txt"));
//		  client.SetAttribute ("VelocityFile", StringValue ("src/Kcl-Haptic-Sim/test-data/velocity.txt"));
//		  client.SetAttribute ("ApplyDataReduction", BooleanValue (true));
//		  apps = client.Install (ueNodes.Get(0));
//		  apps.Start (Seconds (2.0));
//		  apps.Stop (Seconds (130.0));

		  PhantomAgentHelper phantomAgent(localIp,1234);
		  phantomAgent.SetAttribute("Chai3dWrapperAddress", AddressValue( remoteHostAddr));
		  phantomAgent.SetAttribute("Chai3dWrapperPort",UintegerValue( port));
		  ApplicationContainer chai3Dapps = phantomAgent.Install(ueNodes.Get(0));
		  chai3Dapps.Start(Seconds(1.0));
		  chai3Dapps.Stop(Seconds(120));

  //lteHelper->EnableTraces ();


  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //
  //	Monitoring the Simulation
  //
  ///////////////////////////////////////////////////////////////////////////////////////////////////


  // Uncomment to enable PCAP tracing
  //p2ph.EnablePcapAll("lena-epc-first");

  Simulator::Stop(Seconds(simTime));
  Simulator::Run();

  /*GtkConfigStore config;
  config.ConfigureAttributes();*/

  Simulator::Destroy();
  return 0;

}



