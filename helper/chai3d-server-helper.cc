/*
 * haptic-operator-helper.cc
 *
 *  Created on: 13 Jul 2016
 *      Author: matthias
 */

#include "chai3d-server-helper.h"
#include "../model/chai3d-server.h"
#include "ns3/uinteger.h"
#include "ns3/names.h"

namespace ns3 {

Chai3dServerHelper::Chai3dServerHelper (uint16_t port)
{
  m_factory.SetTypeId (Chai3dServer::GetTypeId ());
  m_factory.Set ("Port", UintegerValue (port));
}

void
Chai3dServerHelper::SetAttribute (
  std::string name,
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}


ApplicationContainer
Chai3dServerHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

Ptr<Application>
Chai3dServerHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Chai3dServer> app = m_factory.Create<Chai3dServer> ();
  node->AddApplication (app);
  app->Setup();
  return app;
}

} /* namespace ns3 */
