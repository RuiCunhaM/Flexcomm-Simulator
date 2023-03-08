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

#include "sin-gen.h"
#include "ns3/address.h"
#include "ns3/double.h"
#include "ns3/uinteger.h"
#include "ns3/core-module.h"
#include "ns3/udp-socket-factory.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SinGen");

NS_OBJECT_ENSURE_REGISTERED (SinGen);

TypeId
SinGen::GetTypeId ()
{
  static TypeId tid =
      TypeId ("ns3::SinGen")
          .SetParent<Application> ()
          .SetGroupName ("Applications")
          .AddConstructor<SinGen> ()
          .AddAttribute ("Const", "Const Data Rate Value", DataRateValue (DataRate ("3Mb/s")),
                         MakeDataRateAccessor (&SinGen::m_const), MakeDataRateChecker ())
          .AddAttribute ("A", "A Data Rate Value", DataRateValue (DataRate ("1Mb/s")),
                         MakeDataRateAccessor (&SinGen::m_a), MakeDataRateChecker ())
          .AddAttribute ("B", "B Value", DoubleValue (0.5), MakeDoubleAccessor (&SinGen::m_b),
                         MakeDoubleChecker<double> ())
          .AddAttribute ("C", "C Value", DoubleValue (0.0), MakeDoubleAccessor (&SinGen::m_c),
                         MakeDoubleChecker<double> ())
          .AddAttribute ("Remote", "The address of the destination", AddressValue (),
                         MakeAddressAccessor (&SinGen::m_remote), MakeAddressChecker ())
          .AddAttribute ("PacketSize", "The packet size", UintegerValue (1024),
                         MakeUintegerAccessor (&SinGen::m_pktSize),
                         MakeUintegerChecker<uint32_t> ())
          .AddAttribute (
              "Unit", "The time unit", EnumValue (Time::Unit::S),
              MakeEnumAccessor (&SinGen::m_unit),
              MakeEnumChecker (Time::Unit::H, "h", Time::Unit::MIN, "min", Time::Unit::S, "s"))
          .AddAttribute ("Protocol", "The protocol", TypeIdValue (UdpSocketFactory::GetTypeId ()),
                         MakeTypeIdAccessor (&SinGen::m_prototcolTid), MakeTypeIdChecker ());
  return tid;
}

SinGen::SinGen ()
{
  NS_LOG_FUNCTION (this);
}

SinGen::~SinGen ()
{
  NS_LOG_FUNCTION (this);
}

void
SinGen::StartApplication ()
{
  NS_LOG_FUNCTION (this);
  if (!m_socket)
    {
      m_socket = Socket::CreateSocket (GetNode (), m_prototcolTid);
      m_socket->Bind ();
      m_socket->Connect (m_remote);
    }
  ScheduleNextPacket ();
}

void
SinGen::StopApplication ()
{
  NS_LOG_FUNCTION (this);

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void
SinGen::ScheduleNextPacket ()
{
  NS_LOG_FUNCTION (this);

  uint32_t bits = m_pktSize * 8;

  double rate = m_a.GetBitRate () * sin (m_b * Simulator::Now ().ToDouble (m_unit) + m_c) +
                m_const.GetBitRate ();

  Time nextTime = Seconds (bits / rate);

  Simulator::Schedule (nextTime, &SinGen::SendPacket, this);
}

void
SinGen::SendPacket ()
{
  NS_LOG_FUNCTION (this);

  Ptr<Packet> packet = Create<Packet> (m_pktSize);
  m_socket->Send (packet);

  ScheduleNextPacket ();
}

} // namespace ns3
