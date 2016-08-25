
//
// Code and idea based on: https://www.nsnam.org/doxygen/udp-echo-helper_8cc_source.html
//
#ifndef SRC_KCL_HAPTIC_SIM_HELPER_HAPTIC_TELE_OPERATOR_HELPER_H_
#define SRC_KCL_HAPTIC_SIM_HELPER_HAPTIC_TELE_OPERATOR_HELPER_H_

#include <stdint.h>
#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"


namespace ns3 {

class HapticTeleOperatorHelper
{
public:
  /**
   * Create HapticTeleOperatorHelper which will make life easier for people trying
   * to set up simulations.
   *
   * \param port The port the server will wait on for incoming packets
   */
	HapticTeleOperatorHelper (uint16_t port);

	  /**
	   * Record an attribute to be set in each Application after it is is created.
	   *
	   * \param name the name of the attribute to set
	   * \param value the value of the attribute to set
	   */
	  void SetAttribute (std::string name, const AttributeValue &value);

  /**
   * Create a HapticTeleOperator Application on the specified Node.
   *
   * \param node The node on which to create the Application.  The node is
   *             specified by a Ptr<Node>.
   *
   * \returns An ApplicationContainer holding the Application created,
   */
  ApplicationContainer Install (Ptr<Node> node) const;

private:
  /**
   * Install an ns3::HapticTeleOperator on the node configured with all the
   * attributes set with SetAttribute.
   *
   * \param node The node on which an UdpEchoServer will be installed.
   * \returns Ptr to the application installed.
   */
  Ptr<Application> InstallPriv (Ptr<Node> node) const;

  ObjectFactory m_factory; //!< Object factory.

};

} /* namespace ns3 */

#endif /* SRC_KCL_HAPTIC_SIM_HELPER_HAPTIC_TELE_OPERATOR_HELPER_H_ */
