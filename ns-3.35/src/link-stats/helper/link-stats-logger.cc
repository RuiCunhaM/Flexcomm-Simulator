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

#include "link-stats-logger.h"
#include "ns3/link-stats.h"

namespace ns3 {

Ptr<OutputStreamWrapper> LinkStatsLogger::m_streamWrapper = NULL;
ChannelContainer LinkStatsLogger::m_links = ChannelContainer ();
ChannelContainer LinkStatsLogger::m_linksLog = ChannelContainer ();

LinkStatsLogger::LinkStatsLogger ()
{
  m_streamWrapper = 0;
}

LinkStatsLogger::~LinkStatsLogger ()
{
}

void
LinkStatsLogger::ComputeStats (Time interval, Time stop, Ptr<Channel> channel)
{
  Time i = Seconds (0.0);
  while (i <= stop)
    {
      Simulator::Schedule (i, &Channel::UpdateUsage, channel);
      i += interval;
    }
}

void
LinkStatsLogger::Compute ()
{
  for (ChannelContainer::Iterator i = m_linksLog.Begin (); i != m_linksLog.End (); ++i)
    (*i)->UpdateUsage ();
}

void
LinkStatsLogger::ComputeStats (Time interval, Time stop, ChannelContainer c)
{
  for (ChannelContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    m_links.Add (*i);

  Time i = Seconds (0.0);
  while (i <= stop)
    {
      Simulator::Schedule (i, &LinkStatsLogger::Compute, this);
      i += interval;
    }
}

void
LinkStatsLogger::ComputeStatsAll (Time interval, Time stop)
{
  ComputeStats (interval, stop, ChannelContainer::GetSwitch2Switch ());
}

void
LinkStatsLogger::ComputeStatsLog (Time interval, Time stop, Ptr<Channel> channel, std::string path)
{
  Ptr<LinkStats> ls = channel->GetObject<LinkStats> ();

  if (ls)
    {
      CreateLogFile (path);
      ls->LogStats (interval, stop, m_streamWrapper);
    }
}

void
LinkStatsLogger::Log ()
{
  for (ChannelContainer::Iterator i = m_linksLog.Begin (); i != m_linksLog.End (); ++i)
    (*i)->GetObject<LinkStats> ()->LogStatsInternal (m_streamWrapper);
}

void
LinkStatsLogger::ComputeStatsLog (Time interval, Time stop, ChannelContainer c, std::string path)
{
  CreateLogFile (path);
  for (ChannelContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<LinkStats> ls = (*i)->GetObject<LinkStats> ();

      if (ls)
        m_linksLog.Add (*i);
    }

  std::ostream *stream = m_streamWrapper->GetStream ();
  *stream << "Time;LinkName;LinkUsage\n";

  Time i = Seconds (0.0);
  while (i <= stop)
    {
      Simulator::Schedule (i, &LinkStatsLogger::Log, this);
      i += interval;
    }
}

void
LinkStatsLogger::ComputeStatsAllLog (Time interval, Time stop, std::string path)
{
  ComputeStatsLog (interval, stop, ChannelContainer::GetSwitch2Switch (), path);
}

void
LinkStatsLogger::CreateLogFile (std::string path)
{
  if (!m_streamWrapper)
    m_streamWrapper = Create<OutputStreamWrapper> (path, std::ios::out);
}

} // namespace ns3
