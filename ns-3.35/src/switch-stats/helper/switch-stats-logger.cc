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

#include "switch-stats-logger.h"
#include "ns3/switch-stats.h"

namespace ns3 {

SwitchStatsLogger::SwitchStatsLogger ()
{
  SwitchStatsLogger ("switch-stats");
}

SwitchStatsLogger::SwitchStatsLogger (std::string path)
{
  m_streamWrapper = Create<OutputStreamWrapper> (path, std::ios::out);
}

SwitchStatsLogger::~SwitchStatsLogger ()
{
  m_streamWrapper = 0;
}

void
SwitchStatsLogger::LogStats (Time interval, Time stop, Ptr<Node> node)
{
  Ptr<SwitchStats> stats = node->GetObject<SwitchStats> ();

  if (stats)
    stats->GetStatsLog (interval, stop, m_streamWrapper);
}

void
SwitchStatsLogger::LogStats (Time interval, Time stop, NodeContainer c)
{
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); i++)
    {
      LogStats (interval, stop, *i);
    }
}

void
SwitchStatsLogger::LogStatsAll (Time interval, Time stop)
{
  LogStats (interval, stop, NodeContainer::GetGlobalSwitches ());
}

} // namespace ns3
