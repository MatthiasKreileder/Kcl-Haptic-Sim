
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

	 void ReadPacketFromPhantom (Ptr<Socket> socket);

	Address m_chai3dAgentAddress; //!< The Chai3DAgent address
	uint16_t m_chai3dAgentPort; //!< The Chai3DAgent port

	/**
	 * \brief the socket to communicate with the phantom omni
	 */
	Ptr<Socket> m_socketForCommunicationWithPhantomOmni; //!< Socket to communicate with the phantom omni

	Ipv4Address m_localIpv4ForCommunicationWithPhantomOmni;
	uint16_t m_localPort;

	Ptr<Socket> m_chai3DSocket;

//	/*
//	 * The PhantomAgent needs to listen to receive the
//	 * force feedback from the Chai3DServer
//	 */

	/**
	 * \brief The address of the phantom omni - will be learned on the fly
	 *
	 * This member holds the address of the real device that is sending data to ns-3
	 * It is by no means bound to using a phantom omni but this class was developed to use it
	 * for that purpose => the naming reflects the intended use
	 */
	Address m_phantomAddress;
//	uint16_t m_localChai3DPort;
};

}

#endif
