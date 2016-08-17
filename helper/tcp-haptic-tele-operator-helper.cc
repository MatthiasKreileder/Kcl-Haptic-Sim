/*
 * tcp-haptic-tele-operator-helper.cc
 *
 *  Created on: 17 Aug 2016
 *      Author: matthias
 */

#include "tcp-haptic-tele-operator-helper.h"
#include "../model/tcp-haptic-tele-operator.h"
namespace ns3 {

TcpHapticTeleOperatorHelper::TcpHapticTeleOperatorHelper(uint16_t port, Ipv4Address tcpHapticOperatorAddress) {
	// TODO Auto-generated constructor stub
	m_factory.SetTypeId (TcpHapticTeleOperator::GetTypeId ());
	m_factory.Set ("Port", UintegerValue (port));
	//m_factory.Set ("TcpOperatorAddress", tcpHapticOperatorAddress);
	m_ipv4Addr = tcpHapticOperatorAddress;
}

TcpHapticTeleOperatorHelper::~TcpHapticTeleOperatorHelper() {
	// TODO Auto-generated destructor stub
}

void
TcpHapticTeleOperatorHelper::SetAttribute (
  std::string name,
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}


ApplicationContainer
TcpHapticTeleOperatorHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

Ptr<Application>
TcpHapticTeleOperatorHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<TcpHapticTeleOperator> app = m_factory.Create<TcpHapticTeleOperator> ();
  app->SetTcpHapticTeleOperator(m_ipv4Addr);
  node->AddApplication (app);

  return app;
}

} /* namespace ns3 */
