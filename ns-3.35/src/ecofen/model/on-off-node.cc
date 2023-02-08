/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 CNRS, France.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Anne-Cecile Orgerie <anne-cecile.orgerie@irisa.fr>
 */

#include "node-energy-model.h"
#include "on-off-node.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/ipv4.h"
#include "ns3/ipv6.h"
#include "ns3/node.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (OnOffNode);

TypeId
OnOffNode::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::OnOffNode")
          .SetParent<Object> ()
          .AddConstructor<OnOffNode> ()
          .AddAttribute ("SwitchOnEnergy", "The energy used to switch on a node.", DoubleValue (),
                         MakeDoubleAccessor (&OnOffNode::m_switchOnEnergy),
                         MakeDoubleChecker<double> ())
          .AddAttribute ("SwitchOffEnergy", "The energy used to switch off a node.", DoubleValue (),
                         MakeDoubleAccessor (&OnOffNode::m_switchOffEnergy),
                         MakeDoubleChecker<double> ())
          .AddAttribute ("SwitchOffDuration", "The duration of a switch off a node in seconds.",
                         DoubleValue (), MakeDoubleAccessor (&OnOffNode::m_switchOffDuration),
                         MakeDoubleChecker<double> ())
          .AddAttribute ("SwitchOnDuration", "The duration of a switch on a node in seconds.",
                         DoubleValue (), MakeDoubleAccessor (&OnOffNode::m_switchOnDuration),
                         MakeDoubleChecker<double> ());
  return tid;
}

OnOffNode::OnOffNode ()
{
}

OnOffNode::~OnOffNode ()
{
}

void
OnOffNode::SetNode (Ptr<Node> node)
{
  NS_ASSERT (node != NULL);
  m_node = node;
}

Ptr<Node>
OnOffNode::GetNode (void) const
{
  return m_node;
}

void
OnOffNode::AddNDZeroState (uint32_t statenb)
{
  m_zeroStates.push_back (statenb);
}

void
OnOffNode::SwitchOff (Ptr<Node> node)
{
  Time now = Simulator::Now ();
  if (now > m_lastSwitchEnd)
    {
      Ptr<NodeEnergyModel> nem = node->GetObject<NodeEnergyModel> ();
      nem->SetNodeState (nem->GetNodeOffState ());
      Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
      uint32_t i;
      uint32_t j = 0;
      uint32_t n = node->GetNDevices ();
      for (i = 0; i < n; i++)
        {
          Ptr<NetDevice> netdev = node->GetDevice (i);
          // no energy model attached to the loopback net device.
          if (netdev->GetInstanceTypeId ().GetName () != "ns3::LoopbackNetDevice")
            {
              uint32_t interface = netdev->GetIfIndex ();
              if (ipv4 != NULL)
                {
                  ipv4->SetDown (interface);
                }
              else
                {
                  Ptr<Ipv6> ipv6 = node->GetObject<Ipv6> ();
                  ipv6->SetDown (interface);
                }
              Ptr<NetdeviceEnergyModel> ndem = netdev->GetObject<NetdeviceEnergyModel> ();
              ndem->UpdateState (m_zeroStates[j], 0.0, Seconds (m_switchOffDuration));
              j++;
            }
        }
      NS_LOG_UNCOND ("OFF node " << node->GetId ());
      nem->UpdateState (nem->GetNodeOffState (), m_switchOffEnergy, Seconds (m_switchOffDuration));
    }
  else
    {
      NS_LOG_UNCOND ("Trying to switch off a netdevice already switching!");
    }
  m_lastSwitchEnd = now + Seconds (m_switchOffDuration);
}

void
OnOffNode::SwitchOn (Ptr<Node> node)
{
  Time now = Simulator::Now ();
  if (now > m_lastSwitchEnd)
    {
      Ptr<NodeEnergyModel> nem = node->GetObject<NodeEnergyModel> ();
      nem->SetNodeState (nem->GetNodeOnState ());
      Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
      uint32_t i;
      uint32_t n = node->GetNDevices ();
      for (i = 0; i < n; i++)
        {
          Ptr<NetDevice> netdev = node->GetDevice (i);
          // no energy model attached to the loopback net device.
          if (netdev->GetInstanceTypeId ().GetName () != "ns3::LoopbackNetDevice")
            {
              uint32_t interface = netdev->GetIfIndex ();
              if (ipv4 != NULL)
                {
                  Simulator::Schedule (Seconds (m_switchOnDuration), &Ipv4::SetUp, ipv4, interface);
                  //ipv4->SetUp (interface);
                }
              else
                {
                  Ptr<Ipv6> ipv6 = node->GetObject<Ipv6> ();
                  Simulator::Schedule (Seconds (m_switchOnDuration), &Ipv6::SetUp, ipv6, interface);
                  //ipv6->SetUp (interface);
                }
            }
        }
      NS_LOG_UNCOND ("ON node " << node->GetId ());
      nem->UpdateState (nem->GetNodeOnState (), m_switchOnEnergy, Seconds (m_switchOnDuration));
    }
  else
    {
      NS_LOG_UNCOND ("Trying to switch on a netdevice already switching!");
    }
  m_lastSwitchEnd = now + Seconds (m_switchOnDuration);
}

} // namespace ns3
