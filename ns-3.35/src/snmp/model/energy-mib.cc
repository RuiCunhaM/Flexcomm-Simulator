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
 * along with this program.  If not, see <http: //www.gnu.org/licenses/>.
 *
 * Author: Rui Pedro C. Monteiro <rui.p.monteiro@inesctec.pt>
 */

#include "energy-mib.h"
#include "ns3/log.h"
#include "ns3/node-energy-model.h"
#include "ns3/string.h"
#include "ns3/output-stream-wrapper.h"
#include "ns3/core-module.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (EnergyMib);

TypeId
EnergyMib::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::EnergyMib")
                          .SetParent<Object> ()
                          .SetGroupName ("Snmp")
                          .AddConstructor<EnergyMib> ();
  return tid;
}

EnergyMib::EnergyMib ()
{
  m_mibName = "Energy";
  m_maxConsumed = 0.0;
}

EnergyMib::~EnergyMib ()
{
}

void
EnergyMib::WriteToFile (Ptr<OutputStreamWrapper> streamWrapper)
{
  Ptr<NodeEnergyModel> model = m_node->GetObject<NodeEnergyModel> ();
  double currConso = model->GetCurrentPowerConsumption ();
  double drawn = model->GetPowerDrawn ();

  if (currConso > m_maxConsumed)
    m_maxConsumed = currConso;

  // Write values
  std::ostream *stream = streamWrapper->GetStream ();
  *stream << "1.3.6.1.2.1.229.1.5.1.0.2|66|" << (u_int32_t) drawn << std::endl;
  *stream << "1.3.6.1.2.1.229.1.5.1.0.7|66|" << (u_int32_t) m_maxConsumed << std::endl;
}

void
EnergyMib::Initialize ()
{
}

} // namespace ns3
