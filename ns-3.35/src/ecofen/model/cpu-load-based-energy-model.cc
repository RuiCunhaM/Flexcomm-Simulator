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

#include "cpu-load-based-energy-model.h"
#include "ns3/node-energy-model.h"
#include "ns3/ofswitch13-device.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (CpuLoadBasedEnergyModel);

TypeId
CpuLoadBasedEnergyModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CpuLoadBasedEnergyModel")
                          .SetParent<NodeEnergyModel> ()
                          .AddConstructor<CpuLoadBasedEnergyModel> ();
  return tid;
}

CpuLoadBasedEnergyModel::CpuLoadBasedEnergyModel ()
{
  m_percentages = {0.0, 1.0};
  m_values = {100.0, 200.0};
}

CpuLoadBasedEnergyModel::~CpuLoadBasedEnergyModel ()
{
}

double
CpuLoadBasedEnergyModel::GetPowerConsumption ()
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

  if (i == 0)
    return m_values[0];

  if (i >= int (m_values.size ())) // cpuUsage is greater than the last known percentage
    return m_values.back (); // return the last known value

  // Linear interpolation
  return m_values[i - 1] +
         (cpuUsage - m_percentages[i - 1]) *
             ((m_values[i] - m_values[i - 1]) / (m_percentages[i] - m_percentages[i - 1]));
}

double
CpuLoadBasedEnergyModel::GetMaxPowerConsumption ()
{
  return m_values.back ();
}

double
CpuLoadBasedEnergyModel::GetMinPowerConsumption ()
{
  return m_values.front ();
}

void
CpuLoadBasedEnergyModel::SetUsageValues (std::map<double, double> values)
{
  m_percentages.clear ();
  m_values.clear ();
  for (const auto &pair : values)
    {
      m_percentages.push_back (pair.first);
      m_values.push_back (pair.second);
    }
}

} // namespace ns3
