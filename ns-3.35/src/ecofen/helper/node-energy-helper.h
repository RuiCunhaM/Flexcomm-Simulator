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
#ifndef NODE_ENERGY_HELPER_H
#define NODE_ENERGY_HELPER_H

#include "ns3/node-energy-model.h"
#include "ns3/node.h"
#include "ns3/node-container.h"

namespace ns3 {

class NodeEnergyModel;

/**
 * \brief Build a set of NodeEnergyModel objects
 *
 */
class NodeEnergyHelper : public Object
{
public:
  /**
   * Create a NodeEnergyHelper to make life easier when creating an 
   * energy model for a node.
   */
  NodeEnergyHelper ();
  virtual ~NodeEnergyHelper (){};

  /**
   * \param node Pointer to the node where NodeEnergyModel will be installed.
   *
   * This function installs a NodeEnergyModel onto a node.
   */
  void Install (Ptr<Node> node);

  /**
   * \param c List of nodes where NodeEnergyModel will be installed.
   *
   * This function installs an NodeEnergyModel onto a list of nodes.
   */
  void Install (NodeContainer c);

  /**
   * \param nodeName Name of node where NodeEnergyModel will be installed.
   *
   * This function installs a NodeEnergyModel onto a node.
   */
  void Install (std::string nodeName);

  /**
   * \brief This function installs a NodeEnergyModel on all nodes in simulation.
   */
  void InstallAll (void);

private:
  /**
   * \param node Pointer to node where the node energy model is to be installed.
   * \returns Pointer to the created NodeEnergyModel.
   *
   * Child classes of NodeEnergyHelper only have to implement this function,
   * to create and aggregate an NodeEnergyModel object onto a single node. Rest of
   * the installation process (eg. installing NodeEnergyModel on set of nodes) is
   * implemented in the NodeEnergyHelper base class.
   */
  virtual Ptr<NodeEnergyModel> DoInstall (Ptr<Node> node) const = 0;
};

} // namespace ns3

#endif /* NODE_ENERGY_HELPER_H */
