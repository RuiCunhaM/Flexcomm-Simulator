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

#ifndef DATA_RATE_NETDEVICE_ENERGY_MODEL_H_
#define DATA_RATE_NETDEVICE_ENERGY_MODEL_H_

#include "netdevice-energy-model.h"

namespace ns3 {

class DataRateNetdeviceEnergyModel : public NetdeviceEnergyModel
{
public:
  static TypeId GetTypeId (void);
  DataRateNetdeviceEnergyModel ();
  virtual ~DataRateNetdeviceEnergyModel ();

  void Initialize ();

  void SetUnit (uint64_t bps);
  void SetConsumptionValues (std::map<uint64_t, double> values);

  virtual void SetNetdevice (Ptr<NetDevice> netdevice);
  virtual Ptr<NetDevice> GetNetdevice (void) const;
  virtual double GetPowerConsumption (void);

private:
  Ptr<NetDevice> m_netdevice;
  uint64_t m_unit;
  std::map<uint64_t, double> m_values;
};

} // namespace ns3

#endif /* DATA_RATE_NETDEVICE_ENERGY_MODEL_H_ */
