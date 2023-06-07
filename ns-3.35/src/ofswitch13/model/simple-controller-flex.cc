/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * The GPLv2 License (GPLv2)
 *
 * Copyright (c) 2023 Rui Pedro C. Monteiro
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http: //www.gnu.org/licenses/>.
 *
 * Author: Rui Pedro C. Monteiro <rui.p.monteiro@inesctec.pt>
 */

#include <cstdint>
#include <cstdlib>

#include "simple-controller-flex.h"

#include "ns3/log.h"
#include "ns3/parser.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/ofswitch13-module.h"
#include "ns3/topology-module.h"
#include "ns3/energy-api-module.h"

NS_LOG_COMPONENT_DEFINE ("SimpleControllerFlex");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (SimpleControllerFlex);

SimpleControllerFlex::SimpleControllerFlex ()
{
  NS_LOG_FUNCTION (this);
  m_isFirstUpdate = true;
}

SimpleControllerFlex::~SimpleControllerFlex ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
SimpleControllerFlex::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SimpleControllerFlex")
                          .SetParent<SimpleController> ()
                          .SetGroupName ("OFSwitch13")
                          .AddConstructor<SimpleControllerFlex> ();
  return tid;
}

void
SimpleControllerFlex::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  SimpleController::DoDispose ();
}

void
SimpleControllerFlex::UpdateWeights ()
{
  Graph topo = Topology::GetGraph ();

  boost::graph_traits<Graph>::edge_iterator edgeIt, edgeEnd;
  for (boost::tie (edgeIt, edgeEnd) = boost::edges (topo); edgeIt != edgeEnd; ++edgeIt)
    {
      Edge ed = *edgeIt;
      Ptr<Node> n1 = Topology::VertexToNode (ed.m_source);
      Ptr<Node> n2 = Topology::VertexToNode (ed.m_target);

      if (n1->IsSwitch () && n2->IsSwitch ())
        {
          int index = int (Simulator::Now ().GetMinutes ()) % 60;
          float flex1 = EnergyAPI::GetFlexArray (Names::FindName (n1)).at (index);
          float flex2 = EnergyAPI::GetFlexArray (Names::FindName (n2)).at (index);

          Topology::UpdateEdgeWeight (n1, n2, flex1 + flex2);
        }
    }
}

void
SimpleControllerFlex::UpdateRouting ()
{
  UpdateWeights ();
  NodeContainer switches = NodeContainer::GetGlobalSwitches ();
  for (auto sw = switches.Begin (); sw != switches.End (); sw++)
    ApplyRouting (Id2DpId ((*sw)->GetId ()));

  Simulator::Schedule (Minutes (1), &SimpleControllerFlex::UpdateRouting, this);
}

void
SimpleControllerFlex::HandshakeSuccessful (Ptr<const RemoteSwitch> sw)
{
  NS_LOG_FUNCTION (this << sw);

  uint64_t swDpId = sw->GetDpId ();

  // Default rules
  DpctlExecute (swDpId, "flow-mod cmd=add,table=0,prio=0 "
                        "apply:output=ctrl:128");
  DpctlExecute (swDpId, "set-config miss=128");

  if (m_isFirstUpdate)
    {
      UpdateWeights ();
      m_isFirstUpdate = false;
      Simulator::Schedule (Minutes (1), &SimpleControllerFlex::UpdateRouting, this);
    }

  ApplyRouting (swDpId);
}

} // namespace ns3
