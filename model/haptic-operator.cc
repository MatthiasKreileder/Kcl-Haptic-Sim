/*
 * haptic-operator.cc
 *
 *  Created on: 13 Jul 2016
 *      Author: matthias
 */

#include "haptic-operator.h"
#include "sensor-data-sample.h"
#include "haptic-header.h"

#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"

#include <cstdlib>
#include <cstdio>
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("HapticOperator");

NS_OBJECT_ENSURE_REGISTERED (HapticOperator);

HapticOperator::HapticOperator() {
	  NS_LOG_FUNCTION (this);
	  m_peerPort = 0;
	  m_socket = 0;
	  m_sendEvent = EventId ();
	  m_hapticFileSensor = 0;
}

HapticOperator::~HapticOperator() {
	NS_LOG_FUNCTION(this);
}

TypeId
HapticOperator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::HapticOperator")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<HapticOperator> ()
    .AddAttribute ("Interval",
                   "The time to wait between packets", TimeValue (Seconds (0.0001)),
                   MakeTimeAccessor (&HapticOperator::m_interval),
                   MakeTimeChecker ())
    .AddAttribute ("RemoteAddress",
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&HapticOperator::m_peerAddress),
                   MakeAddressChecker ())
    .AddAttribute ("RemotePort", "The destination port of the outbound packets",
                   UintegerValue (100),
                   MakeUintegerAccessor (&HapticOperator::m_peerPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("FileName",
                   "The name of the file which contains the recorded haptic data.",
                   StringValue ("src/Kcl-Haptic-Sim/test/test_pos.txt"),
                   MakeStringAccessor (&HapticOperator::m_fileName),
                   MakeStringChecker()
				   )
  ;
  return tid;
}

void
HapticOperator::SetRemote (Ipv4Address ip, uint16_t port)
{
  NS_LOG_FUNCTION (this << ip << port);
  m_peerAddress = Address(ip);
  m_peerPort = port;
}

void
HapticOperator::SetRemote (Ipv6Address ip, uint16_t port)
{
  NS_LOG_FUNCTION (this << ip << port);
  m_peerAddress = Address(ip);
  m_peerPort = port;
}

void
HapticOperator::SetRemote (Address ip, uint16_t port)
{
  NS_LOG_FUNCTION (this << ip << port);
  m_peerAddress = ip;
  m_peerPort = port;
}

void
HapticOperator::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void
HapticOperator::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  if (m_socket == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket = Socket::CreateSocket (GetNode (), tid);
      if (Ipv4Address::IsMatchingType(m_peerAddress) == true)
        {
          m_socket->Bind ();
          m_socket->Connect (InetSocketAddress (Ipv4Address::ConvertFrom(m_peerAddress), m_peerPort));
        }
      else if (Ipv6Address::IsMatchingType(m_peerAddress) == true)
        {
          m_socket->Bind6 ();
          m_socket->Connect (Inet6SocketAddress (Ipv6Address::ConvertFrom(m_peerAddress), m_peerPort));
        }
    }

  m_socket->SetRecvCallback (MakeCallback (&HapticOperator::HandleRead, this));
  m_socket->SetAllowBroadcast (true);

  m_hapticFileSensor = new HapticFileSensor(m_fileName,HapticFileSensor::POSITION);

  m_sendEvent = Simulator::Schedule (Seconds (0.0), &HapticOperator::Send, this);
}

void
HapticOperator::StopApplication (void)
{
  NS_LOG_FUNCTION (this);
  delete m_hapticFileSensor;
  Simulator::Cancel (m_sendEvent);
}

void HapticOperator::HandleRead(Ptr<Socket> socket){
	  NS_LOG_FUNCTION (this << socket);

	  Ptr<Packet> packet;
	  Address from;
	  while ((packet = socket->RecvFrom (from)))
	    {
	      if (InetSocketAddress::IsMatchingType (from))
	        {
	          NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s server received " << packet->GetSize () << " bytes from " <<
	                       InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " <<
	                       InetSocketAddress::ConvertFrom (from).GetPort ());
	        }
	      else if (Inet6SocketAddress::IsMatchingType (from))
	        {
	          NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s server received " << packet->GetSize () << " bytes from " <<
	                       Inet6SocketAddress::ConvertFrom (from).GetIpv6 () << " port " <<
	                       Inet6SocketAddress::ConvertFrom (from).GetPort ());
	        }

	      packet->RemoveAllPacketTags ();
	      packet->RemoveAllByteTags ();

	      if (InetSocketAddress::IsMatchingType (from))
	        {
	          NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s server sent " << packet->GetSize () << " bytes to " <<
	                       InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " <<
	                       InetSocketAddress::ConvertFrom (from).GetPort ());
	        }
	      else if (Inet6SocketAddress::IsMatchingType (from))
	        {
	          NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s server sent " << packet->GetSize () << " bytes to " <<
	                       Inet6SocketAddress::ConvertFrom (from).GetIpv6 () << " port " <<
	                       Inet6SocketAddress::ConvertFrom (from).GetPort ());
	        }
	    }
}

void
HapticOperator::Send (void)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (m_sendEvent.IsExpired ());

  SensorDataSample sds;
  if(m_hapticFileSensor->GetNextSensorDataSample(sds)){

	  //
	  //	We made it here => there is still data to send
	  //

	  //
	  //	Wrap it in a HapticHeader to prepare data to be
	  //	send over the wire
	  //
	  HapticHeader hapticHeader;
	  hapticHeader.SetHapticMessage(sds.getSensorDataString());

	  Ptr<Packet> p = Create<Packet> (hapticHeader.GetSerializedSize());
	  p->AddHeader(hapticHeader);

	  std::stringstream peerAddressStringStream;
	  if (Ipv4Address::IsMatchingType (m_peerAddress))
	    {
	      peerAddressStringStream << Ipv4Address::ConvertFrom (m_peerAddress);
	    }
	  else if (Ipv6Address::IsMatchingType (m_peerAddress))
	    {
	      peerAddressStringStream << Ipv6Address::ConvertFrom (m_peerAddress);
	    }

	  if ((m_socket->Send (p)) >= 0)
	    {
	      NS_LOG_INFO ("TraceDelay TX to"
	                                    << peerAddressStringStream.str () << " Uid: "
	                                    << p->GetUid () << " Time: "
	                                    << (Simulator::Now ()).GetSeconds ());

	    }
	  else
	    {
	      NS_LOG_INFO ("Error while sending to"
	                                          << peerAddressStringStream.str ());
	    }

	      m_sendEvent = Simulator::Schedule (m_interval, &HapticOperator::Send, this);

  }
  else{
	  NS_LOG_DEBUG("No more SensorDataSample objects to send.");
  }





}

} /* namespace ns3 */
