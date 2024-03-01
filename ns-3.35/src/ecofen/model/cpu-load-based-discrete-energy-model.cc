/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * The GPLv2 License (GPLv2)
 *
 * Copyright (c) 2023 Rui Pedro C. Monteiro
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Rui Pedro C. Monteiro <rui.p.monteiro@inesctec.pt>
 */

#include "cpu-load-based-discrete-energy-model.h"
#include "ns3/ofswitch13-device.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (CpuLoadBasedDiscreteEnergyModel);

TypeId
CpuLoadBasedDiscreteEnergyModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CpuLoadBasedDiscreteEnergyModel")
                          .SetParent<CpuLoadBasedEnergyModel> ()
                          .AddConstructor<CpuLoadBasedDiscreteEnergyModel> ();
  return tid;
}

CpuLoadBasedDiscreteEnergyModel::CpuLoadBasedDiscreteEnergyModel ()
{
}

CpuLoadBasedDiscreteEnergyModel::~CpuLoadBasedDiscreteEnergyModel ()
{
}

double
CpuLoadBasedDiscreteEnergyModel::GetPowerConsumption ()
{
  Ptr<OFSwitch13Device> device = m_node->GetObject<OFSwitch13Device> ();
  double cpuUsage = device->GetCpuUsage ();

  int i = 0;
  for (double percent : m_percentages)
    {
      if (cpuUsage <= percent)
        break;
      i++;
    }

  if (i >= int (m_values.size ())) // cpuUsage is greater than the last percentage
    return m_values.back (); // return the last known value

  return m_values[i];
}

} // namespace ns3
