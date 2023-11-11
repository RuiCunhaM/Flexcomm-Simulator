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

#include "switch-stats.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (SwitchStats);

TypeId
SwitchStats::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::SwitchStats").SetParent<Object> ().AddConstructor<SwitchStats> ();
  return tid;
}

SwitchStats::SwitchStats ()
{
  m_packets = 0;
  m_droppedPackets = 0;
  m_bytes = 0;
}

SwitchStats::~SwitchStats ()
{
}

void
SwitchStats::SetNode (Ptr<Node> node)
{
  m_node = node;
  m_nodeName = Names::FindName (node);
  m_device = node->GetObject<OFSwitch13Device> ();

  m_device->TraceConnectWithoutContext ("PipelinePacket",
                                        MakeCallback (&SwitchStats::HandlePipelinePacket, this));
  m_device->TraceConnectWithoutContext ("OverloadDrop",
                                        MakeCallback (&SwitchStats::HandleDroppedPacket, this));
}

void
SwitchStats::GetStatsLog (Time interval, Time stop, Ptr<OutputStreamWrapper> streamWrapper)
{
  Time i = Seconds (0.0);
  while (i <= stop)
    {
      Simulator::Schedule (i, &SwitchStats::LogStats, this, streamWrapper);
      i += interval;
    }
}

void
SwitchStats::LogStats (Ptr<OutputStreamWrapper> streamWrapper)
{
  std::ostream *stream = streamWrapper->GetStream ();

  *stream << std::fixed << Simulator::Now ().GetSeconds () << ";" << m_nodeName << ";"
          << m_device->GetCpuUsage () << ";" << m_packets << ";" << m_droppedPackets << ";"
          << m_bytes << "\n";

  // Restart counters
  m_packets = 0;
  m_droppedPackets = 0;
  m_bytes = 0;
}

void
SwitchStats::HandlePipelinePacket (Ptr<const Packet> packet)
{
  m_packets++;
  m_bytes += packet->GetSize ();
}

void
SwitchStats::HandleDroppedPacket (Ptr<const Packet> packet)
{
  m_droppedPackets++;
}

} // namespace ns3
