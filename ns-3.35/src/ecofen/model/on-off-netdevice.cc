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

#include "netdevice-energy-model.h"
#include "on-off-netdevice.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/ipv4.h"
#include "ns3/ipv6.h"
#include "ns3/node.h"
#include "ns3/basic-netdevice-energy-model.h"
#include "ns3/linear-netdevice-energy-model.h"
#include "ns3/complete-netdevice-energy-model.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (OnOffNetdevice);

TypeId
OnOffNetdevice::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::OnOffNetdevice")
          .SetParent<Object> ()
          .AddConstructor<OnOffNetdevice> ()
          .AddAttribute ("SwitchOnEnergy", "The energy used to switch on a net device.",
                         DoubleValue (), MakeDoubleAccessor (&OnOffNetdevice::m_switchOnEnergy),
                         MakeDoubleChecker<double> ())
          .AddAttribute ("SwitchOffEnergy", "The energy used to switch off a net device.",
                         DoubleValue (), MakeDoubleAccessor (&OnOffNetdevice::m_switchOffEnergy),
                         MakeDoubleChecker<double> ())
          .AddAttribute ("SwitchOffDuration",
                         "The duration of a switch off of a net device in seconds.", DoubleValue (),
                         MakeDoubleAccessor (&OnOffNetdevice::m_switchOffDuration),
                         MakeDoubleChecker<double> ())
          .AddAttribute ("SwitchOnDuration",
                         "The duration of a switch on of a net device in seconds.", DoubleValue (),
                         MakeDoubleAccessor (&OnOffNetdevice::m_switchOnDuration),
                         MakeDoubleChecker<double> ());
  return tid;
}

OnOffNetdevice::OnOffNetdevice ()
{
}

OnOffNetdevice::~OnOffNetdevice ()
{
}

void
OnOffNetdevice::SetNetdevice (Ptr<NetDevice> netdevice)
{
  NS_ASSERT (netdevice != NULL);
  m_netdevice = netdevice;
}

Ptr<NetDevice>
OnOffNetdevice::GetNetdevice (void) const
{
  return m_netdevice;
}

void
OnOffNetdevice::SwitchOff (uint32_t interface, Ptr<NetDevice> netdevice)
{
  Time now = Simulator::Now ();
  if (now > m_lastSwitchEnd)
    {
      Ptr<NetdeviceEnergyModel> ndem = netdevice->GetObject<NetdeviceEnergyModel> ();
      Ptr<NetdeviceEnergyModel> nem;
      if (ndem->GetInstanceTypeId ().GetName () == "ns3::BasicNetdeviceEnergyModel")
        { // TODO vérifier si c'est vraiment utile
          nem = netdevice->GetObject<BasicNetdeviceEnergyModel> ();
        }
      else if (ndem->GetInstanceTypeId ().GetName () == "ns3::LinearNetdeviceEnergyModel")
        {
          nem = netdevice->GetObject<LinearNetdeviceEnergyModel> ();
        }
      else if (ndem->GetInstanceTypeId ().GetName () == "ns3::CompleteNetdeviceEnergyModel")
        {
          nem = netdevice->GetObject<CompleteNetdeviceEnergyModel> ();
        }
      nem->SetNetdeviceState (nem->GetNetdeviceOffState ());
      Ptr<Node> node = netdevice->GetNode ();
      Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
      if (ipv4 != NULL)
        {
          ipv4->SetDown (interface);
          NS_LOG_UNCOND ("OFF interface " << interface << " on node "
                                          << netdevice->GetNode ()->GetId ());
        }
      else
        {
          Ptr<Ipv6> ipv6 = node->GetObject<Ipv6> ();
          ipv6->SetDown (interface);
          NS_LOG_UNCOND ("OFF interface " << interface << " on node "
                                          << netdevice->GetNode ()->GetId ());
        }
      uint32_t offstate = nem->GetNetdeviceOffState ();
      nem->UpdateState (offstate, m_switchOffEnergy, Seconds (m_switchOffDuration));
    }
  else
    {
      NS_LOG_UNCOND ("Trying to switch off a netdevice already switching!");
    }
  m_lastSwitchEnd = now + Seconds (m_switchOffDuration);
}

void
OnOffNetdevice::SwitchOn (uint32_t interface, Ptr<NetDevice> netdevice)
{
  Time now = Simulator::Now ();
  if (now > m_lastSwitchEnd)
    {
      Ptr<NetdeviceEnergyModel> nem = netdevice->GetObject<NetdeviceEnergyModel> ();
      nem->SetNetdeviceState (nem->GetNetdeviceOnState ());
      Ptr<Node> node = netdevice->GetNode ();
      Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
      if (ipv4 != NULL)
        {
          Simulator::Schedule (Seconds (m_switchOnDuration), &Ipv4::SetUp, ipv4, interface);
          NS_LOG_UNCOND ("OOOO when " << now + Seconds (m_switchOnDuration));
          Simulator::Schedule (Seconds (m_switchOnDuration), &OnOffNetdevice::DelayedPrintOn, this,
                               interface, netdevice->GetNode ()->GetId ());
          //NS_LOG_UNCOND ("ON interface " << interface << " on node " << netdevice->GetNode ()->GetId () << " in " << m_switchOnDuration);
          //ipv4->SetUp (interface);
        }
      else
        {
          Ptr<Ipv6> ipv6 = node->GetObject<Ipv6> ();
          Simulator::Schedule (Seconds (m_switchOnDuration), &Ipv6::SetUp, ipv6, interface);
          NS_LOG_UNCOND ("ON interface " << interface << " on node "
                                         << netdevice->GetNode ()->GetId ());
          //ipv6->SetUp (interface);
        }
      nem->UpdateState (nem->GetNetdeviceOnState (), m_switchOnEnergy,
                        Seconds (m_switchOnDuration));
    }
  else
    {
      NS_LOG_UNCOND ("Trying to switch on a netdevice already switching!");
    }
  m_lastSwitchEnd = now + Seconds (m_switchOnDuration);
}

void
OnOffNetdevice::DelayedPrintOn (uint32_t interface, uint32_t node)
{
  NS_LOG_UNCOND ("ON interface " << interface << " on node " << node << " at "
                                 << Simulator::Now ());
}

} // namespace ns3
