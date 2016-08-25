
//
// Code and idea based on: https://www.nsnam.org/doxygen/udp-echo-helper_8cc_source.html
//
#include "tcp-haptic-operator-helper.h"
#include "../model/tcp-haptic-operator.h"
namespace ns3 {

TcpHapticOperatorHelper::~TcpHapticOperatorHelper() {
	// TODO Auto-generated destructor stub
}

TcpHapticOperatorHelper::TcpHapticOperatorHelper (Address address, uint16_t port)
{
  m_factory.SetTypeId (TcpHapticOperator::GetTypeId ());
  m_factory.Set  ("TcpTeleOperatorAddress", AddressValue (address));
  m_factory.Set  ("TcpTeleOperatorPort", UintegerValue (port));
}

void
TcpHapticOperatorHelper::SetAttribute (
  std::string name,
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
TcpHapticOperatorHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}


ApplicationContainer
TcpHapticOperatorHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}
Ptr<Application>
TcpHapticOperatorHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<TcpHapticOperator> ();
  node->AddApplication (app);

  return app;
}

} /* namespace ns3 */
