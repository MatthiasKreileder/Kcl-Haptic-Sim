/*
 * phantom-agent-helper.h
 *
 *  Created on: 8 Aug 2016
 *      Author: matthias
 */
//
// Code and idea based on: https://www.nsnam.org/doxygen/udp-echo-helper_8cc_source.html
//
#ifndef SRC_KCL_HAPTIC_SIM_HELPER_PHANTOM_AGENT_HELPER_H_
#define SRC_KCL_HAPTIC_SIM_HELPER_PHANTOM_AGENT_HELPER_H_

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/socket.h"
#include <stdint.h>
#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
namespace ns3 {

class PhantomAgentHelper {
public:
	PhantomAgentHelper(Ipv4Address localIpv4, uint16_t localPort);

	  /**
	   * Record an attribute to be set in each Application after it is is created.
	   *
	   * \param name the name of the attribute to set
	   * \param value the value of the attribute to set
	   */
	  void SetAttribute (std::string name, const AttributeValue &value);

	virtual ~PhantomAgentHelper();

	/**
	   * Create a PhantomAgent application on the specified node.  The Node
	   * is provided as a Ptr<Node>.
	   *
	   * \param node The Ptr<Node> on which to create the PhantomAgent Application.
	   *
	   * \returns An ApplicationContainer that holds a Ptr<Application> to the
	   *          application created
	   */
	  ApplicationContainer Install (Ptr<Node> node) const;

	  /**
	   * \param c the nodes
	   *
	   * Create one PhantomAgent application on each of the input nodes
	   *
	   * \returns the applications created, one application per input node.
	   */
	  ApplicationContainer Install (NodeContainer c) const;
private:

	  /**
	   * Install an ns3::PhantomAgent on the node configured with all the
	   * attributes set with SetAttribute.
	   *
	   * \param node The node on which an PhantomAgent will be installed.
	   * \returns Ptr to the application installed.
	   */
	  Ptr<Application> InstallPriv (Ptr<Node> node) const;

	ObjectFactory m_factory; //!< Object factory.
	Ipv4Address m_localIpv4;
	uint16_t m_localPort;
};

} /* namespace ns3 */

#endif /* SRC_KCL_HAPTIC_SIM_HELPER_PHANTOM_AGENT_HELPER_H_ */
