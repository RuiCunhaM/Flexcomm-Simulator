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

#ifndef ONOFF_NODE_H
#define ONOFF_NODE_H

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/type-id.h"
#include "ns3/net-device.h"
#include "ns3/nstime.h"
#include "node-energy-model.h"
#include <vector>
#include "ns3/object-vector.h"
#include "netdevice-energy-model.h"

namespace ns3 {

/**
 * \ingroup energy
 * \brief Base class for on/off functions on nodes.
 * Warning: this class works at the condition that the user is using IPv4 or IPv6 on the concerned nodes.
 */
class OnOffNode : public Object
{
public:
  /**
   * Get the TypeId of this class.
   */
  static TypeId GetTypeId (void);
  OnOffNode ();
  virtual ~OnOffNode ();

  /**
   * \brief Sets pointer to node containing this NodeEnergyModel.
   *
   * \param node Pointer to node containing this NodeEnergyModel.
   */
  void SetNode (Ptr<Node> node);

  /**
   * \returns Pointer to node containing this NodeEnergyModel.
   *
   * When a subclass needs to get access to the underlying node base class to
   * print the NodeId for example, it can invoke this method.
   */
  Ptr<Node> GetNode (void) const;

  /**
   *
   * To switch off a node.
   *
   * \param node Pointer to the node to switch off
   */
  void SwitchOff (Ptr<Node> node);

  /**
   *
   * To switch on a node.
   *
   * \param node Pointer to the node to switch on
   */
  void SwitchOn (Ptr<Node> node);

  /**
   * Used to store the state number of the zero states of the net devices.
   * Used by OnOffNodeHelper::Install.
   *
   * \param statenb State number of the new zero state.
   */
  void AddNDZeroState (uint32_t statenb);

private:
  /**
   * Pointer to node containing this OnOffNode. Used by helper class
   * to make sure models are installed onto the corresponding node.
   */
  Ptr<Node> m_node;

  double m_switchOnEnergy;
  double m_switchOffEnergy;
  double m_switchOnDuration;
  double m_switchOffDuration;
  Time m_lastSwitchEnd;
  std::vector<uint32_t> m_zeroStates;
};

} // namespace ns3

#endif /* ONOFF_NODE_H */
