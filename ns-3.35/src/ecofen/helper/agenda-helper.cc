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
 * Author: Anne-Cécile Orgerie <anne-cecile.orgerie@irisa.fr>
 */

#include "ns3/abort.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/queue.h"
#include "ns3/config.h"
#include "ns3/names.h"
#include "ns3/agenda.h"
#include "ns3/agenda-helper.h"
#include "ns3/loopback-net-device.h"

NS_LOG_COMPONENT_DEFINE ("AgendaHelper");

namespace ns3 {

AgendaHelper::AgendaHelper ()
{
  m_ag.SetTypeId ("ns3::Agenda");
}

AgendaHelper::~AgendaHelper ()
{
}

void
AgendaHelper::Install (Ptr<NetDevice> netdev)
{
  NS_ASSERT (netdev != NULL);
  // check if agenda already exists
  Ptr<Agenda> nag = netdev->GetObject<Agenda> ();
  if (nag != NULL)
    {
      NS_FATAL_ERROR ("Net device agenda already installed!");
    }
  Ptr<Agenda> ag = m_ag.Create<Agenda> ();
  netdev->AggregateObject (ag);
}

void
AgendaHelper::Install (NetDeviceContainer netdevc)
{
  for (NetDeviceContainer::Iterator i = netdevc.Begin (); i != netdevc.End (); ++i)
    {
      Install (*i);
    }
}

void
AgendaHelper::CreateNodeAgenda (Ptr<Node> node)
{
  NS_ASSERT (node != NULL);
  // Check if agenda already exists
  Ptr<Agenda> nag = node->GetObject<Agenda> ();
  if (nag != NULL)
    {
      NS_FATAL_ERROR ("Node agenda already installed!");
    }
  Ptr<Agenda> agenda = m_ag.Create<Agenda> ();
  // Merge all net device's agendas
  for (uint32_t i = 0; i < node->GetNDevices (); i++)
    {
      Ptr<NetDevice> netdev = node->GetDevice (i);
      if (netdev->GetInstanceTypeId ().GetName () != "ns3::LoopbackNetDevice")
        {
          std::vector<double> ag = netdev->GetObject<Agenda> ()->GetTab ();
          for (uint32_t k = 0; k < ag.size (); k += 2)
            {
              agenda->AddEvent (ag[k], ag[k + 1]);
            }
        }
    }
  // Install agenda
  node->AggregateObject (agenda);
}

void
AgendaHelper::CreateNodeAgenda (NodeContainer nodec)
{
  for (NodeContainer::Iterator i = nodec.Begin (); i != nodec.End (); ++i)
    {
      CreateNodeAgenda (*i);
    }
}

} // namespace ns3
