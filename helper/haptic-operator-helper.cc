
//
// Code and idea based on: https://www.nsnam.org/doxygen/udp-echo-helper_8cc_source.html
//
#include "haptic-operator-helper.h"

#include "haptic-operator-helper.h"
#include "../model/haptic-operator.h"
#include "ns3/uinteger.h"
#include "ns3/names.h"

namespace ns3 {

HapticOperatorHelper::HapticOperatorHelper (Address address, uint16_t port)
{
  m_factory.SetTypeId (HapticOperator::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (address));
  SetAttribute ("RemotePort", UintegerValue (port));
}

HapticOperatorHelper::HapticOperatorHelper (Ipv4Address address, uint16_t port)
{
  m_factory.SetTypeId (HapticOperator::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (Address(address)));
  SetAttribute ("RemotePort", UintegerValue (port));
}

HapticOperatorHelper::HapticOperatorHelper (Ipv6Address address, uint16_t port)
{
  m_factory.SetTypeId (HapticOperator::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (Address(address)));
  SetAttribute ("RemotePort", UintegerValue (port));
}

void
HapticOperatorHelper::SetAttribute (
  std::string name,
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}


ApplicationContainer
HapticOperatorHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}


ApplicationContainer
HapticOperatorHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
HapticOperatorHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<HapticOperator> ();
  node->AddApplication (app);

  return app;
}

} /* namespace ns3 */
