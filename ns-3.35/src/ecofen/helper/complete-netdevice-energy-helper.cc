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

#include "complete-netdevice-energy-helper.h"
#include "ns3/netdevice-energy-model.h"
#include "ns3/complete-netdevice-energy-model.h"

namespace ns3 {

CompleteNetdeviceEnergyHelper::CompleteNetdeviceEnergyHelper ()
{
  m_completeNetdeviceEnergyModel.SetTypeId ("ns3::CompleteNetdeviceEnergyModel");
}

CompleteNetdeviceEnergyHelper::~CompleteNetdeviceEnergyHelper ()
{
}

void
CompleteNetdeviceEnergyHelper::Set (std::string name, const AttributeValue &v)
{
  m_completeNetdeviceEnergyModel.Set (name, v);
}

Ptr<NetdeviceEnergyModel>
CompleteNetdeviceEnergyHelper::DoInstall (Ptr<NetDevice> netdevice) const
{
  NS_ASSERT (netdevice != NULL);
  // check if energy model already exists
  Ptr<NetdeviceEnergyModel> nenergy = netdevice->GetObject<NetdeviceEnergyModel> ();
  if (nenergy != NULL)
    {
      NS_FATAL_ERROR ("Net device energy model already installed!");
    }
  Ptr<CompleteNetdeviceEnergyModel> energy =
      m_completeNetdeviceEnergyModel.Create<CompleteNetdeviceEnergyModel> ();
  NS_ASSERT (energy != NULL);
  energy->SetNetdevice (netdevice);
  netdevice->AggregateObject (energy);
  energy->Initialize ();
  return energy;
}

} // namespace ns3
