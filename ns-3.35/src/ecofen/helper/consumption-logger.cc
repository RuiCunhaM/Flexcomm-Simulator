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
#include "ns3/netdevice-energy-model.h"
#include "ns3/node-energy-model.h"
#include "ns3/simulator.h"
#include "ns3/queue.h"
#include "ns3/config.h"
#include "ns3/names.h"

#include "consumption-logger.h"
#include "ns3/basic-node-energy-model.h"

NS_LOG_COMPONENT_DEFINE ("ConsumptionLogger");

namespace ns3 {

NodeContainer ConsumptionLogger::m_nodes = NodeContainer ();
NodeContainer ConsumptionLogger::m_nodesLog = NodeContainer ();
Ptr<OutputStreamWrapper> ConsumptionLogger::m_streamWrapper = NULL;

ConsumptionLogger::ConsumptionLogger ()
{
}

void
ConsumptionLogger::NodeConso (Time interval, Time stop, Ptr<Node> node)
{
  Ptr<NodeEnergyModel> noem = node->GetObject<NodeEnergyModel> ();

  if (noem)
    {
      noem->GetConso (interval, stop, node);
    }

  for (uint32_t i = 0; i < node->GetNDevices (); i++)
    {
      Ptr<NetdeviceEnergyModel> ndem = node->GetDevice (i)->GetObject<NetdeviceEnergyModel> ();

      if (ndem)
        {
          ndem->GetConso (interval, stop);
        }
    }
}

void
ConsumptionLogger::NodeConsoLog (Time interval, Time stop, Ptr<Node> node, std::string path)
{
  Ptr<NodeEnergyModel> noem = node->GetObject<NodeEnergyModel> ();
  if (noem)
    {
      CreateLogFile (path);
      noem->GetConsoLog (interval, stop, node, m_streamWrapper);
    }
}

void
ConsumptionLogger::UpdateEnergy ()
{
  for (NodeContainer::Iterator i = m_nodes.Begin (); i != m_nodes.End (); ++i)
    {
      (*i)->GetObject<NodeEnergyModel> ()->UpdateEnergy (*i);

      for (uint32_t index = 0; index < ((*i)->GetNDevices ()); index++)
        {
          Ptr<NetdeviceEnergyModel> ndem =
              (*i)->GetDevice (index)->GetObject<NetdeviceEnergyModel> ();

          if (ndem)
            {
              ndem->UpdateEnergy ();
            }
        }
    }
}

void
ConsumptionLogger::NodeConso (Time interval, Time stop, NodeContainer c)
{
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<NodeEnergyModel> noem = (*i)->GetObject<NodeEnergyModel> ();

      if (noem)
        m_nodes.Add (*i);
    }

  Time i = Seconds (0.0);
  while (i <= stop)
    {
      Simulator::Schedule (i, &ConsumptionLogger::UpdateEnergy, this);
      i += interval;
    }
}

void
ConsumptionLogger::LogEnergy ()
{
  for (NodeContainer::Iterator i = m_nodesLog.Begin (); i != m_nodesLog.End (); ++i)
    (*i)->GetObject<NodeEnergyModel> ()->LogTotalPowerConsumption (*i, m_streamWrapper);
}

void
ConsumptionLogger::NodeConsoLog (Time interval, Time stop, NodeContainer c, std::string path)
{
  CreateLogFile (path);
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<NodeEnergyModel> noem = (*i)->GetObject<NodeEnergyModel> ();

      if (noem)
        m_nodesLog.Add (*i);
    }

  std::ostream *stream = m_streamWrapper->GetStream ();
  *stream << "Time;NodeName;Consumption\n";

  Time i = Seconds (0.0);
  while (i <= stop)
    {
      Simulator::Schedule (i, &ConsumptionLogger::LogEnergy, this);
      i += interval;
    }
}

void
ConsumptionLogger::NodeConso (Time interval, Time stop, std::string nodeName)
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  NodeConso (interval, stop, NodeContainer (node));
}

void
ConsumptionLogger::NodeConsoLog (Time interval, Time stop, std::string nodeName, std::string path)
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  NodeConsoLog (interval, stop, NodeContainer (node));
}

void
ConsumptionLogger::NodeConsoAll (Time interval, Time stop)
{
  NodeConso (interval, stop, NodeContainer::GetGlobal ());
}

void
ConsumptionLogger::NodeConsoAllLog (Time interval, Time stop, std::string path)
{
  NodeConsoLog (interval, stop, NodeContainer::GetGlobal (), path);
}

void
ConsumptionLogger::CreateLogFile (std::string path)
{
  if (!m_streamWrapper)
    m_streamWrapper = Create<OutputStreamWrapper> (path, std::ios::out);
}

} // namespace ns3
