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

#ifndef NODE_ENERGY_MODEL_H
#define NODE_ENERGY_MODEL_H

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/type-id.h"
#include "ns3/node.h"
#include "netdevice-energy-model.h"
#include "ns3/simulator.h"
#include "ns3/output-stream-wrapper.h"

namespace ns3 {

/**
 * \ingroup energy
 * \brief Base class for node energy models.
 *
 */
class NodeEnergyModel : public Object
{
public:
  /**
   * Callback type for ChangeState function. Nodes use this state to notify
   * NodeEnergyModel of a state change.
   */
  typedef Callback<void, int> ChangeStateCallback;

public:
  /**
   * Get the TypeId of this class.
   */
  static TypeId GetTypeId (void);
  NodeEnergyModel ();
  virtual ~NodeEnergyModel ();

  /**
   * \returns Total power consumption of the node.
   *
   * NodeEnergyModel records its own energy consumption during simulation.
   *
   * \param node Pointer to the desired node.
   */
  double GetTotalPowerConsumption (Ptr<Node> node);

  double GetPowerDrawn ();

  /**
  * To log the energy consumption each time interval up to stop.
  *
  * \param interval Time interval between two logs (Time value).
  * \param stop Stop time for the logs (Time value).
  * \param node Pointer to the desired node.
  */
  void GetConso (Time interval, Time stop, Ptr<Node> node);

  void GetConsoLog (Time interval, Time stop, Ptr<Node> node,
                    Ptr<OutputStreamWrapper> streamWrapper);

  /**
  * Getter for node state.
  *
  * \return Current node state id.
  */
  uint32_t GetNodeState (void) const;

  /**
  * Setter for the node state.
  *
  * \param nodeState Desired node state id.
  */
  void SetNodeState (uint32_t nodeState);

  /**
  * Getter for current node On state.
  * A node can have several On states (in case of rate adaptation for instance).
  * This state is used by the OnOffNode::SwitchOn function.
  *
  * \return Current on state id.
  */
  uint32_t GetNodeOnState (void) const;

  /**
  * Setter for current node On state.
  * A node can have several On states (in case of rate adaptation for instance).
  * This state is used by the OnOffNode::SwitchOn function.
  *
  * \param state id of the desired on state.
  */
  void SetNodeOnState (uint32_t state);

  /**
  * Getter for current node Off state.
  * A node can have several Off states.
  * This state is used by the OnOffNode::SwitchOff function.
  *
  * \return Current off state id.
  */
  uint32_t GetNodeOffState (void) const;

  /**
  * Setter for current node Off state.
  * A node can have several Off states.
  * This state is used by the OnOffNode::SwitchOff function.
  *
  * \param state Id of the desired off state.
  */

  void SetNodeOffState (uint32_t state);

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
   * print the nodeId for example, it can invoke this method.
   */
  Ptr<Node> GetNode (void) const;

  /**
   * Update the node state when there is a switch.
   * Called by OnOffNode, AlrNode and LpiNode.
   * To implement in each child class.
   *
   * \param state New state.
   * \param energy Energy consumed by the state switch.
   * \param duration Duration of the state switch (Time value).
   */
  virtual void UpdateState (uint32_t state, double energy, Time duration);

private:
  /**
   * \returns power consumption of the chassis.
   *
   * To be implemented in each child class. 
   */
  virtual double GetPowerConsumption (void);

  void UpdateEnergy (Ptr<Node> node);
  void LogTotalPowerConsumption (Ptr<Node> node, Ptr<OutputStreamWrapper> streamWrapper);

private:
  uint32_t m_nodeState; // Node state for this node
  uint32_t m_nodeOnState; // Current On state for this node
  uint32_t m_nodeOffState; // Current Off state for this node

  double m_powerDrawn;
  double m_lastConso;
  Time m_lastUpdate;

  /**
   * Pointer to node containing this NodeEnergyModel. Used by helper class to make
   * sure energy models are installed onto the corresponding node.
   */
protected:
  Ptr<Node> m_node;
};

} // namespace ns3

#endif /* NODE_ENERGY_MODEL_H */
