/*
 * haptic-operator.cc
 *
 *  Created on: 13 Jul 2016
 *      Author: matthias
 */
//
//	Code and idea based on: https://www.nsnam.org/docs/release/3.16/doxygen/udp-echo-client_8cc_source.html
//
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
#include "ns3/integer.h"
#include "ns3/string.h"
#include "ns3/double.h"
#include "ns3/boolean.h"
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
	  m_packetsSent = 0;
	  m_reduction = nullptr;
	  m_deadband = 0.3;
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
    .AddAttribute ("SamplingIntervalSeconds",
                   "The time in seconds between two data samples.", DoubleValue (0.001),
                   MakeDoubleAccessor (&HapticOperator::m_interval),
                   MakeDoubleChecker<double>())
    .AddAttribute ("RemoteAddress",
                   "The destination Address of the outbound packets",
                   AddressValue (),
                   MakeAddressAccessor (&HapticOperator::m_peerAddress),
                   MakeAddressChecker ())
    .AddAttribute ("RemotePort", "The destination port of the outbound packets",
                   UintegerValue (100),
                   MakeUintegerAccessor (&HapticOperator::m_peerPort),
                   MakeUintegerChecker<uint16_t> ())
	.AddAttribute ("PositionFile",
				   "The file that contains the position samples.",
				   StringValue ("src/Kcl-Haptic-Sim/test/position.txt"),
				   MakeStringAccessor (&HapticOperator::m_positionFile),
				   MakeStringChecker()
				   )
	.AddAttribute ("VelocityFile",
				   "The file that contains the velocity samples.",
				   StringValue ("src/Kcl-Haptic-Sim/test/faleVosition.txt"),
                   MakeStringAccessor (&HapticOperator::m_velocityFile),
                   MakeStringChecker()
				   )
	.AddAttribute  ("ApplyDataReduction",
				   "Set to true if data reduction should be applied.",
				   BooleanValue (false),
				   MakeBooleanAccessor(&HapticOperator::m_useDataReductionAlgorithm),
				   MakeBooleanChecker()
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

  if (m_useDataReductionAlgorithm){
	  m_reduction = std::unique_ptr<HapticDataReductionAlgorithm>{new  HapticDataReductionAlgorithm(0.2)};
  }

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

  m_hapticFileSensor = new HapticFileSensor(m_positionFile,m_velocityFile);

  m_sendEvent = Simulator::Schedule (Seconds (0.0), &HapticOperator::Send, this);
}

void
HapticOperator::StopApplication (void)
{
  NS_LOG_FUNCTION (this);
  delete m_hapticFileSensor;
  NS_LOG_DEBUG("Number of packets sent:" << m_packetsSent);
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
	          NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s HapticOperator received " << packet->GetSize () << " bytes from " <<
	                       InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " <<
	                       InetSocketAddress::ConvertFrom (from).GetPort ());
	        }
	      else if (Inet6SocketAddress::IsMatchingType (from))
	        {
	          NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s HapticOperator received " << packet->GetSize () << " bytes from " <<
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
  // on the HapticOperator side we want to send position samples
  // ToDo: Think about sending Velocity + Position samples
  if(m_hapticFileSensor->GetNextSensorDataSample(sds,HapticFileSensor::POSITION)){

	  //
	  //	We made it here => there is still data to send
	  //
	  NS_LOG_DEBUG("SensorDataContent: " << sds.getSensorDataString());


	  if (m_useDataReductionAlgorithm){
		  // check if we need to transmit this sample - the algorithm works with velocity samples

		  NS_LOG_DEBUG("Apply reduction");
		  SensorDataSample velocity;
		  m_hapticFileSensor->GetNextSensorDataSample(velocity,HapticFileSensor::VELOCITY);
		  if (!m_reduction->needsToBeTransmitted(velocity.getSensorDataVector())){
			  m_sendEvent = Simulator::Schedule (Seconds( m_interval), &HapticOperator::Send, this);
			  NS_LOG_DEBUG("Does not need to be transmitted");
			  return;
		  }


	  }
	  else{
		  NS_LOG_DEBUG("DISABLED REDUCTION ALGO");
	  }
	  //
	  //	Wrap it in a HapticHeader to prepare data to be
	  //	send over the wire
	  //
	  HapticHeader hapticHeader;
	  hapticHeader.SetHapticMessage(sds.getSensorDataString());

	  NS_LOG_DEBUG("HapticHeader contents: " << hapticHeader.GetHapticMessage());

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
	      NS_LOG_INFO ("Error while sending to" << peerAddressStringStream.str ());
	    }

	      m_sendEvent = Simulator::Schedule (Seconds( m_interval), &HapticOperator::Send, this);
	      m_packetsSent++;
	      NS_LOG_DEBUG("Packets sent: " << m_packetsSent);

  }
  else{
	  NS_LOG_DEBUG("No more SensorDataSample objects to send.");
  }





}

} /* namespace ns3 */
