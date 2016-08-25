
//
// Code and idea based on: https://www.nsnam.org/doxygen/udp-echo-helper_8cc_source.html
//
#include "phantom-agent-helper.h"
#include "../model/phantom-agent.h"
#include "ns3/application-container.h"
namespace ns3 {

PhantomAgentHelper::PhantomAgentHelper(Ipv4Address localIpv4, uint16_t localPort) {
	m_factory.SetTypeId (PhantomAgent::GetTypeId ());
	m_localIpv4 = localIpv4;
	m_localPort = localPort;
}

void
PhantomAgentHelper::SetAttribute (
  std::string name,
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

PhantomAgentHelper::~PhantomAgentHelper() {
	// TODO Auto-generated destructor stub
}


ApplicationContainer
PhantomAgentHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}


ApplicationContainer
PhantomAgentHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
PhantomAgentHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<PhantomAgent> app = m_factory.Create<PhantomAgent> ();
  app->SetLocal(m_localIpv4,m_localPort);
  node->AddApplication (app);

  return app;
}

} /* namespace ns3 */
