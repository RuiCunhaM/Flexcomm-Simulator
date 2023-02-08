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

#include "linear-netdevice-energy-helper.h"
#include "ns3/netdevice-energy-model.h"
#include "ns3/linear-netdevice-energy-model.h"

namespace ns3 {

LinearNetdeviceEnergyHelper::LinearNetdeviceEnergyHelper ()
{
  m_linearNetdeviceEnergyModel.SetTypeId ("ns3::LinearNetdeviceEnergyModel");
}

LinearNetdeviceEnergyHelper::~LinearNetdeviceEnergyHelper ()
{
}

void
LinearNetdeviceEnergyHelper::Set (std::string name, const AttributeValue &v)
{
  m_linearNetdeviceEnergyModel.Set (name, v);
}

Ptr<NetdeviceEnergyModel>
LinearNetdeviceEnergyHelper::DoInstall (Ptr<NetDevice> netdevice) const
{
  NS_ASSERT (netdevice != NULL);
  // check if energy model already exists
  Ptr<NetdeviceEnergyModel> nenergy = netdevice->GetObject<NetdeviceEnergyModel> ();
  if (nenergy != NULL)
    {
      NS_FATAL_ERROR ("Net device energy model already installed!");
    }
  Ptr<LinearNetdeviceEnergyModel> energy =
      m_linearNetdeviceEnergyModel.Create<LinearNetdeviceEnergyModel> ();
  NS_ASSERT (energy != NULL);
  energy->SetNetdevice (netdevice);
  netdevice->AggregateObject (energy);
  energy->Initialize ();
  return energy;
}

} // namespace ns3
