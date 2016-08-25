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
#include <iostream>
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("Chai3dServer");

NS_OBJECT_ENSURE_REGISTERED (Chai3dServer);

Chai3dServer::Chai3dServer() {
	NS_LOG_FUNCTION(this);
	m_chai3dWrapperProg = "";
	m_port = -1;
	m_socket = 0;
	m_nph = 0;

//	namedPipes_lock.lock();
//	namedPipes_lock.unlock();

	m_sharedMemHandler = new SharedMemoryHandler();

}

Chai3dServer::~Chai3dServer() {
	NS_LOG_FUNCTION(this);
	delete m_sharedMemHandler;
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
                   StringValue ("/home/matthias/Development/chai3d-3.0.0/bin/04-shapes"),
                   MakeStringAccessor (&Chai3dServer::m_chai3dWrapperProg),
                   MakeStringChecker()
 			   	  )
	.AddAttribute ("NamedPipesFolder",
				   "The folder inside your file system where the two named pipes will be created.",
				   StringValue ("/home/matthias/Development/Learning-Named-Pipes/"),
				   MakeStringAccessor (&Chai3dServer::m_namedPipesFolder),
				   MakeStringChecker()
				   )
	.AddAttribute ("PhantomAgentAddress",
	               "The PhantomAgent Address",
	               AddressValue (),
	               MakeAddressAccessor (&Chai3dServer::m_phantomAgentAddress),
	               MakeAddressChecker ())
	.AddAttribute ("PhantomAgentPort",
	               "The PhantomAgent port",
	               UintegerValue (0),
	               MakeUintegerAccessor (&Chai3dServer::m_phantomAgentPort),
			       MakeUintegerChecker<uint16_t> ())
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
	  NS_LOG_FUNCTION("Packet from: " << from);
	  HapticHeader hapticHeader;

	  packet->RemoveHeader(hapticHeader);

	  //NS_LOG_DEBUG(hapticHeader.GetHapticMessage());

	  std::string s = hapticHeader.GetHapticMessage();

	  //NS_LOG_DEBUG("Sending haptic message to chai3d");

	  //m_nph->SafeWrite(s);

	  ////////////////////////////////////////////////////
	  m_sharedMemHandler->KclSafeWrite(s);
	  std::string answer;
	  //sleep(0.0001);
	  m_sharedMemHandler->KclSafeRead(answer);


	  /*
	   * Extract message
	   */
	  std::size_t firstHashTag = answer.find_first_of('#');
	  if(firstHashTag == 0){
		  // no force sample from chai3d available => nothing for us to do

		 NS_LOG_DEBUG("Nothing to do in this cycle");
	  }
	  else{
		  std::string msg_from_chai3d =  answer.substr(0,firstHashTag);

		  NS_LOG_DEBUG("Received " << msg_from_chai3d);

		  HapticHeader hapticHeaderReturnPacket;
		  hapticHeaderReturnPacket.SetHapticMessage(msg_from_chai3d);

		  Ptr<Packet> packetForPhantom = Create<Packet> ();
		  packetForPhantom->AddHeader(hapticHeaderReturnPacket);


		  m_socket->SendTo(packetForPhantom,0,from);

	  }

	  ////////////////////////////////////////////////////



    }
}

void
Chai3dServer::Setup(){




//	/*
//	 * NS-3 and CHAI3-D will communicate via two uni-directional
//	 * FIFOs (named pipes).
//	 */
//    std::string ns3ToChai3DPipeName = "/home/matthias/Development/Learning-Named-Pipes/ns3ToChai3D";
//    std::string chai3dToNs3Pipe = "/home/matthias/Development/Learning-Named-Pipes/chai3dToNs3";

	/*
	 * We need a '/' character at the end - let's add that if needed
	 */
	if(m_namedPipesFolder[m_namedPipesFolder.size() - 1] != '/'){
		m_namedPipesFolder.append("/");
	}

	/*
	 * NS-3 and CHAI3-D will communicate via two uni-directional
	 * FIFOs (named pipes).
	 */
    std::string ns3ToChai3DPipeName = m_namedPipesFolder;
    		ns3ToChai3DPipeName.append("ns3ToChai3D");
    std::string chai3dToNs3Pipe = m_namedPipesFolder;
    		chai3dToNs3Pipe.append("chai3dToNs3");

    m_fullNameNamedPipeNs3ToChaid3D = ns3ToChai3DPipeName;
    m_fullNameNamedPipeChai3dToNs3 = chai3dToNs3Pipe;

    mkfifo(ns3ToChai3DPipeName.c_str(), 0666);
    mkfifo(chai3dToNs3Pipe.c_str(), 0666);

	pid_t pid;

	/* Create the child process. */
	pid = fork ();
	if (pid == (pid_t) 0)
	{
		NS_LOG_DEBUG("Launching haptic mock");

//        execl("/home/matthias/Development/chai3d-3.0.0/bin/04-shapes",
//              "/home/matthias/Development/chai3d-3.0.0/bin/04-shapes",
//              ns3ToChai3DPipeName.c_str(),
//              chai3dToNs3Pipe.c_str(),
//              NULL);
        execl(m_chai3dWrapperProg.c_str(),
        	  m_chai3dWrapperProg.c_str(),
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
	unlink(m_fullNameNamedPipeNs3ToChaid3D.c_str());
	unlink(m_fullNameNamedPipeChai3dToNs3.c_str());
}

} /* namespace ns3 */
