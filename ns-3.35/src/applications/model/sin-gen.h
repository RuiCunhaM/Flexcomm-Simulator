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

#ifndef __SIN_GEN_H__
#define __SIN_GEN_H__

#include "ns3/application.h"
#include "ns3/data-rate.h"
#include "ns3/socket.h"

namespace ns3 {
class SinGen : public Application
{

public:
  static TypeId GetTypeId ();
  SinGen ();
  virtual ~SinGen ();

private:
  virtual void StartApplication ();
  virtual void StopApplication ();

  Ptr<Socket> m_socket;

  DataRate m_a;
  DataRate m_const;
  double m_b;
  double m_c;
  uint32_t m_pktSize;
  Time::Unit m_unit;

  TypeId m_prototcolTid;
  Address m_remote;

  void ScheduleNextPacket ();
  void SendPacket ();
};

} // namespace ns3

#endif
