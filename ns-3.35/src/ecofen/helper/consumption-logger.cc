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

#include "consumption-logger.h"
#include "ns3/basic-node-energy-model.h"

NS_LOG_COMPONENT_DEFINE ("ConsumptionLogger");

namespace ns3 {

ConsumptionLogger::ConsumptionLogger ()
{
}

void
ConsumptionLogger::NodeConso (Time interval, Time stop, Ptr<Node> node)
{
  Ptr<NodeEnergyModel> noem = node->GetObject<NodeEnergyModel> ();

  if (noem)
    noem->GetConso (interval, stop, node);
}

void
ConsumptionLogger::NodeConsoLog (Time interval, Time stop, Ptr<Node> node, std::string path)
{
  Ptr<NodeEnergyModel> noem = node->GetObject<NodeEnergyModel> ();
  if (noem)
    {
      Ptr<OutputStreamWrapper> wrapper =
          Create<OutputStreamWrapper> (path + Names::FindName (node), std::ios::out);
      noem->GetConsoLog (interval, stop, node, wrapper);
    }
}

void
ConsumptionLogger::NodeConso (Time interval, Time stop, NodeContainer c)
{
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      NodeConso (interval, stop, *i);
    }
}

void
ConsumptionLogger::NodeConsoLog (Time interval, Time stop, NodeContainer c, std::string path)
{
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      NodeConsoLog (interval, stop, *i, path);
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

} // namespace ns3
