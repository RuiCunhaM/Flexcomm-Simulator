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

#ifndef CHANNEL_CONTAINER_H
#define CHANNEL_CONTAINER_H

#include "ns3/channel.h"
#include "ns3/core-module.h"

namespace ns3 {

class ChannelContainer
{
public:
  typedef std::vector<Ptr<Channel>>::const_iterator Iterator;

  ChannelContainer ();

  ChannelContainer (Ptr<Channel> channel);

  ChannelContainer (const ChannelContainer &a, const ChannelContainer &b);

  ChannelContainer (const ChannelContainer &a, const ChannelContainer &b,
                    const ChannelContainer &c);

  ChannelContainer (const ChannelContainer &a, const ChannelContainer &b, const ChannelContainer &c,
                    const ChannelContainer &d);

  ChannelContainer (const ChannelContainer &a, const ChannelContainer &b, const ChannelContainer &c,
                    const ChannelContainer &d, const ChannelContainer &e);

  Iterator Begin (void) const;

  Iterator End (void) const;

  uint32_t GetN (void) const;

  Ptr<Channel> Get (uint32_t i) const;

  void Add (ChannelContainer other);

  void Add (Ptr<Channel> channel);

  static ChannelContainer GetGlobal (void);
  static ChannelContainer GetSwitch2Switch (void);

private:
  std::vector<Ptr<Channel>> m_channels;
};

} // namespace ns3

#endif /* channel_CONTAINER_H */
