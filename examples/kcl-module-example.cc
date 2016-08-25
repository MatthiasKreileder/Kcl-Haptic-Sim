/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/core-module.h"
#include "ns3/new-module-helper.h"
#include "ns3/log.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE("kcl-module");

int 
main (int argc, char *argv[])
{
  LogComponentEnable("kcl-module",LOG_LEVEL_DEBUG);
  bool verbose = true;

  CommandLine cmd;
  cmd.AddValue ("verbose", "Tell application to log if true", verbose);

  cmd.Parse (argc,argv);

  /* ... */

  NS_LOG_DEBUG("some message");

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}


