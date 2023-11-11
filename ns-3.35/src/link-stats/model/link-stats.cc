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

#include "link-stats.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (LinkStats);

TypeId
LinkStats::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LinkStats").SetParent<Object> ().AddConstructor<LinkStats> ();
  return tid;
}

LinkStats::LinkStats ()
{
  m_channel = 0;
}

LinkStats::~LinkStats ()
{
}

void
LinkStats::LogStats (Time interval, Time stop, Ptr<OutputStreamWrapper> streamWrapper)
{
  Time i = Seconds (0.0);
  while (i <= stop)
    {
      Simulator::Schedule (i, &LinkStats::LogStatsInternal, this, streamWrapper);
      i += interval;
    }
}

void
LinkStats::LogStatsInternal (Ptr<OutputStreamWrapper> streamWrapper)
{
  std::string linkName = Names::FindName (m_channel);

  std::ostream *stream = streamWrapper->GetStream ();
  *stream << std::fixed << Simulator::Now ().GetSeconds () << ";" << linkName << ";"
          << m_channel->GetChannelUsage () << "\n";
}

void
LinkStats::SetChannel (Ptr<Channel> channel)
{
  m_channel = channel;
}

} // namespace ns3
