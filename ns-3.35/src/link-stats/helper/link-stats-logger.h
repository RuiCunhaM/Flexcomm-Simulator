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

#ifndef LINK_STATS_LOGGER_H
#define LINK_STATS_LOGGER_H

#include "ns3/channel-container.h"
#include "ns3/core-module.h"
#include "ns3/output-stream-wrapper.h"
#include "ns3/channel.h"

namespace ns3 {

class LinkStatsLogger : public Object
{
public:
  LinkStatsLogger ();
  virtual ~LinkStatsLogger ();

  void ComputeStats (Time interval, Time stop, Ptr<Channel> channel);
  void ComputeStats (Time interval, Time stop, ChannelContainer c);
  void ComputeStatsAll (Time interval, Time stop);

  void ComputeStatsLog (Time interval, Time stop, Ptr<Channel> channel,
                        std::string path = "link-stats");
  void ComputeStatsLog (Time interval, Time stop, ChannelContainer c,
                        std::string path = "link-stats");
  void ComputeStatsAllLog (Time interval, Time stop, std::string path = "link-stats");

private:
  void CreateLogFile (std::string path);

  Ptr<OutputStreamWrapper> m_streamWrapper;
};

} // namespace ns3

#endif /* LINK_STATS_LOGGER_H */
