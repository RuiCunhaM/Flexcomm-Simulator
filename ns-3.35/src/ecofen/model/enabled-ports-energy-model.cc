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

#include "enabled-ports-energy-model.h"
#include "ns3/node-energy-model.h"
#include "ns3/ofswitch13-device.h"
#include "ns3/ofswitch13-port.h"
#include <cstdint>

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (EnabledPortsEnergyModel);

TypeId
EnabledPortsEnergyModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::EnabledPortsEnergyModel")
                          .SetParent<NodeEnergyModel> ()
                          .AddConstructor<EnabledPortsEnergyModel> ();
  return tid;
}

EnabledPortsEnergyModel::EnabledPortsEnergyModel ()
{
}

EnabledPortsEnergyModel::~EnabledPortsEnergyModel ()
{
}

double
EnabledPortsEnergyModel::GetPowerConsumption ()
{
  double conso = m_idle;

  Ptr<OFSwitch13Device> device = m_node->GetObject<OFSwitch13Device> ();

  for (uint32_t i = 0; i < device->GetNSwitchPorts (); i++)
    {
      Ptr<OFSwitch13Port> port = device->GetSwitchPort (i + 1);
      DataRate dr = port->GetPortDevice ()->GetChannel ()->GetDataRate ();
      conso += m_values[dr.GetBitRate ()];
    }

  return conso;
}

void
EnabledPortsEnergyModel::SetIdleConsumption (double consumption)
{
  m_idle = consumption;
}

void
EnabledPortsEnergyModel::SetPortsConsumption (std::map<uint64_t, double> values)
{
  m_values = values;
}

} // namespace ns3
