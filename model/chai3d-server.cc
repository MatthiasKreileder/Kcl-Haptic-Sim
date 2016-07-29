/*
 * chai3d-server.cc
 *
 *  Created on: 25 Jul 2016
 *      Author: matthias
 */

#include "chai3d-server.h"
#include "haptic-header.h"

#include "ns3/uinteger.h"
#include "ns3/packet.h"
#include "ns3/udp-socket.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("Chai3dServer");

NS_OBJECT_ENSURE_REGISTERED (Chai3dServer);

Chai3dServer::Chai3dServer() {
	NS_LOG_FUNCTION(this);
	m_chai3dWrapperProg = "";
	m_port = -1;
	m_socket = 0;
	m_nph = 0;
}

Chai3dServer::~Chai3dServer() {
	NS_LOG_FUNCTION(this);
}

TypeId
Chai3dServer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Chai3dServer")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<Chai3dServer> ()
    .AddAttribute ("Port", "Port on which we listen for incoming packets.",
                   UintegerValue (9),
                   MakeUintegerAccessor (&Chai3dServer::m_port),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("Chai3dWrapper",
                   "The program's name (+ path in your file system) which interacts with Chai3d.",
                   StringValue ("/home/matthias/Development/Workspace/CHAI3D-Mock/Debug/CHAI3D-Mock"),
                   MakeStringAccessor (&Chai3dServer::m_chai3dWrapperProg),
                   MakeStringChecker()
 			   	  )
  ;
  return tid;
}

void
Chai3dServer::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void
Chai3dServer::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

  Ptr<Packet> packet;
  Address from;
  while ((packet = socket->RecvFrom (from)))
    {

	  HapticHeader hapticHeader;

	  packet->RemoveHeader(hapticHeader);

	  NS_LOG_DEBUG(hapticHeader.GetHapticMessage());

	  std::string s = hapticHeader.GetHapticMessage();

	  NS_LOG_DEBUG("Sending haptic message to chai3d");
	  m_nph->SafeWrite(s);

	  std::string msg_from_chai3d;
	  m_nph->SafeRead(msg_from_chai3d);
	  NS_LOG_DEBUG("Received " << msg_from_chai3d);


    }
}

void
Chai3dServer::Setup(){


	/*
	 * NS-3 and CHAI3-D will communicate via two uni-directional
	 * FIFOs (named pipes).
	 */
    std::string ns3ToChai3DPipeName = "/home/matthias/Development/Learning-Named-Pipes/ns3ToChai3D";
    std::string chai3dToNs3Pipe = "/home/matthias/Development/Learning-Named-Pipes/chai3dToNs3";

    mkfifo(ns3ToChai3DPipeName.c_str(), 0666);
    mkfifo(chai3dToNs3Pipe.c_str(), 0666);

	pid_t pid;

	/* Create the child process. */
	pid = fork ();
	if (pid == (pid_t) 0)
	{
		NS_LOG_DEBUG("Launching haptic mock");

        execl("/home/matthias/Development/Learning-Named-Pipes/chai3d-mock/build/Chai3D-Mock",
              "/home/matthias/Development/Learning-Named-Pipes/chai3d-mock/build/Chai3D-Mock",
              ns3ToChai3DPipeName.c_str(),
              chai3dToNs3Pipe.c_str(),
              NULL);

		   NS_LOG_DEBUG("Launching haptic mock failed");
	     }
	   else if (pid < (pid_t) 0)
	     {
	       /* The fork failed. */
		   NS_ABORT_MSG("Fork failed " << EXIT_FAILURE);
	     }
	   else
	     {
	       /* This is the parent process.
	          Close other end first. */
	       m_nph = new NamedPipeHandler (ns3ToChai3DPipeName,chai3dToNs3Pipe,true);

//	        for(int i = 0; i < 10; i++){
//	            std::string s("Best wishes from your parent");
//	            m_nph->SafeWrite(s);
//
//	            std::string msg_return;
//	            m_nph->SafeRead(msg_return);
//	            NS_LOG_DEBUG(msg_return);
//
//	        }

	     }

}

void
Chai3dServer::StartApplication (void)
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

  m_socket->SetRecvCallback (MakeCallback (&Chai3dServer::HandleRead, this));
}

void
Chai3dServer::StopApplication (void){
	unlink("/home/matthias/Development/Learning-Named-Pipes/ns3ToChai3D");
	unlink("/home/matthias/Development/Learning-Named-Pipes/chai3dToNs3");
}

} /* namespace ns3 */
