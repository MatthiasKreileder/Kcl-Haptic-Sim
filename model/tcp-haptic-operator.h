
//
//	Code and idea based on: https://www.nsnam.org/doxygen/udp-client_8cc_source.html
//
#ifndef SRC_KCL_HAPTIC_SIM_MODEL_TCP_HAPTIC_OPERATOR_H_
#define SRC_KCL_HAPTIC_SIM_MODEL_TCP_HAPTIC_OPERATOR_H_

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/socket.h"

#include "haptic-file-sensor.h"
#include "haptic-data-reduction-algorithm.h"
#include <string>
#include <memory>

namespace ns3 {

class TcpHapticOperator : public Application{
public:
    /**
	  * \brief Get the type ID.
	  * \return the object TypeId
	  */
	static TypeId GetTypeId (void);
	TcpHapticOperator();
	virtual ~TcpHapticOperator();
	  /**
	   * \brief set the remote address and port
	   * \param ip remote IP address
	   * \param port remote port
	   */
	  void SetRemote (Address ip, uint16_t port);

protected:
  virtual void DoDispose (void);

private:

  virtual void StartApplication (void);
  void HandleRead(Ptr<Socket> socket);
  void Send (void);


	  double m_interval; //!< Packet inter-send time
	  Address m_tcpHapticTeleOperatorAddress; //!< the address of the TcpHapticTeleOperator
	  uint16_t m_tcpHapticTeleOperatorPort; //!< the port of the TcpHapticTeleOperator

	  HapticFileSensor* m_hapticFileSensor;	//!< The interface to the recorded data

	  Ptr<Socket> m_socket; //!< Socket
	  EventId m_sendEvent; //!< Event to send the next packet

	  bool m_useDataReductionAlgorithm;
	  int m_deadband;
	  std::unique_ptr<HapticDataReductionAlgorithm> m_reduction;

	  int m_packetsSent;

	  std::string m_positionFile;
	  std::string m_velocityFile;
};

} /* namespace ns3 */

#endif /* SRC_KCL_HAPTIC_SIM_MODEL_TCP_HAPTIC_OPERATOR_H_ */
