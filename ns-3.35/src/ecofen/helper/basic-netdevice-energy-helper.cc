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
 * Author: Anne-Cecile Orgerie <anne-cecile.orgerie@irisa.fr>
 */

#include "basic-netdevice-energy-helper.h"
#include "ns3/netdevice-energy-model.h"
#include "ns3/basic-netdevice-energy-model.h"

namespace ns3 {

BasicNetdeviceEnergyHelper::BasicNetdeviceEnergyHelper ()
{
  m_basicNetdeviceEnergyModel.SetTypeId ("ns3::BasicNetdeviceEnergyModel");
}

BasicNetdeviceEnergyHelper::~BasicNetdeviceEnergyHelper ()
{
}

void
BasicNetdeviceEnergyHelper::Set (std::string name, const AttributeValue &v)
{
  m_basicNetdeviceEnergyModel.Set (name, v);
}

Ptr<NetdeviceEnergyModel>
BasicNetdeviceEnergyHelper::DoInstall (Ptr<NetDevice> netdevice) const
{
  NS_ASSERT (netdevice != NULL);
  // check if energy model already exists
  Ptr<NetdeviceEnergyModel> nenergy = netdevice->GetObject<NetdeviceEnergyModel> ();
  if (nenergy != NULL)
    {
      NS_FATAL_ERROR ("Net device energy model already installed!");
    }
  Ptr<BasicNetdeviceEnergyModel> energy =
      m_basicNetdeviceEnergyModel.Create<BasicNetdeviceEnergyModel> ();
  NS_ASSERT (energy != NULL);
  energy->SetNetdevice (netdevice);
  netdevice->AggregateObject (energy);
  energy->Initialize ();
  return energy;
}

} // namespace ns3
