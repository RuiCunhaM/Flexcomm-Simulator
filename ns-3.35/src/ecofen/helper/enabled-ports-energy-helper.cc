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

#include "enabled-ports-energy-helper.h"
#include "ns3/enabled-ports-energy-model.h"

namespace ns3 {

EnabledPortsEnergyHelper::EnabledPortsEnergyHelper ()
{
  m_enabledPortsEnergyModel.SetTypeId ("ns3::EnabledPortsEnergyModel");
}

EnabledPortsEnergyHelper::~EnabledPortsEnergyHelper ()
{
}

Ptr<NodeEnergyModel>
EnabledPortsEnergyHelper::DoInstall (Ptr<Node> node) const
{
  NS_ASSERT (node != NULL);
  // check if energy model already exists
  Ptr<NodeEnergyModel> nenergy = node->GetObject<NodeEnergyModel> ();
  if (nenergy != NULL)
    {
      NS_FATAL_ERROR ("Node energy model already installed!");
    }
  Ptr<EnabledPortsEnergyModel> energy =
      m_enabledPortsEnergyModel.Create<EnabledPortsEnergyModel> ();
  NS_ASSERT (energy != NULL);
  energy->SetNode (node);
  energy->SetIdleConsumption (m_idle);
  energy->SetPortsConsumption (m_values);
  node->AggregateObject (energy);
  energy->Initialize ();
  return energy;
}

void
EnabledPortsEnergyHelper::SetIdleConsumption (double consumption)
{
  m_idle = consumption;
}

void
EnabledPortsEnergyHelper::SetPortsConsumptions (std::map<uint64_t, double> values)
{
  m_values = values;
}

} // namespace ns3
