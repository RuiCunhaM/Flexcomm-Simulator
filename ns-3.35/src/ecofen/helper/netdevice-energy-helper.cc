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
#include "netdevice-energy-helper.h"

NS_LOG_COMPONENT_DEFINE ("NetdeviceEnergyHelper");

namespace ns3 {

NetdeviceEnergyHelper::NetdeviceEnergyHelper ()
{
}

void
NetdeviceEnergyHelper::Install (Ptr<NetDevice> netdevice)
{
  Ptr<NetdeviceEnergyModel> nrj = DoInstall (netdevice);
}

void
NetdeviceEnergyHelper::Install (NetDeviceContainer c)
{
  for (NetDeviceContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Install (*i);
    }
}

void
NetdeviceEnergyHelper::Install (std::string netdeviceName)
{
  Ptr<NetDevice> netdevice = Names::Find<NetDevice> (netdeviceName);
  Install (NetDeviceContainer (netdevice));
}

void
NetdeviceEnergyHelper::Install (NodeContainer c)
{
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Install (*i);
    }
}

void
NetdeviceEnergyHelper::Install (Ptr<Node> node)
{
  uint32_t n = node->GetNDevices ();
  uint32_t i;
  for (i = 0; i < n; i++)
    {
      Ptr<NetDevice> netdev = node->GetDevice (i);
      // We do not want to attach an energy model to the loopback net device.
      if (netdev->GetInstanceTypeId ().GetName () != "ns3::LoopbackNetDevice")
        {
          Ptr<NetdeviceEnergyModel> nrj = DoInstall (netdev);
        }
    }
}

} // namespace ns3
