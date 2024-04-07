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

#ifndef DATA_RATE_NETDEVICE_ENERGY_HELPER_H
#define DATA_RATE_NETDEVICE_ENERGY_HELPER_H

#include "netdevice-energy-helper.h"
#include "ns3/net-device.h"
#include "ns3/object-factory.h"

namespace ns3 {

class DataRateNetdeviceEnergyHelper : public NetdeviceEnergyHelper
{
public:
  DataRateNetdeviceEnergyHelper ();
  ~DataRateNetdeviceEnergyHelper ();

  void SetUnit (uint64_t bps);
  void SetConsumptionValues (std::map<uint64_t, double> values);

private:
  virtual Ptr<NetdeviceEnergyModel> DoInstall (Ptr<NetDevice> netdevice) const;

private:
  ObjectFactory m_dataRateNetdeviceEnergyModel;
  uint64_t m_unit;
  std::map<uint64_t, double> m_values;
};

} // namespace ns3

#endif /* DATA_RATE_NETDEVICE_ENERGY_HELPER_H */
