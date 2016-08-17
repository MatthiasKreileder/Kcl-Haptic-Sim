/*
 * tcp-haptic-operator.cc
 *
 *  Created on: 17 Aug 2016
 *      Author: matthias
 */

#include "tcp-haptic-operator.h"
#include "haptic-header.h"
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
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

NS_LOG_COMPONENT_DEFINE ("TcpHapticOperator");

NS_OBJECT_ENSURE_REGISTERED (TcpHapticOperator);

TypeId
TcpHapticOperator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TcpHapticOperator")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<TcpHapticOperator> ()
    .AddAttribute ("SamplingIntervalSeconds",
                   "The time in seconds between two data samples.", DoubleValue (0.001),
                   MakeDoubleAccessor (&TcpHapticOperator::m_interval),
                   MakeDoubleChecker<double>())
    .AddAttribute ("TcpTeleOperatorAddress",
                   "the address of the TcpHapticTeleOperator",
                   AddressValue (),
                   MakeAddressAccessor (&TcpHapticOperator::m_tcpHapticTeleOperatorAddress),
                   MakeAddressChecker ())
    .AddAttribute ("TcpTeleOperatorPort", "the port of the TcpHapticTeleOperator",
                   UintegerValue (100),
                   MakeUintegerAccessor (&TcpHapticOperator::m_tcpHapticTeleOperatorPort),
                   MakeUintegerChecker<uint16_t> ())
	.AddAttribute ("PositionFile",
				   "The file that contains the position samples.",
				   StringValue ("src/Kcl-Haptic-Sim/test/position.txt"),
				   MakeStringAccessor (&TcpHapticOperator::m_positionFile),
				   MakeStringChecker()
				   )
	.AddAttribute ("VelocityFile",
				   "The file that contains the velocity samples.",
				   StringValue ("src/Kcl-Haptic-Sim/test/faleVosition.txt"),
                   MakeStringAccessor (&TcpHapticOperator::m_velocityFile),
                   MakeStringChecker()
				   )
	.AddAttribute  ("ApplyDataReduction",
				   "Set to true if data reduction should be applied.",
				   BooleanValue (false),
				   MakeBooleanAccessor(&TcpHapticOperator::m_useDataReductionAlgorithm),
				   MakeBooleanChecker()
				   )

  ;
  return tid;
}

TcpHapticOperator::TcpHapticOperator() {
	// TODO Auto-generated constructor stub

}

TcpHapticOperator::~TcpHapticOperator() {
	// TODO Auto-generated destructor stub
}
void
TcpHapticOperator::SetRemote (Address ip, uint16_t port)
{
  NS_LOG_FUNCTION (this << ip << port);
  m_tcpHapticTeleOperatorAddress = ip;
  m_tcpHapticTeleOperatorPort = port;
}

void
TcpHapticOperator::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void
TcpHapticOperator::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  if (m_useDataReductionAlgorithm){
	  m_reduction = std::unique_ptr<HapticDataReductionAlgorithm>{new  HapticDataReductionAlgorithm(0.05)};
  }

  if (m_socket == 0)
    {

	  m_socket = Socket::CreateSocket (GetNode(), TcpSocketFactory::GetTypeId ());
	  m_socket->Bind();
	  m_socket->Connect(InetSocketAddress (Ipv4Address::ConvertFrom(m_tcpHapticTeleOperatorAddress), m_tcpHapticTeleOperatorPort));
    }

  m_socket->SetRecvCallback (MakeCallback (&TcpHapticOperator::HandleRead, this));


  m_hapticFileSensor = new HapticFileSensor(m_positionFile,m_velocityFile);



  m_sendEvent = Simulator::Schedule (Seconds (0.0), &TcpHapticOperator::Send, this);
}

void TcpHapticOperator::HandleRead(Ptr<Socket> socket){
	  NS_LOG_FUNCTION (this << socket);

	  Ptr<Packet> packet;
	  Address from;
	  while ((packet = socket->RecvFrom (from)))
	    {
	      if (InetSocketAddress::IsMatchingType (from))
	        {
	          NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s TcpHapticOperator received " << packet->GetSize () << " bytes from " <<
	                       InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " <<
	                       InetSocketAddress::ConvertFrom (from).GetPort ());
	        }
	      else if (Inet6SocketAddress::IsMatchingType (from))
	        {
	          NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s TcpHapticOperator received " << packet->GetSize () << " bytes from " <<
	                       Inet6SocketAddress::ConvertFrom (from).GetIpv6 () << " port " <<
	                       Inet6SocketAddress::ConvertFrom (from).GetPort ());
	        }
	    }
}

void
TcpHapticOperator::Send (void)
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
		  SensorDataSample velocity;
		  m_hapticFileSensor->GetNextSensorDataSample(velocity,HapticFileSensor::VELOCITY);
		  if (!m_reduction->needsToBeTransmitted(velocity.getSensorDataVector())){
			  m_sendEvent = Simulator::Schedule (Seconds( m_interval), &TcpHapticOperator::Send, this);
			  return;
		  }
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
	  peerAddressStringStream << Ipv4Address::ConvertFrom (m_tcpHapticTeleOperatorAddress);

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

	      m_sendEvent = Simulator::Schedule (Seconds( m_interval), &TcpHapticOperator::Send, this);
	      m_packetsSent++;
	      NS_LOG_DEBUG("Packets sent: " << m_packetsSent);

  }
  else{
	  NS_LOG_DEBUG("No more SensorDataSample objects to send.");
  }





}

} /* namespace ns3 */
