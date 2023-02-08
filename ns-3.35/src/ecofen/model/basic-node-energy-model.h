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

#ifndef BASIC_NODE_ENERGY_MODEL_H_
#define BASIC_NODE_ENERGY_MODEL_H_

#include "node-energy-model.h"
#include "ns3/nstime.h"
#include "ns3/traced-value.h"

namespace ns3 {

/**
 * \ingroup energy
 *
 * \brief A simple node energy model.
 *
 */
class BasicNodeEnergyModel : public NodeEnergyModel
{
public:
  /**
   * Get the TypeId of this class.
   */
  static TypeId GetTypeId (void);
  BasicNodeEnergyModel ();
  virtual ~BasicNodeEnergyModel ();

  /**
   * \brief Initialize the internal parameters with the values set by the user.
   */
  void Initialize ();

  /**
   * \brief Set pointer to node.
   *
   * \param node Pointer to node.
   *
   * Implements BasicNodeEnergyModel::SetNode.
   */
  virtual void SetNode (Ptr<Node> node);

  /**
   * \brief Get pointer to node.
   *
   * \returns Pointer to node.
   *
   * Implements BasicNodeEnergyModel::GetNode.
   */
  virtual Ptr<Node> GetNode (void) const;

  /**
   * Set the power consumption when on.
   *
   * \param onconso Power consumption (in Watts) when the node is on.
   */
  void SetOnConso (double onconso);

  /**
   * Set the power consumption when off.
   *
   * \param offconso Power consumption (in Watts) when the node is off.
   */
  void SetOffConso (double offconso);

  /**
   * To add states for energy-aware functions.
   *
   * \param conso Power consumption of the new state (in Watts).
   *
   * \returns Id of this new state.
   */
  uint32_t AddState (double conso);

  /**
   * Update the node state when there is a switch.
   * Implements NodeEnergyModel:UpdateState.
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
   * Implements NodeEnergyModel::GetPowerConsumption.
   */
  virtual double GetPowerConsumption (void);

private:
  double m_onConso;
  double m_offConso;
  Time m_lastUpdateTime;
  Ptr<Node> m_node;
  //double m_totalEnergyConsumption;

  std::vector<double> m_stateConso;
  std::vector<uint32_t> m_stateTab;
  std::vector<Time> m_timeTab;
  std::vector<double> m_switchEnergyTab;
};

} // namespace ns3

#endif /* BASIC_NODE_ENERGY_MODEL_H_ */
