/*
 * haptic-operator.h
 *
 *  Created on: 13 Jul 2016
 *      Author: matthias
 */

#ifndef SRC_KCL_HAPTIC_SIM_MODEL_HAPTIC_OPERATOR_H_
#define SRC_KCL_HAPTIC_SIM_MODEL_HAPTIC_OPERATOR_H_

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

/**
 * Represents the Operator in a Telepresence and Teleaction System (TPTA).
 */
class HapticOperator : public Application
{
public:
	  /**
	   * \brief Get the type ID.
	   * \return the object TypeId
	   */
	  static TypeId GetTypeId (void);

	HapticOperator();
	virtual ~HapticOperator();

	/**
	   * \brief set the remote address and port
	   * \param ip remote IPv4 address
	   * \param port remote port
	   */
	  void SetRemote (Ipv4Address ip, uint16_t port);
	  /**
	   * \brief set the remote address and port
	   * \param ip remote IPv6 address
	   * \param port remote port
	   */
	  void SetRemote (Ipv6Address ip, uint16_t port);
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
  virtual void StopApplication (void);

  void HandleRead(Ptr<Socket> socket);

  /**
   * \brief Send a packet
   */
  void Send (void);

  double m_interval; //!< Packet inter-send time

  Address m_peerAddress; //!< Remote peer address
  uint16_t m_peerPort; //!< Remote peer port

  HapticFileSensor* m_hapticFileSensor;	//!< The interface to the recorded data
  std::string m_fileName;
  std::string m_fileType; //!< Indicates what type of data the provided file contains
  HapticFileSensor::FileType m_hapticSensorFileType;

  Ptr<Socket> m_socket; //!< Socket
  EventId m_sendEvent; //!< Event to send the next packet

  bool m_useDataReductionAlgorithm;
  uint m_deadband;
  std::unique_ptr<HapticDataReductionAlgorithm> m_reduction;

  int m_packetsSent;
};

} /* namespace ns3 */

#endif /* SRC_KCL_HAPTIC_SIM_MODEL_HAPTIC_OPERATOR_H_ */
