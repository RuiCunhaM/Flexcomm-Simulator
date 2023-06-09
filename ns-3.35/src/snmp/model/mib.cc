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

#include "mib.h"
#include "ns3/log.h"
#include "ns3/string.h"
#include "ns3/simulator.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Mib);

TypeId
Mib::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::Mib")
          .SetParent<Object> ()
          .SetGroupName ("Snmp")
          .AddAttribute ("Name", "The name of this MIB", StringValue (),
                         MakeStringAccessor (&Mib::m_mibName), MakeStringChecker ());
  return tid;
}

Mib::~Mib ()
{
}

void
Mib::SetNode (Ptr<Node> node)
{
  m_node = node;
}

void
Mib::WriteToFile (Ptr<OutputStreamWrapper> streamWrapper)
{
}

void
Mib::Initialize ()
{
}

} // namespace ns3
