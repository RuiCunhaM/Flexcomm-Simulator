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
#ifndef ON_OFF_NODE_HELPER_H
#define ON_OFF_NODE_HELPER_H

#include "ns3/on-off-node.h"
#include "ns3/node.h"
#include "ns3/node-container.h"

namespace ns3 {

class OnOffNode;

/**
 * \brief Build a set of NodeEnergyModel objects
 *
 * Warning: energy models on the netdevices and nodes should be installed
 * before calling this function.
 */
class OnOffNodeHelper : public Object
{
public:
  /**
   * Create an OnOffNodeHelper to make life easier when wanting
   * to switch on/off a node.
   */
  OnOffNodeHelper ();
  ~OnOffNodeHelper ();

  /**
   * \param node Pointer to the node where OnOffNode will be installed.
   *
   * This function installs an OnOffNode onto a node.
   * The OnOffNode is aggregated to the NodeEnergyModel.
   */
  void Install (Ptr<Node> node);

  /**
   * \param c List of nodes where OnOffNode will be installed.
   *
   * This function installs an OnOffNodeEnergy onto a list of nodes.
   */
  void Install (NodeContainer c);

  /**
   * To set the parameters values of this class.
   */
  void Set (std::string name, const AttributeValue &v);

  /**
   * \param node Node to switch off.
   * \param date Time of the switch off.
   *
   * This function switches off a node.
   */
  void NodeSwitchOff (Ptr<Node> node, Time date);

  /**
   * \param nodec NodeContainer containing the nodes to switch off.
   * \param date Time of the switch off.
   *
   * This function switches off a set of nodes.
   */
  void NodeSwitchOff (NodeContainer nodec, Time date);

  /**
   * \param node Node to switch on. 
   * \param date Time of the switch on.
   *
   * This function switches on a node.
   */
  void NodeSwitchOn (Ptr<Node> node, Time date);

  /**
   * \param nodec NodeContainer containing the nodes to switch on.
   * \param date Time of the switch on.
   *
   * This function switches on a set of nodes.
   */
  void NodeSwitchOn (NodeContainer nodec, Time date);

  /**
   * \param node Node
   *
   * This function provides the duration of a switch off plus a switch on (in s.).
   */
  double GetDuration (Ptr<Node> node);

  /**
   * \param node Node
   *
   * This function provides the duration of a switch on (in s.).
   */
  double GetSwitchOn (Ptr<Node> node);

  /**
   * \param node Node
   *
   * This function provides the duration of a switch off (in s.).
   */
  double GetSwitchOff (Ptr<Node> node);

private:
  ObjectFactory m_onOffNode;
};

} // namespace ns3

#endif /* ON_OFF_NODE_HELPER_H */
