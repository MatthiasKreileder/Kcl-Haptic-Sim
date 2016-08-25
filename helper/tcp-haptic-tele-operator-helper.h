
//
// Code and idea based on: https://www.nsnam.org/doxygen/udp-echo-helper_8cc_source.html
//
#ifndef SRC_KCL_HAPTIC_SIM_HELPER_TCP_HAPTIC_TELE_OPERATOR_HELPER_H_
#define SRC_KCL_HAPTIC_SIM_HELPER_TCP_HAPTIC_TELE_OPERATOR_HELPER_H_

#include <stdint.h>
#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "ns3/ipv4-address.h"

namespace ns3 {

/**
 * \brief A TcpHapticTeleOperatorHelper object simplifies the process of creating and
 * installing a TcpHapticTeleOperator application on a node.
 */
class TcpHapticTeleOperatorHelper {
public:
	TcpHapticTeleOperatorHelper(uint16_t port, Ipv4Address tcpHapticOperatorAddress);
	virtual ~TcpHapticTeleOperatorHelper();

	  /**
	   * Record an attribute to be set in each Application after it is is created.
	   *
	   * \param name the name of the attribute to set
	   * \param value the value of the attribute to set
	   */
	  void SetAttribute (std::string name, const AttributeValue &value);

  ApplicationContainer Install (Ptr<Node> node) const;

private:

  Ptr<Application> InstallPriv (Ptr<Node> node) const;

  Ipv4Address m_ipv4Addr;
  ObjectFactory m_factory; //!< Object factory.public:

};

} /* namespace ns3 */

#endif /* SRC_KCL_HAPTIC_SIM_HELPER_TCP_HAPTIC_TELE_OPERATOR_HELPER_H_ */
