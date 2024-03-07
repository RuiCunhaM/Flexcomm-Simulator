/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007, 2008 University of Washington
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "point-to-point-ethernet-channel.h"
#include "point-to-point-ethernet-net-device.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PointToPointEthernetChannel");

NS_OBJECT_ENSURE_REGISTERED (PointToPointEthernetChannel);

TypeId
PointToPointEthernetChannel::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::PointToPointEthernetChannel")
          .SetParent<Channel> ()
          .SetGroupName ("PointToPointEthernet")
          .AddConstructor<PointToPointEthernetChannel> ()
          .AddAttribute ("Delay", "Propagation delay through the channel", TimeValue (Seconds (0)),
                         MakeTimeAccessor (&PointToPointEthernetChannel::m_delay),
                         MakeTimeChecker ())
          .AddTraceSource (
              "TxRxPointToPointEthernet",
              "Trace source indicating transmission of packet "
              "from the PointToPointEthernetChannel, used by the Animation "
              "interface.",
              MakeTraceSourceAccessor (&PointToPointEthernetChannel::m_txrxPointToPointEthernet),
              "ns3::PointToPointEthernetChannel::TxRxAnimationCallback");
  return tid;
}

//
// By default, you get a channel that
// has an "infitely" fast transmission speed and zero delay.
PointToPointEthernetChannel::PointToPointEthernetChannel ()
    : Channel (), m_delay (Seconds (0.)), m_nDevices (0)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_lastTime = Simulator::Now ();
  m_bytesTransmitted = 0;
}

void
PointToPointEthernetChannel::Attach (Ptr<PointToPointEthernetNetDevice> device)
{
  NS_LOG_FUNCTION (this << device);
  NS_ASSERT_MSG (m_nDevices < N_DEVICES, "Only two devices permitted");
  NS_ASSERT (device != 0);

  m_link[m_nDevices++].m_src = device;
  //
  // If we have both devices connected to the channel, then finish introducing
  // the two halves and set the links to IDLE.
  //
  if (m_nDevices == N_DEVICES)
    {
      m_link[0].m_dst = m_link[1].m_src;
      m_link[1].m_dst = m_link[0].m_src;
      m_link[0].m_state = IDLE;
      m_link[1].m_state = IDLE;
    }
}

bool
PointToPointEthernetChannel::TransmitStart (Ptr<const Packet> p,
                                            Ptr<PointToPointEthernetNetDevice> src, Time txTime)
{
  NS_LOG_FUNCTION (this << p << src);
  NS_LOG_LOGIC ("UID is " << p->GetUid () << ")");

  NS_ASSERT (m_link[0].m_state != INITIALIZING);
  NS_ASSERT (m_link[1].m_state != INITIALIZING);

  uint32_t wire = src == m_link[0].m_src ? 0 : 1;

  Simulator::ScheduleWithContext (m_link[wire].m_dst->GetNode ()->GetId (), txTime + m_delay,
                                  &PointToPointEthernetNetDevice::Receive, m_link[wire].m_dst,
                                  p->Copy ());

  // Call the tx anim callback on the net device
  m_txrxPointToPointEthernet (p, src, m_link[wire].m_dst, txTime, txTime + m_delay);
  m_bytesTransmitted += p->GetSize ();
  return true;
}

std::size_t
PointToPointEthernetChannel::GetNDevices (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_nDevices;
}

Ptr<PointToPointEthernetNetDevice>
PointToPointEthernetChannel::GetPointToPointEthernetDevice (std::size_t i) const
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_ASSERT (i < 2);
  return m_link[i].m_src;
}

Ptr<NetDevice>
PointToPointEthernetChannel::GetDevice (std::size_t i) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return GetPointToPointEthernetDevice (i);
}

NetDeviceContainer
PointToPointEthernetChannel::GetDevices () const
{
  NS_LOG_FUNCTION_NOARGS ();
  NetDeviceContainer r = NetDeviceContainer (m_link[0].m_src);
  r.Add (m_link[1].m_src);
  return r;
}

Time
PointToPointEthernetChannel::GetDelay (void) const
{
  return m_delay;
}

Ptr<PointToPointEthernetNetDevice>
PointToPointEthernetChannel::GetSource (uint32_t i) const
{
  return m_link[i].m_src;
}

Ptr<PointToPointEthernetNetDevice>
PointToPointEthernetChannel::GetDestination (uint32_t i) const
{
  return m_link[i].m_dst;
}

bool
PointToPointEthernetChannel::IsInitialized (void) const
{
  NS_ASSERT (m_link[0].m_state != INITIALIZING);
  NS_ASSERT (m_link[1].m_state != INITIALIZING);
  return true;
}

void
PointToPointEthernetChannel::UpdateUsage ()
{
  Time delta = Simulator::Now () - m_lastTime;

  if (delta.GetSeconds () > 0)
    {
      m_lastTime = Simulator::Now ();
      uint64_t bits = m_bytesTransmitted * 8;
      m_bytesTransmitted = 0;
      m_usage = (bits / delta.GetSeconds ()) / GetDataRate ().GetBitRate ();
    }
}

DataRate
PointToPointEthernetChannel::GetDataRate ()
{
  return m_link[0].m_dst->GetDataRate ();
}

} // namespace ns3
