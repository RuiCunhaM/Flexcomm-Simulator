/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 CNRS, France.
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
 *
 * Author: Anne-Cecile Orgerie <anne-cecile.orgerie@irisa.fr>
 */

#ifndef AGENDA_H
#define AGENDA_H

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/type-id.h"
#include "ns3/net-device.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include <vector>
#include "ns3/net-device-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"

namespace ns3 {

/**
 * \ingroup energy
 * \brief Base class for net devices agendas.
 */

class Agenda : public Object
{
public:
  static TypeId GetTypeId (void);
  Agenda ();
  virtual ~Agenda ();

  /**
   * \brief Sets pointer to netdevice containing this Agenda.
   *
   * \param link Pointer to the netdevice containing this.
   */
  void SetNetdev (Ptr<NetDevice> netdev);

  /**
   * \returns Pointer to netdev containing this Agenda.
   *
   * When a subclass needs to get access to the underlying link base class to
   * print the NetDeviceId for example, it can invoke this method.
   */
  Ptr<NetDevice> GetNetdev (void) const;

  /**
   * Add a commnication period into the agenda.
   *
   */
  void AddEvent (double start, double stop);

  /**
   * Get the vector containing the dates.
   */
  std::vector<double> GetTab (void) const;

public:
  std::vector<double> m_agenda;
  Ptr<NetDevice> m_netdev;
};

} // namespace ns3

#endif /* AGENDA_H */
