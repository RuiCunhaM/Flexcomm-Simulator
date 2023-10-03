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

#ifndef LINK_STATS_H
#define LINK_STATS_H

#include "ns3/core-module.h"
#include "ns3/channel.h"
#include "ns3/output-stream-wrapper.h"

namespace ns3 {

class LinkStats : public Object
{
public:
  static TypeId GetTypeId (void);
  LinkStats ();
  virtual ~LinkStats ();

  void LogStats (Time interval, Time stop, Ptr<OutputStreamWrapper> streamWrapper);
  void LogStatsInternal (Ptr<OutputStreamWrapper> streamWrapper);
  void SetChannel (Ptr<Channel> channel);

private:
  Ptr<Channel> m_channel;
};

} // namespace ns3

#endif /* LINK_STATS_H */
