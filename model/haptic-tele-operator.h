/*
 * haptic-tele-operator.h
 *
 *  Created on: 19 Jul 2016
 *      Author: matthias
 */

#ifndef SRC_KCL_HAPTIC_SIM_MODEL_HAPTIC_TELE_OPERATOR_H_
#define SRC_KCL_HAPTIC_SIM_MODEL_HAPTIC_TELE_OPERATOR_H_

#include <stdlib.h>
#include <string>
#include <sstream>

#include "ns3/socket.h"
#include "ns3/address.h"
#include "ns3/application.h"

#include "ns3/haptic-file-sensor.h"

namespace ns3 {

/**
 * Represents the TeleOperator in a Telepresence and Teleaction System (TPTA).
 *
 * A HapticTeleOperator waits for a HapticOperator to send Position/Velocity samples. It does nothing before it receives its first position/velocity packet.
 * It will from point of the first receiving event send force feedback data samples to the HapticOperator. Please note that this is a 1-1 relationship, i.e. one
 * HapticOperator connects to one HapticOperator.
 */
class HapticTeleOperator : public Application {
public:

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  HapticTeleOperator();
  virtual ~HapticTeleOperator();

protected:
  virtual void DoDispose (void);

private:

  virtual void StartApplication (void);
  virtual void StopApplication (void);

  /**
   * \brief Handle a packet reception.
   *
   * This function is called by lower layers.
   *
   * \param socket the socket the packet was received to.
   */
  void HandleRead (Ptr<Socket> socket);

  /**
   * \brief Read the next data sample from the sensor and schedule a transmission if needed
   *
   */
  void ReadSensorData ();

  void SendPriv (Ptr<Socket> socket, Ptr<Packet> p, std::string peerAddressString);

  uint16_t m_port; //!< Port on which we listen for incoming packets.
  Ptr<Socket> m_socket; //!< IPv4 Socket
  Ptr<Socket> m_socket6; //!< IPv6 Socket
  Address m_local; //!< local multicast address

  Address m_hapticOperatorAddress; //!< The HapticOperator's address
  bool m_hapticOperatorAddressIsSet;	//!< Is m_hapticOperatorAddress set to the Address of a HapticOperator

  HapticFileSensor* m_hapticFileSensor;	//!< The interface to the recorded data
  double m_interval; //!< Packet inter-send time
  std::string m_fileName; //!< holds the name (+ path) of the file which contains the recorded force feedback data samples

  EventId m_readSensorDataEvent;
};

} /* namespace ns3 */

#endif /* SRC_KCL_HAPTIC_SIM_MODEL_HAPTIC_TELE_OPERATOR_H_ */
