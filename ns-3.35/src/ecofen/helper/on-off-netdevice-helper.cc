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
#include "ns3/loopback-net-device.h"
#include "on-off-netdevice-helper.h"
#include "ns3/double.h"

NS_LOG_COMPONENT_DEFINE ("OnOffNetdeviceHelper");

namespace ns3 {

OnOffNetdeviceHelper::OnOffNetdeviceHelper ()
{
  m_onOffNetdevice.SetTypeId ("ns3::OnOffNetdevice");
}

OnOffNetdeviceHelper::~OnOffNetdeviceHelper ()
{
}

void
OnOffNetdeviceHelper::Install (Ptr<NetDevice> netdevice)
{
  NS_ASSERT (netdevice != NULL);
  // check if on off netdevice already exists
  Ptr<NetdeviceEnergyModel> nenergy = netdevice->GetObject<NetdeviceEnergyModel> ();
  Ptr<OnOffNetdevice> nonoffn = nenergy->GetObject<OnOffNetdevice> ();
  if (nonoffn != NULL)
    {
      NS_FATAL_ERROR ("On Off model already installed!");
    }
  Ptr<OnOffNetdevice> onoffn = m_onOffNetdevice.Create<OnOffNetdevice> ();
  NS_ASSERT (onoffn != NULL);
  onoffn->SetNetdevice (netdevice);
  nenergy->AggregateObject (onoffn);
}

void
OnOffNetdeviceHelper::Install (NetDeviceContainer c)
{
  for (NetDeviceContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Install (*i);
    }
}

void
OnOffNetdeviceHelper::Install (std::string netdeviceName)
{
  Ptr<NetDevice> netdevice = Names::Find<NetDevice> (netdeviceName);
  Install (NetDeviceContainer (netdevice));
}

void
OnOffNetdeviceHelper::Install (NodeContainer c)
{
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Install (*i);
    }
}

void
OnOffNetdeviceHelper::Install (Ptr<Node> node)
{
  uint32_t n = node->GetNDevices ();
  uint32_t i;
  for (i = 0; i < n; i++)
    {
      Ptr<NetDevice> netdev = node->GetDevice (i);
      // We do not want to attach an energy model to the loopback net device.
      if (netdev->GetInstanceTypeId ().GetName () != "ns3::LoopbackNetDevice")
        {
          Install (netdev);
        }
    }
}

void
OnOffNetdeviceHelper::Set (std::string name, const AttributeValue &v)
{
  m_onOffNetdevice.Set (name, v);
}

void
OnOffNetdeviceHelper::LinkSwitchOff (NetDeviceContainer link, Time date)
{
  for (NetDeviceContainer::Iterator i = link.Begin (); i != link.End (); ++i)
    {
      NetDeviceSwitchOff (*i, date);
    }
}

void
OnOffNetdeviceHelper::LinkSwitchOn (NetDeviceContainer link, Time date)
{
  for (NetDeviceContainer::Iterator i = link.Begin (); i != link.End (); ++i)
    {
      NetDeviceSwitchOn (*i, date);
    }
}

void
OnOffNetdeviceHelper::NetDeviceSwitchOff (Ptr<NetDevice> netdev, Time date)
{
  Ptr<Node> node = netdev->GetNode ();
  uint32_t interface = netdev->GetIfIndex ();
  Ptr<NetdeviceEnergyModel> nem = netdev->GetObject<NetdeviceEnergyModel> ();
  Ptr<OnOffNetdevice> onoffnetdev = nem->GetObject<OnOffNetdevice> ();
  // Schedule the switch off at the right date
  Simulator::Schedule (date - Simulator::Now (), &OnOffNetdevice::SwitchOff, onoffnetdev, interface,
                       netdev);
}

void
OnOffNetdeviceHelper::NetDeviceSwitchOff (NetDeviceContainer netdevc, Time date)
{
  for (NetDeviceContainer::Iterator i = netdevc.Begin (); i != netdevc.End (); ++i)
    {
      NetDeviceSwitchOff (*i, date);
    }
}

void
OnOffNetdeviceHelper::NetDeviceSwitchOn (Ptr<NetDevice> netdev, Time date)
{
  Ptr<Node> node = netdev->GetNode ();
  uint32_t interface = netdev->GetIfIndex ();
  Ptr<NetdeviceEnergyModel> nem = netdev->GetObject<NetdeviceEnergyModel> ();
  Ptr<OnOffNetdevice> onoffnetdev = nem->GetObject<OnOffNetdevice> ();
  // Schedule the switch on at the right date
  Simulator::Schedule (date - Simulator::Now (), &OnOffNetdevice::SwitchOn, onoffnetdev, interface,
                       netdev);
}

void
OnOffNetdeviceHelper::NetDeviceSwitchOn (NetDeviceContainer netdevc, Time date)
{
  for (NetDeviceContainer::Iterator i = netdevc.Begin (); i != netdevc.End (); ++i)
    {
      NetDeviceSwitchOn (*i, date);
    }
}

double
OnOffNetdeviceHelper::GetDuration (Ptr<NetDevice> nd)
{
  double duration;
  Ptr<NetdeviceEnergyModel> nem = nd->GetObject<NetdeviceEnergyModel> ();
  Ptr<OnOffNetdevice> oonetdev = nem->GetObject<OnOffNetdevice> ();
  DoubleValue son;
  DoubleValue soff;
  oonetdev->GetAttribute ("SwitchOnDuration", son);
  oonetdev->GetAttribute ("SwitchOffDuration", soff);
  duration = son.Get () + soff.Get ();
  return duration;
}

double
OnOffNetdeviceHelper::GetSwitchOn (Ptr<NetDevice> nd)
{
  Ptr<NetdeviceEnergyModel> nem = nd->GetObject<NetdeviceEnergyModel> ();
  Ptr<OnOffNetdevice> oonetdev = nem->GetObject<OnOffNetdevice> ();
  DoubleValue son;
  oonetdev->GetAttribute ("SwitchOnDuration", son);
  return son.Get ();
}

double
OnOffNetdeviceHelper::GetSwitchOff (Ptr<NetDevice> nd)
{
  Ptr<NetdeviceEnergyModel> nem = nd->GetObject<NetdeviceEnergyModel> ();
  Ptr<OnOffNetdevice> oonetdev = nem->GetObject<OnOffNetdevice> ();
  DoubleValue soff;
  oonetdev->GetAttribute ("SwitchOffDuration", soff);
  return soff.Get ();
}

} // namespace ns3
