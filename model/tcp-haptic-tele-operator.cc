/*
 * tcp-haptic-tele-operator.cc
 *
 *  Created on: 17 Aug 2016
 *      Author: matthias
 */

#include "tcp-haptic-tele-operator.h"
#include "haptic-header.h"
#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/address-utils.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("TcpHapticTeleOperator");

NS_OBJECT_ENSURE_REGISTERED (TcpHapticTeleOperator);

TypeId
TcpHapticTeleOperator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TcpHapticTeleOperator")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<TcpHapticTeleOperator> ()
    .AddAttribute ("Port", "Port on which we listen for incoming packets.",
                   UintegerValue (9),
                   MakeUintegerAccessor (&TcpHapticTeleOperator::m_port),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("SamplingIntervalSeconds",
                   "The time in seconds between two data samples.", DoubleValue (0.001),
                   MakeDoubleAccessor (&TcpHapticTeleOperator::m_interval),
                   MakeDoubleChecker<double>())
    .AddAttribute ("FileName",
                   "The name of the file which contains the recorded haptic data.",
                   StringValue ("src/Kcl-Haptic-Sim/test/test_force.txt"),
                   MakeStringAccessor (&TcpHapticTeleOperator::m_fileName),
                   MakeStringChecker()
 			   	  )
//	.AddAttribute ("TcpOperatorAddress",
//	               "the address of the TcpHapticOperator",
//	               Ipv4AddressValue (),
//	               MakeIpv4AddressAccessor (&TcpHapticTeleOperator::m_tcpHapticOperatorAddress),
//	               MakeAddressChecker ())
	.AddAttribute ("ApplyDataReduction",
				   "Set to true if data reduction should be applied.",
				   BooleanValue (false),
				   MakeBooleanAccessor(&TcpHapticTeleOperator::m_useDataReductionAlgorithm),
				   MakeBooleanChecker()
				  )
  ;
  return tid;
}

TcpHapticTeleOperator::TcpHapticTeleOperator()
	:	m_receivedFirstPacketFromTcpHapticOperator(false)
{
	// TODO Auto-generated constructor stub
}

TcpHapticTeleOperator::~TcpHapticTeleOperator() {
	// TODO Auto-generated destructor stub
}

void TcpHapticTeleOperator::SetTcpHapticTeleOperator (Ipv4Address ipv4){
	m_tcpHapticOperatorAddress = ipv4;
}

void
TcpHapticTeleOperator::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void
TcpHapticTeleOperator::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

  Ptr<Packet> packet;
  Address from;
  while ((packet = socket->RecvFrom (from)))
    {

//	  if(InetSocketAddress::ConvertFrom (from).GetIpv4 ()
//			  !=
//		 InetSocketAddress::ConvertFrom (m_tcpHapticOperatorAddress).GetIpv4 ())
//	  {
//		  NS_LOG_DEBUG(this << "TcpHapticTeleOperator received address from un-known source: " << InetSocketAddress::ConvertFrom (from).GetIpv4 ());
//		  return;
//	  }

	  if (!m_receivedFirstPacketFromTcpHapticOperator){
		  m_receivedFirstPacketFromTcpHapticOperator = true;
		  ReadSensorData();
	  }

//      NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s server received " << packet->GetSize () << " bytes from " <<
//                   InetSocketAddress::ConvertFrom (from).GetIpv4 () << " port " <<
//                   InetSocketAddress::ConvertFrom (from).GetPort ());
    }
}


void
TcpHapticTeleOperator::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  if (m_useDataReductionAlgorithm)
	  m_reduction = std::unique_ptr<HapticDataReductionAlgorithm>{new  HapticDataReductionAlgorithm(0.05)};

  if (m_socket == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::TcpSocketFactory");
      m_socket = Socket::CreateSocket (GetNode (), tid);
      InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_port);
      m_socket->Bind (local);
      m_socket->Listen();

    }

  m_hapticFileSensor = new HapticFileSensor(m_fileName);

  m_socket->SetAcceptCallback(MakeCallback (&TcpHapticTeleOperator::HandleAcceptRequest, this),
		  	  	  	  	  	  MakeCallback (&TcpHapticTeleOperator::HandleAccept, this));

}

void
TcpHapticTeleOperator::StopApplication ()
{
  NS_LOG_FUNCTION (this);

  if (m_socket != 0)
    {
      m_socket->Close ();
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }

  delete m_hapticFileSensor;
  NS_LOG_DEBUG("Packet sent: " << m_packetsSent);
  Simulator::Cancel (m_readSensorDataEvent);
}

void
TcpHapticTeleOperator::ReadSensorData ()
{
	NS_LOG_FUNCTION (this);

	SensorDataSample sds;
	// on the TcpHapticTeleOperator side we want to send force-feedback samples
	if(m_hapticFileSensor->GetNextSensorDataSample(sds,HapticFileSensor::FORCEFEEDBACK))
	{

		if (m_useDataReductionAlgorithm){
			if (!m_reduction->needsToBeTransmitted(sds.getSensorDataVector())){
				m_readSensorDataEvent = Simulator::Schedule (Seconds( m_interval), &TcpHapticTeleOperator::ReadSensorData, this);
				return;
			 }
		}
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

		  //std::stringstream peerAddressStringStream;
		  //peerAddressStringStream << " " << InetSocketAddress::ConvertFrom ( m_tcpHapticOperatorAddress).GetIpv4 ();
		  //SendPriv(m_socket,p,peerAddressStringStream.str());
		  SendPriv(m_tcpHapticOperatorSocket,p,"test");

		  m_packetsSent++;
		  //NS_LOG_DEBUG("Packet sent: " << m_packetsSent);
		      m_readSensorDataEvent = Simulator::Schedule (Seconds(m_interval), &TcpHapticTeleOperator::ReadSensorData, this);

	  }
	  else
	  {
		  NS_LOG_DEBUG("No more SensorDataSample objects to send.");
	  }
}

void TcpHapticTeleOperator::SendPriv (Ptr<Socket> socket, Ptr<Packet> p, std::string peerAddressString){
	NS_LOG_FUNCTION(this);
	if ((socket->SendTo(p,0,m_tcpHapticOperatorAddress)) >= 0)
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

void TcpHapticTeleOperator::HandleAccept (Ptr<Socket> s, const Address& from)
{
  NS_LOG_FUNCTION (this << s << from);
  m_tcpHapticOperatorSocket = s;
  m_tcpHapticOperatorSocket->SetRecvCallback (MakeCallback (&TcpHapticTeleOperator::HandleRead, this));
}

bool TcpHapticTeleOperator::HandleAcceptRequest (Ptr<Socket> socket, const Address& from){
	NS_LOG_FUNCTION(from);
			  if(InetSocketAddress::ConvertFrom (from).GetIpv4 ()
				  !=
			 m_tcpHapticOperatorAddress)
		  {
			  NS_LOG_DEBUG(this << "TcpHapticTeleOperator received address from un-known source: " << InetSocketAddress::ConvertFrom (from).GetIpv4 ());
			  return false;;
		  }

	return true;
}

} /* namespace ns3 */
