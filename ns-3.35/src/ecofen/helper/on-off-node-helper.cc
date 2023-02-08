/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 CNRS, France
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
 * Author: Anne-Cécile Orgerie <anne-cecile.orgerie@irisa.fr>
 */

#include "ns3/abort.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/queue.h"
#include "ns3/config.h"
#include "ns3/names.h"
#include "on-off-node-helper.h"
#include "ns3/basic-netdevice-energy-model.h"
#include "ns3/linear-netdevice-energy-model.h"
#include "ns3/complete-netdevice-energy-model.h"

NS_LOG_COMPONENT_DEFINE ("OnOffNodeHelper");

namespace ns3 {

OnOffNodeHelper::OnOffNodeHelper ()
{
  m_onOffNode.SetTypeId ("ns3::OnOffNode");
}

OnOffNodeHelper::~OnOffNodeHelper ()
{
}

void
OnOffNodeHelper::Install (Ptr<Node> node)
{
  NS_ASSERT (node != NULL);
  // check if on off node already exists
  Ptr<NodeEnergyModel> nenergy = node->GetObject<NodeEnergyModel> ();
  Ptr<OnOffNode> nonoffn = nenergy->GetObject<OnOffNode> ();
  if (nonoffn != NULL)
    {
      NS_FATAL_ERROR ("On Off model already installed!");
    }
  Ptr<OnOffNode> onoffn = m_onOffNode.Create<OnOffNode> ();
  NS_ASSERT (onoffn != NULL);
  onoffn->SetNode (node);
  nenergy->AggregateObject (onoffn);
  uint32_t i;
  uint32_t n = node->GetNDevices ();
  // Add a state with null conso to all the netdevices.
  // This state will be used when the node is off or switching.
  for (i = 0; i < n; i++)
    {
      Ptr<NetDevice> netdev = node->GetDevice (i);
      // no energy model attached to the loopback net device.
      if (netdev->GetInstanceTypeId ().GetName () != "ns3::LoopbackNetDevice")
        {
          Ptr<NetdeviceEnergyModel> ndem = netdev->GetObject<NetdeviceEnergyModel> ();
          uint32_t statenb;
          if (ndem->GetInstanceTypeId ().GetName () == "ns3::BasicNetdeviceEnergyModel")
            {
              Ptr<BasicNetdeviceEnergyModel> nem;
              nem = netdev->GetObject<BasicNetdeviceEnergyModel> ();
              statenb = nem->AddState (0.0);
            }
          else if (ndem->GetInstanceTypeId ().GetName () == "ns3::LinearNetdeviceEnergyModel")
            {
              Ptr<LinearNetdeviceEnergyModel> nem;
              nem = netdev->GetObject<LinearNetdeviceEnergyModel> ();
              statenb = nem->AddState (0.0, 0.0);
            }
          else if (ndem->GetInstanceTypeId ().GetName () == "ns3::CompleteNetdeviceEnergyModel")
            {
              Ptr<CompleteNetdeviceEnergyModel> nem;
              nem = netdev->GetObject<CompleteNetdeviceEnergyModel> ();
              statenb = nem->AddState (0.0, 0.0, 0.0, 0.0, 0.0);
            }
          else
            {
              NS_FATAL_ERROR ("Unknown netdevice energy model! You may have forget to set one. It "
                              "is required to use on-off functions.");
            }
          onoffn->AddNDZeroState (statenb);
        }
    }
}

void
OnOffNodeHelper::Install (NodeContainer c)
{
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Install (*i);
    }
}

void
OnOffNodeHelper::Set (std::string name, const AttributeValue &v)
{
  m_onOffNode.Set (name, v);
}

void
OnOffNodeHelper::NodeSwitchOff (Ptr<Node> node, Time date)
{
  Ptr<NodeEnergyModel> nem = node->GetObject<NodeEnergyModel> ();
  Ptr<OnOffNode> onoffnode = nem->GetObject<OnOffNode> ();
  // Schedule the switch off at the right date.
  Simulator::Schedule (date - Simulator::Now (), &OnOffNode::SwitchOff, onoffnode, node);
}

void
OnOffNodeHelper::NodeSwitchOff (NodeContainer nodec, Time date)
{
  for (NodeContainer::Iterator i = nodec.Begin (); i != nodec.End (); ++i)
    {
      NodeSwitchOff (*i, date);
    }
}

void
OnOffNodeHelper::NodeSwitchOn (Ptr<Node> node, Time date)
{
  Ptr<NodeEnergyModel> nem = node->GetObject<NodeEnergyModel> ();
  Ptr<OnOffNode> onoffnode = nem->GetObject<OnOffNode> ();
  // Schedule the switch on at the right date.
  Simulator::Schedule (date - Simulator::Now (), &OnOffNode::SwitchOn, onoffnode, node);
}

void
OnOffNodeHelper::NodeSwitchOn (NodeContainer nodec, Time date)
{
  for (NodeContainer::Iterator i = nodec.Begin (); i != nodec.End (); ++i)
    {
      NodeSwitchOn (*i, date);
    }
}

double
OnOffNodeHelper::GetDuration (Ptr<Node> node)
{
  double duration;
  Ptr<NodeEnergyModel> nem = node->GetObject<NodeEnergyModel> ();
  Ptr<OnOffNode> oonode = nem->GetObject<OnOffNode> ();
  DoubleValue son;
  DoubleValue soff;
  oonode->GetAttribute ("SwitchOnDuration", son);
  oonode->GetAttribute ("SwitchOffDuration", soff);
  duration = son.Get () + soff.Get ();
  return duration;
}

double
OnOffNodeHelper::GetSwitchOn (Ptr<Node> node)
{
  Ptr<NodeEnergyModel> nem = node->GetObject<NodeEnergyModel> ();
  Ptr<OnOffNode> oonode = nem->GetObject<OnOffNode> ();
  DoubleValue son;
  oonode->GetAttribute ("SwitchOnDuration", son);
  return son.Get ();
}

double
OnOffNodeHelper::GetSwitchOff (Ptr<Node> node)
{
  Ptr<NodeEnergyModel> nem = node->GetObject<NodeEnergyModel> ();
  Ptr<OnOffNode> oonode = nem->GetObject<OnOffNode> ();
  DoubleValue soff;
  oonode->GetAttribute ("SwitchOffDuration", soff);
  return soff.Get ();
}

} // namespace ns3
