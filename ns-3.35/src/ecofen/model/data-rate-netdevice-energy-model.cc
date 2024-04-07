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

#include "data-rate-netdevice-energy-model.h"
#include "ns3/channel.h"
#include "ns3/data-rate.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (DataRateNetdeviceEnergyModel);

TypeId
DataRateNetdeviceEnergyModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DataRateNetdeviceEnergyModel")
                          .SetParent<NetdeviceEnergyModel> ()
                          .AddConstructor<DataRateNetdeviceEnergyModel> ();
  return tid;
}

DataRateNetdeviceEnergyModel::DataRateNetdeviceEnergyModel ()
{
}

DataRateNetdeviceEnergyModel::~DataRateNetdeviceEnergyModel ()
{
}

void
DataRateNetdeviceEnergyModel::Initialize ()
{
}

void
DataRateNetdeviceEnergyModel::SetUnit (uint64_t bps)
{
  m_unit = bps;
}

void
DataRateNetdeviceEnergyModel::SetConsumptionValues (std::map<uint64_t, double> values)
{
  m_values = values;
}

void
DataRateNetdeviceEnergyModel::SetNetdevice (Ptr<NetDevice> netdevice)
{
  NS_ASSERT (netdevice != NULL);
  m_netdevice = netdevice;
}

Ptr<NetDevice>
DataRateNetdeviceEnergyModel::GetNetdevice () const
{
  return m_netdevice;
}

double
DataRateNetdeviceEnergyModel::GetPowerConsumption (void)
{
  Ptr<Channel> channel = m_netdevice->GetChannel ();
  uint64_t bps = channel->GetDataRate ().GetBitRate ();
  uint64_t current_bps = bps * channel->GetChannelUsage ();

  return m_values[bps] * (current_bps / m_unit);
}

} // namespace ns3
