/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * The GPLv2 License (GPLv2)
 *
 * Copyright (c) 2024 Rui Pedro C. Monteiro
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

#include "data-rate-netdevice-energy-helper.h"
#include "ns3/netdevice-energy-model.h"
#include "ns3/data-rate-netdevice-energy-model.h"

namespace ns3 {

DataRateNetdeviceEnergyHelper::DataRateNetdeviceEnergyHelper ()
{
  m_dataRateNetdeviceEnergyModel.SetTypeId ("ns3::DataRateNetdeviceEnergyModel");
}

DataRateNetdeviceEnergyHelper::~DataRateNetdeviceEnergyHelper ()
{
}

void
DataRateNetdeviceEnergyHelper::SetUnit (uint64_t bps)
{
  m_unit = bps;
}

void
DataRateNetdeviceEnergyHelper::SetConsumptionValues (std::map<uint64_t, double> values)
{
  m_values = values;
}

Ptr<NetdeviceEnergyModel>
DataRateNetdeviceEnergyHelper::DoInstall (Ptr<NetDevice> netdevice) const
{
  NS_ASSERT (netdevice != NULL);
  // check if energy model already exists
  Ptr<NetdeviceEnergyModel> nenergy = netdevice->GetObject<NetdeviceEnergyModel> ();
  if (nenergy != NULL)
    {
      NS_FATAL_ERROR ("Net device energy model already installed!");
    }
  Ptr<DataRateNetdeviceEnergyModel> energy =
      m_dataRateNetdeviceEnergyModel.Create<DataRateNetdeviceEnergyModel> ();
  NS_ASSERT (energy != NULL);
  energy->SetNetdevice (netdevice);
  energy->SetUnit (m_unit);
  energy->SetConsumptionValues (m_values);
  netdevice->AggregateObject (energy);
  energy->Initialize ();
  return energy;
}

} // namespace ns3
