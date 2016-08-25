
//
//	Idea and code based on: https://www.nsnam.org/doxygen/udp-server_8cc_source.html
//
#ifndef SRC_KCL_HAPTIC_SIM_MODEL_TCP_HAPTIC_TELE_OPERATOR_H_
#define SRC_KCL_HAPTIC_SIM_MODEL_TCP_HAPTIC_TELE_OPERATOR_H_

#include <stdlib.h>
#include <string>
#include <sstream>
#include <memory>

#include "ns3/socket.h"
#include "ns3/address.h"
#include "ns3/application.h"

#include "ns3/haptic-file-sensor.h"
#include "haptic-data-reduction-algorithm.h"

namespace ns3 {

class TcpHapticTeleOperator : public Application  {
public:

	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);

	TcpHapticTeleOperator();
	virtual ~TcpHapticTeleOperator();

	void SetTcpHapticTeleOperator (Ipv4Address ipv4);

protected:
  virtual void DoDispose (void);

private:
    virtual void StartApplication (void);
    virtual void StopApplication (void);
    void HandleAccept (Ptr<Socket> socket, const Address& from);
    bool HandleAcceptRequest (Ptr<Socket> socket, const Address& from);
    void HandleRead (Ptr<Socket> socket);
  	void ReadSensorData ();
  	void SendPriv (Ptr<Socket> socket, Ptr<Packet> p, std::string peerAddressString);
	uint16_t m_port; //!< Port on which we listen for incoming packets.
	Ptr<Socket> m_socket; //!< IPv4 Socket
	Ptr<Socket> m_tcpHapticOperatorSocket;

	Ipv4Address m_tcpHapticOperatorAddress; //!< The HapticOperator's address
	Address m_from;

	HapticFileSensor* m_hapticFileSensor;	//!< The interface to the recorded data
	double m_interval; //!< Packet inter-send time
	std::string m_fileName; //!< holds the name (+ path) of the file which contains the recorded force feedback data samples

	EventId m_readSensorDataEvent;

	bool m_useDataReductionAlgorithm;
	uint m_deadband;
	std::unique_ptr<HapticDataReductionAlgorithm> m_reduction;

	int m_packetsSent;
	bool m_receivedFirstPacketFromTcpHapticOperator; //! Start sending force samples after the first position/velocity sample
};

} /* namespace ns3 */

#endif /* SRC_KCL_HAPTIC_SIM_MODEL_TCP_HAPTIC_TELE_OPERATOR_H_ */
