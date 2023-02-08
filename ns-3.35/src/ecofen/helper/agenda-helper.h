/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 CNRS, France
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
#ifndef AGENDA_HELPER_H
#define AGENDA_HELPER_H

#include "ns3/agenda.h"
#include "ns3/node.h"
#include "ns3/node-container.h"
#include "ns3/object-vector.h"
#include "ns3/net-device-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"

namespace ns3 {

class Agenda;

/**
 * \brief Build a set of Agenda objects
 */
class AgendaHelper : public Object
{
public:
  /**
   * Create an AgendaHelper to make life easier when wanting
   * to add agendas to links or nodes.
   */
  AgendaHelper ();
  ~AgendaHelper ();

  /**
   * Install an agenda on a NetDevice (aggregation).
   */
  void Install (Ptr<NetDevice> netdev);

  /**
   * Install an agenda on a NetDevice (aggregation).
   */
  void Install (NetDeviceContainer netdevc);

  /**
   * Create Node agenda
   */
  void CreateNodeAgenda (Ptr<Node> node);

  void CreateNodeAgenda (NodeContainer nodec);

private:
  ObjectFactory m_ag;
};

} // namespace ns3

#endif /* AGENDA_HELPER_H */
