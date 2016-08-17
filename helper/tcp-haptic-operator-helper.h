/*
 * tcp-haptic-operator-helper.h
 *
 *  Created on: 17 Aug 2016
 *      Author: matthias
 */

#ifndef SRC_KCL_HAPTIC_SIM_HELPER_TCP_HAPTIC_OPERATOR_HELPER_H_
#define SRC_KCL_HAPTIC_SIM_HELPER_TCP_HAPTIC_OPERATOR_HELPER_H_
#include <stdint.h>
#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"

namespace ns3 {

class TcpHapticOperatorHelper {
public:
	TcpHapticOperatorHelper();
	virtual ~TcpHapticOperatorHelper();

  /**
   * Create HapticOperatorHelper which will make life easier for people trying
   * to set up simulations with echos.
   *
   * \param ip The IP address of the remote HapticTeleOperator
   * \param port The port number of the remote HapticTeleOperator
   */
	TcpHapticOperatorHelper (Address ip, uint16_t port);

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

	  void SetAttribute (std::string name,const AttributeValue &value);
private:
	  Ptr<Application> InstallPriv (Ptr<Node> node) const;
	ObjectFactory m_factory; //!< Object factory.
};

} /* namespace ns3 */

#endif /* SRC_KCL_HAPTIC_SIM_HELPER_TCP_HAPTIC_OPERATOR_HELPER_H_ */
