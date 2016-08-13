
#include "ns3/log.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "phantom-agent.h"
#include "haptic-header.h"

namespace ns3{

NS_LOG_COMPONENT_DEFINE ("PhantomAgent");

TypeId
PhantomAgent::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PhantomAgent")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<PhantomAgent> ()
    .AddAttribute ("Chai3dWrapperAddress",
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&PhantomAgent::m_peerAddress),
                   MakeAddressChecker ())
    .AddAttribute ("Chai3dWrapperPort", "The destination port of the outbound packets",
                   UintegerValue (100),
                   MakeUintegerAccessor (&PhantomAgent::m_peerPort),
                   MakeUintegerChecker<uint16_t> ())
//	.AddAttribute ("LocalAddressForChai3D",
//                   "The address we are binding to for the Chai3dServer",
//                   AddressValue (),
//                   MakeAddressAccessor (&PhantomAgent::m_localChai3DAddress),
//                   MakeAddressChecker ())
//    .AddAttribute ("LocalPortForChai3D",
//    			   "The port we are binding to for the Chai3dServer",
//                   UintegerValue (100),
//                   MakeUintegerAccessor (&PhantomAgent::m_localChai3DPort),
//                   MakeUintegerChecker<uint16_t> ())
  ;
  return tid;
}

PhantomAgent::PhantomAgent (){
	m_peerPort = 0;
	m_localPort = 0;
}

void PhantomAgent::SetLocal (Ipv4Address localIpv4, uint16_t localPort)
{
	m_localIpv4 = localIpv4;
	m_localPort = localPort;
}

void
PhantomAgent::SetRemote (Ipv4Address ip, uint16_t port)
{
  NS_LOG_FUNCTION (this << ip << port);
  m_peerAddress = Address(ip);
  m_peerPort = port;
}

void
PhantomAgent::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  if (m_socketForCommunicationWithPhantomOmni == 0)
    {

	  Ptr<UdpL4Protocol> udpSocketFactory = this->GetNode()->GetObject<UdpL4Protocol> ();
	  m_socketForCommunicationWithPhantomOmni = udpSocketFactory->CreateSocket ();
	  InetSocketAddress dst = InetSocketAddress (m_localIpv4, m_localPort);
	  m_socketForCommunicationWithPhantomOmni->Bind(dst);


    }
  m_socketForCommunicationWithPhantomOmni->SetRecvCallback (MakeCallback (&PhantomAgent::ReadPacketFromPhantom, this));
  //m_socketForCommunicationWithPhantomOmni->SetAllowBroadcast (true);


  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  m_chai3DSocket = Socket::CreateSocket (GetNode (), tid);
  if (Ipv4Address::IsMatchingType(m_peerAddress) == true)
    {
	  //m_chai3DSocket->Bind ();
	  m_chai3DSocket->Connect (InetSocketAddress (Ipv4Address::ConvertFrom(m_peerAddress), m_peerPort));
	  m_chai3DSocket->SetRecvCallback(MakeCallback (&PhantomAgent::ReadPacketFromChai3D, this));
    }
  else{
	  NS_ABORT_MSG("PhantomAgent currently only works with IPv4");
  }



}

void PhantomAgent::ReadPacketFromChai3D (Ptr<Socket> socket){
	NS_LOG_FUNCTION(this);

	Ptr<Packet> packet;
	Address from;
	while ((packet = socket->RecvFrom (from))){
		m_socketForCommunicationWithPhantomOmni->SendTo(packet,0,m_phantomAddress);
	}
}

void PhantomAgent::ReadPacketFromPhantom (Ptr<Socket> socket){
	NS_LOG_FUNCTION(this);

	Ptr<Packet> packet;
	Address from;
	while ((packet = socket->RecvFrom (from)))
	{
		NS_LOG_DEBUG ("At time " << Simulator::Now ().GetSeconds () << "s PhantomAgent received " << packet->GetSize () << " bytes from " <<
	                  InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " <<
	                  InetSocketAddress::ConvertFrom (from).GetPort ());

	    /*
	     * We need to store the this address to be able to send back the force-feedback
	     */
		m_phantomAddress = from;

        uint8_t buf[packet->GetSize () + 1];
        buf[packet->GetSize ()] = '\0';
        uint32_t size = packet->GetSize();

        packet->CopyData(buf,size);

        std::ostringstream oss;
        for(uint32_t i = 0; i < size; i++){
      	  oss << buf[i];
        }
        NS_LOG_DEBUG(oss.str());

	          //socket->SendTo(packet,0,from);

	          std::string msgToChai3D = oss.str();
	          msgToChai3D = msgToChai3D.substr(1,size-2);
	          NS_LOG_DEBUG(msgToChai3D);

	      	HapticHeader hapticHeader;
	      	hapticHeader.SetHapticMessage(msgToChai3D);

	      	// add header
	      	Ptr<Packet> packetToChai3D = Create<Packet> ();

	      	// Fill out udpHeader fields appropriately
	      	packetToChai3D->AddHeader (hapticHeader);

	      	//m_socketForCommunicationWithPhantomOmni->SendTo(packetToChai3D,0,from);
	          m_chai3DSocket->Send(packetToChai3D);


	}

}

}
