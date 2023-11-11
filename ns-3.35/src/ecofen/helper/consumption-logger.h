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
#ifndef CONSUMPTION_LOGGER_H
#define CONSUMPTION_LOGGER_H

#include "ns3/node-energy-model.h"
#include "ns3/node.h"
#include "ns3/simulator.h"
#include "ns3/node-container.h"
#include "ns3/output-stream-wrapper.h"

namespace ns3 {

class ConsumptionLogger;

/**
 * \brief Define the parameters to get the power consumption values.
 *
 */
class ConsumptionLogger : public Object
{
public:
  /**
   * Create a ConsumptionLogger to make life easier when asking for 
   * power consumption logs.
   */
  ConsumptionLogger ();
  virtual ~ConsumptionLogger (){};

  /**
   * \param node Pointer to the node we want to log the power consumption.
   * \param interval: time value between two logs.
   * \param stop: time value when we stop to log.
   *
   * This function gets the power consumption of a node.
   */
  void NodeConso (Time interval, Time stop, Ptr<Node> node);
  void NodeConsoLog (Time interval, Time stop, Ptr<Node> node,
                     std::string path = "ecofen-trace.csv");

  /**
   * \param c List of nodes we want to log the power consumption.
   * \param interval: time value between two logs.
   * \param stop: time value when we stop to log.
   *
   * This function gets the power consumption of a list of nodes.
   */
  void NodeConso (Time interval, Time stop, NodeContainer c);
  void NodeConsoLog (Time interval, Time stop, NodeContainer c,
                     std::string path = "ecofen-trace.csv");

  /**
   * \param nodeName Name of node we want to log the power consumption.
   * \param interval: time value between two logs.
   * \param stop: time value when we stop to log.
   *
   * This function gets the power consumption of a node.
   */
  void NodeConso (Time interval, Time stop, std::string nodeName);
  void NodeConsoLog (Time interval, Time stop, std::string nodeName,
                     std::string path = "ecofen-trace.csv");

  /**
   * \brief This function gets the power consumption of all nodes in simulation.
   *
   * \param interval: time value between two logs.
   * \param stop: time value when we stop to log.
   *
   * This function gets the power consumption of all nodes in the simulation. 
   */
  void NodeConsoAll (Time interval, Time stop);
  void NodeConsoAllLog (Time interval, Time stop, std::string path = "ecofen-trace.csv");

private:
  void CreateLogFile (std::string path);
  void UpdateEnergy ();
  void LogEnergy ();

  static Ptr<OutputStreamWrapper> m_streamWrapper;
  static NodeContainer m_nodes;
  static NodeContainer m_nodesLog;
};

} // namespace ns3

#endif /* CONSUMPTION_LOGGER_H */
