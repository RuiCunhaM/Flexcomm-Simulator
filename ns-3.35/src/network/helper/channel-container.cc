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

#include "channel-container.h"
#include "ns3/channel-list.h"
#include "ns3/net-device.h"
#include "ns3/node.h"

namespace ns3 {

ChannelContainer::ChannelContainer ()
{
}

ChannelContainer::ChannelContainer (Ptr<Channel> channel)
{
  m_channels.push_back (channel);
}

ChannelContainer::ChannelContainer (const ChannelContainer &a, const ChannelContainer &b)
{
  Add (a);
  Add (b);
}
ChannelContainer::ChannelContainer (const ChannelContainer &a, const ChannelContainer &b,
                                    const ChannelContainer &c)
{
  Add (a);
  Add (b);
  Add (c);
}
ChannelContainer::ChannelContainer (const ChannelContainer &a, const ChannelContainer &b,
                                    const ChannelContainer &c, const ChannelContainer &d)
{
  Add (a);
  Add (b);
  Add (c);
  Add (d);
}

ChannelContainer::ChannelContainer (const ChannelContainer &a, const ChannelContainer &b,
                                    const ChannelContainer &c, const ChannelContainer &d,
                                    const ChannelContainer &e)
{
  Add (a);
  Add (b);
  Add (c);
  Add (d);
  Add (e);
}

ChannelContainer::Iterator
ChannelContainer::Begin (void) const
{
  return m_channels.begin ();
}

ChannelContainer::Iterator
ChannelContainer::End (void) const
{
  return m_channels.end ();
}

uint32_t
ChannelContainer::GetN (void) const
{
  return m_channels.size ();
}

Ptr<Channel>
ChannelContainer::Get (uint32_t i) const
{
  return m_channels[i];
}

void
ChannelContainer::Add (ChannelContainer other)
{
  for (Iterator i = other.Begin (); i != other.End (); i++)
    {
      m_channels.push_back (*i);
    }
}

void
ChannelContainer::Add (Ptr<Channel> channel)
{
  m_channels.push_back (channel);
}

ChannelContainer
ChannelContainer::GetGlobal (void)
{
  ChannelContainer c;
  for (ChannelList::Iterator i = ChannelList::Begin (); i != ChannelList::End (); ++i)
    {
      c.Add (*i);
    }
  return c;
}

ChannelContainer
ChannelContainer::GetSwitch2Switch (void)
{
  ChannelContainer c;
  for (ChannelList::Iterator i = ChannelList::Begin (); i != ChannelList::End (); ++i)
    {
      bool isValid = true;

      for (size_t n = 0; n < (*i)->GetNDevices (); n++)
        isValid = isValid && (*i)->GetDevice (n)->GetNode ()->IsSwitch ();

      if (isValid)
        c.Add (*i);
    }
  return c;
}

} // namespace ns3
