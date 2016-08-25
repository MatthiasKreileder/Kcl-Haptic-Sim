
//
// Code and idea based on: https://www.nsnam.org/doxygen/udp-echo-helper_8cc_source.html
//
#include "haptic-tele-operator-helper.h"
#include "../model/haptic-tele-operator.h"
#include "ns3/uinteger.h"
#include "ns3/names.h"

namespace ns3 {

HapticTeleOperatorHelper::HapticTeleOperatorHelper (uint16_t port)
{
  m_factory.SetTypeId (HapticTeleOperator::GetTypeId ());
  m_factory.Set ("Port", UintegerValue (port));
}

void
HapticTeleOperatorHelper::SetAttribute (
  std::string name,
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}


ApplicationContainer
HapticTeleOperatorHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

Ptr<Application>
HapticTeleOperatorHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<HapticTeleOperator> ();
  node->AddApplication (app);

  return app;
}

} /* namespace ns3 */
