/*
 * haptic-tele-operator.cc
 *
 *  Created on: 19 Jul 2016
 *      Author: matthias
 */

#include "haptic-tele-operator.h"
#include "haptic-header.h"
#include "ns3/log.h"

#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/address-utils.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("HapticTeleOperator");

NS_OBJECT_ENSURE_REGISTERED (HapticTeleOperator);

TypeId
HapticTeleOperator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::HapticTeleOperator")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<HapticTeleOperator> ()
    .AddAttribute ("Port", "Port on which we listen for incoming packets.",
                   UintegerValue (9),
                   MakeUintegerAccessor (&HapticTeleOperator::m_port),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("SamplingIntervalSeconds",
                   "The time in seconds between two data samples.", DoubleValue (0.0001),
                   MakeDoubleAccessor (&HapticTeleOperator::m_interval),
                   MakeDoubleChecker<double>())
    .AddAttribute ("FileName",
                   "The name of the file which contains the recorded haptic data.",
                   StringValue ("src/Kcl-Haptic-Sim/test/test_force.txt"),
                   MakeStringAccessor (&HapticTeleOperator::m_fileName),
                   MakeStringChecker()
 			   	  )
  ;
  return tid;
}

HapticTeleOperator::HapticTeleOperator(){
	NS_LOG_FUNCTION(this);
	m_hapticFileSensor = 0;
	m_hapticOperatorAddressIsSet = false;
	m_readSensorDataEvent = EventId ();
}

HapticTeleOperator::~HapticTeleOperator(){
	  NS_LOG_FUNCTION (this);
	  m_socket = 0;
	  m_socket6 = 0;
}

void
HapticTeleOperator::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void
HapticTeleOperator::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  if (m_socket == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket = Socket::CreateSocket (GetNode (), tid);
      InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_port);
      m_socket->Bind (local);
      if (addressUtils::IsMulticast (m_local))
        {
          Ptr<UdpSocket> udpSocket = DynamicCast<UdpSocket> (m_socket);
          if (udpSocket)
            {
              // equivalent to setsockopt (MCAST_JOIN_GROUP)
              udpSocket->MulticastJoinGroup (0, m_local);
            }
          else
            {
              NS_FATAL_ERROR ("Error: Failed to join multicast group");
            }
        }
    }

  if (m_socket6 == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket6 = Socket::CreateSocket (GetNode (), tid);
      Inet6SocketAddress local6 = Inet6SocketAddress (Ipv6Address::GetAny (), m_port);
      m_socket6->Bind (local6);
      if (addressUtils::IsMulticast (local6))
        {
          Ptr<UdpSocket> udpSocket = DynamicCast<UdpSocket> (m_socket6);
          if (udpSocket)
            {
              // equivalent to setsockopt (MCAST_JOIN_GROUP)
              udpSocket->MulticastJoinGroup (0, local6);
            }
          else
            {
              NS_FATAL_ERROR ("Error: Failed to join multicast group");
            }
        }
    }

  m_hapticFileSensor = new HapticFileSensor(m_fileName,HapticFileSensor::FORCEFEEDBACK,m_interval);

  m_socket->SetRecvCallback (MakeCallback (&HapticTeleOperator::HandleRead, this));
  m_socket6->SetRecvCallback (MakeCallback (&HapticTeleOperator::HandleRead, this));
}

void
HapticTeleOperator::StopApplication ()
{
  NS_LOG_FUNCTION (this);

  if (m_socket != 0)
    {
      m_socket->Close ();
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }
  if (m_socket6 != 0)
    {
      m_socket6->Close ();
      m_socket6->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }

  delete m_hapticFileSensor;
  Simulator::Cancel (m_readSensorDataEvent);
}

void
HapticTeleOperator::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

  Ptr<Packet> packet;
  Address from;
  while ((packet = socket->RecvFrom (from)))
    {

	  /*
	   * The HapticTeleOperator needs to learn the address of the corresponding HapticOperator
	   */
	  if(!m_hapticOperatorAddressIsSet){
		  m_hapticOperatorAddress = from;
		  m_hapticOperatorAddressIsSet = true;
		  m_readSensorDataEvent = Simulator::Schedule (Seconds (0.0), &HapticTeleOperator::ReadSensorData, this);
		  ReadSensorData();
	  }


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
    }
}

void
HapticTeleOperator::ReadSensorData ()
{
	  NS_LOG_FUNCTION (this);
	  //NS_ASSERT (m_readSensorDataEvent.IsExpired ());

	  SensorDataSample sds;
	  // on the HapticOperator side we want to send velocity samples
	  if(m_hapticFileSensor->GetNextSensorDataSample(sds,HapticFileSensor::FORCEFEEDBACK)){

		  //
		  //	We made it here => there is still data to send
		  //
		  NS_LOG_DEBUG("At time " << Simulator::Now ().GetSeconds () << " about to send force feedback");

		  //
		  //	Wrap it in a HapticHeader to prepare data to be
		  //	send over the wire
		  //
		  HapticHeader hapticHeader;
		  hapticHeader.SetHapticMessage(sds.getSensorDataString());

		  Ptr<Packet> p = Create<Packet> (hapticHeader.GetSerializedSize());
		  p->AddHeader(hapticHeader);

		  std::stringstream peerAddressStringStream;
		  peerAddressStringStream << " " << InetSocketAddress::ConvertFrom ( m_hapticOperatorAddress).GetIpv4 ();
		  SendPriv(m_socket,p,peerAddressStringStream.str());

//		  if (Ipv4Address::IsMatchingType (m_hapticOperatorAddress))
//		    {
//		      peerAddressStringStream << Ipv4Address::ConvertFrom (m_hapticOperatorAddress);
//		      SendPriv(m_socket,p,peerAddressStringStream.str());
//		    }
//		  else if (Ipv6Address::IsMatchingType (m_hapticOperatorAddress))
//		    {
//		      peerAddressStringStream << Ipv6Address::ConvertFrom (m_hapticOperatorAddress);
//		      SendPriv(m_socket6,p,peerAddressStringStream.str());
//		    }

		      m_readSensorDataEvent = Simulator::Schedule (Seconds(m_interval), &HapticTeleOperator::ReadSensorData, this);

	  }
	  else{
		  NS_LOG_DEBUG("No more SensorDataSample objects to send.");
	  }
}

void HapticTeleOperator::SendPriv (Ptr<Socket> socket, Ptr<Packet> p, std::string peerAddressString){
	NS_LOG_FUNCTION(this);
	if ((socket->SendTo(p,0,m_hapticOperatorAddress)) >= 0)
	    {
	      NS_LOG_INFO ("TraceDelay TX to"
	                                    << peerAddressString << " Uid: "
	                                    << p->GetUid () << " Time: "
	                                    << (Simulator::Now ()).GetSeconds ());

	    }
	  else
	    {
	      NS_LOG_INFO ("Error while sending to"
	                                          << peerAddressString);
	    }
}

} /* namespace ns3 */
