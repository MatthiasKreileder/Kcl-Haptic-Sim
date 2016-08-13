
#ifndef SRC_KCL_HAPTIC_SIM_MODEL_PHANTOM_AGENT_H_
#define SRC_KCL_HAPTIC_SIM_MODEL_PHANTOM_AGENT_H_

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/socket.h"

namespace ns3 {

/**
 * \brief This application interacts with the real network traffic sent from a Phantom Omni
 */
class PhantomAgent : public Application {

public:
	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);

	/*
	 * \brief Constructs a PhantomAgent object
	 */
	PhantomAgent ();

	/**
	 * \brief set the remote address and port
	 * \param ip remote IPv4 address
	 * \param port remote port
	 */
	 void SetRemote (Ipv4Address ip, uint16_t port);

	 /**
	  * \brief the PhantomAgent will listen on this address and port for packets
	  */
	 void SetLocal (Ipv4Address localIpv4, uint16_t localPort);

private:

	 virtual void StartApplication (void);

	 void ReadPacketFromChai3D (Ptr<Socket> socket);

	 void ReadFromPacketFromPhantom (Ptr<Socket> socket);

	Address m_peerAddress; //!< Remote peer address
	uint16_t m_peerPort; //!< Remote peer port

	Ptr<Socket> m_socket; //!< Socket

	Ipv4Address m_localIpv4;
	uint16_t m_localPort;

	Ptr<Socket> m_chai3DSocket;

//	/*
//	 * The PhantomAgent needs to listen to receive the
//	 * force feedback from the Chai3DServer
//	 */
//	Address m_localChai3DAddress;
//	uint16_t m_localChai3DPort;
};

}

#endif
