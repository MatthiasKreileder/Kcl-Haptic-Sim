/*
 * haptic-operator-helper.h
 *
 *  Created on: 13 Jul 2016
 *      Author: matthias
 */

#ifndef SRC_KCL_HAPTIC_SIM_HELPER_HAPTIC_OPERATOR_HELPER_H_
#define SRC_KCL_HAPTIC_SIM_HELPER_HAPTIC_OPERATOR_HELPER_H_

#include <stdint.h>
#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"


namespace ns3 {

class HapticOperatorHelper {
public:
  /**
   * Create HapticOperatorHelper which will make life easier for people trying
   * to set up simulations with echos.
   *
   * \param ip The IP address of the remote HapticTeleOperator
   * \param port The port number of the remote HapticTeleOperator
   */
	HapticOperatorHelper (Address ip, uint16_t port);
  /**
   * Create HapticOperatorHelper which will make life easier for people trying
   * to set up simulations with echos.
   *
   * \param ip The IPv4 address of the remote HapticTeleOperator
   * \param port The port number of the remote HapticTeleOperator
   */
	HapticOperatorHelper (Ipv4Address ip, uint16_t port);
  /**
   * Create HapticOperatorHelper which will make life easier for people trying
   * to set up simulations with echos.
   *
   * \param ip The IPv6 address of the remote HapticTeleOperator
   * \param port The port number of the remote HapticTeleOperator
   */
	HapticOperatorHelper (Ipv6Address ip, uint16_t port);

  /**
   * Record an attribute to be set in each Application after it is is created.
   *
   * \param name the name of the attribute to set
   * \param value the value of the attribute to set
   */
  void SetAttribute (std::string name, const AttributeValue &value);

  /**
   * Create a HapticOperator application on the specified node.  The Node
   * is provided as a Ptr<Node>.
   *
   * \param node The Ptr<Node> on which to create the HapticOperator Application.
   *
   * \returns An ApplicationContainer that holds a Ptr<Application> to the
   *          application created
   */
  ApplicationContainer Install (Ptr<Node> node) const;

  /**
   * \param c the nodes
   *
   * Create one HapticOperator application on each of the input nodes
   *
   * \returns the applications created, one application per input node.
   */
  ApplicationContainer Install (NodeContainer c) const;

private:
  /**
   * Install an ns3::HapticOperator on the node configured with all the
   * attributes set with SetAttribute.
   *
   * \param node The node on which an HapticOperator will be installed.
   * \returns Ptr to the application installed.
   */
  Ptr<Application> InstallPriv (Ptr<Node> node) const;
  ObjectFactory m_factory; //!< Object factory.
};

} /* namespace ns3 */

#endif /* SRC_KCL_HAPTIC_SIM_HELPER_HAPTIC_OPERATOR_HELPER_H_ */
