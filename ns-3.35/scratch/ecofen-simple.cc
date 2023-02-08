/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
#include "ns3/core-module.h"
#include "ns3/node-container.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/basic-node-energy-helper.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/linear-netdevice-energy-helper.h"
#include "ns3/consumption-logger.h"

using namespace ns3;

int
main (int argc, char *argv[])
{

  NodeContainer nodes;
  nodes.Create (3);

  InternetStackHelper stack;
  stack.Install (nodes);

  BasicNodeEnergyHelper basicNodeEnergy;
  basicNodeEnergy.Set ("OnConso", DoubleValue (10.0));
  basicNodeEnergy.Set ("OffConso", DoubleValue (1.0));
  basicNodeEnergy.Install (nodes);

  NodeContainer n0n1 = NodeContainer (nodes.Get (0), nodes.Get (1));
  NodeContainer n1n2 = NodeContainer (nodes.Get (1), nodes.Get (2));
  PointToPointHelper pointtopoint;
  pointtopoint.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
  pointtopoint.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (10)));
  NetDeviceContainer d0d1 = pointtopoint.Install (n0n1);
  NetDeviceContainer d1d2 = pointtopoint.Install (n1n2);

  LinearNetdeviceEnergyHelper linearNetdeviceEnergy;
  linearNetdeviceEnergy.Set ("IdleConso", DoubleValue (0.5));
  linearNetdeviceEnergy.Set ("OffConso", DoubleValue (0.2));
  linearNetdeviceEnergy.Set ("ByteEnergy", DoubleValue (300.0));
  linearNetdeviceEnergy.Install (nodes);

  ConsumptionLogger conso;
  conso.NodeConso (Seconds (0.5), Seconds (10.0), nodes);

  // Run the simulation
  Simulator::Stop (Seconds (10));
  Simulator::Run ();
  Simulator::Destroy ();
}
