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
#ifndef SWITCH_STATS_H
#define SWITCH_STATS_H

#include "ns3/core-module.h"
#include "ns3/node.h"
#include "ns3/ofswitch13-device.h"
#include "ns3/output-stream-wrapper.h"

namespace ns3 {

class SwitchStats : public Object
{
public:
  static TypeId GetTypeId (void);
  SwitchStats ();
  virtual ~SwitchStats ();

  void SetNode (Ptr<Node> node);
  void GetStatsLog (Time interval, Time stop, Ptr<OutputStreamWrapper> streamWrapper);

private:
  uint32_t m_packets;
  uint32_t m_droppedPackets;
  uint32_t m_bytes;
  std::string m_nodeName;
  Ptr<Node> m_node;
  Ptr<OFSwitch13Device> m_device;

  void LogStats (Ptr<OutputStreamWrapper> streamWrapper);
  void HandlePipelinePacket (Ptr<const Packet> packet);
  void HandleDroppedPacket (Ptr<const Packet> packet);
};

} // namespace ns3

#endif /* SWITCH_STATS_H */
