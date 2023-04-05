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

#include "switch-stats-helper.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (SwitchStatsHelper);

TypeId
SwitchStatsHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SwitchStatsHelper").SetParent<Object> ();
  return tid;
}

SwitchStatsHelper::SwitchStatsHelper ()
{
  m_factory.SetTypeId ("ns3::SwitchStats");
}

SwitchStatsHelper::~SwitchStatsHelper ()
{
}

void
SwitchStatsHelper::Install (Ptr<Node> node)
{
  Ptr<SwitchStats> stats = m_factory.Create<SwitchStats> ();
  stats->SetNode (node);
  node->AggregateObject (stats);
}

void
SwitchStatsHelper::Install (NodeContainer c)
{
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); i++)
    {
      Install (*i);
    }
}

void
SwitchStatsHelper::Install (std::string name)
{
  Ptr<Node> node = Names::Find<Node> (name);
  Install (node);
}

void
SwitchStatsHelper::InstallAll ()
{
  Install (NodeContainer::GetGlobalSwitches ());
}

} // namespace ns3
