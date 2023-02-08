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

#ifndef BASIC_NETDEVICE_ENERGY_MODEL_H_
#define BASIC_NETDEVICE_ENERGY_MODEL_H_

#include <vector>
#include "netdevice-energy-model.h"
#include "ns3/nstime.h"
#include "ns3/traced-value.h"
#include "ns3/object-vector.h"

namespace ns3 {

/**
 * \ingroup energy
 *
 * \brief A simple net device energy model.
 *
 */
class BasicNetdeviceEnergyModel : public NetdeviceEnergyModel
{
public:
  /**
   * Get the TypeId of this class.
   */
  static TypeId GetTypeId (void);
  BasicNetdeviceEnergyModel ();
  virtual ~BasicNetdeviceEnergyModel ();

  /**
   * \brief Initialize the internal parameters with the values set by the user.
   */
  void Initialize ();

  /**
   * \brief Set pointer to the net device.
   *
   * \param netdevice Pointer to the net device.
   *
   * Implements BasicNetdeviceEnergyModel::SetNetdevice.
   */
  virtual void SetNetdevice (Ptr<NetDevice> netdevice);

  /**
   * \brief Get pointer to the net device.
   *
   * \returns Pointer to the net device.
   *
   * Implements BasicNetdeviceEnergyModel::GetNetdevice.
   */
  virtual Ptr<NetDevice> GetNetdevice (void) const;

  /**
   * Set the power consumption when on.
   *
   * \param onconso Value in Watts of the on conso.
   */
  void SetOnConso (double onconso);

  /**
   * Set the power consumption when off.
   *
   * \param offconso Value in Watts of the off conso.
   */
  void SetOffConso (double offconso);

  /**
   * To add a new state for energy-aware functions.
   *
   * \param conso Power consumption of the new state in Watts.
   *
   * \returns Id of this new state.
   */
  uint32_t AddState (double conso);

  /**
   * \returns Total energy consumption of the net device.
   *
   * Implements NetdeviceEnergyModel::GetPowerConsumption.
   */
  virtual double GetPowerConsumption (void);

  /**
   * Update the net device state when there is a switch.
   * Implements NetdeviceEnergyModel:UpdateState.
   *
   * \param state Id of the new state.
   * \param energy Energy consumed by the state switch.
   * \param duration Duration of the state switch (Time value).
   */
  virtual void UpdateState (uint32_t state, double energy, Time duration);

private:
  std::vector<double> m_stateConso;
  double m_offConso;
  double m_onConso;

  Time m_lastUpdateTime;
  Ptr<NetDevice> m_netdevice;

  std::vector<uint32_t> m_stateTab;
  std::vector<Time> m_timeTab;
  std::vector<double> m_switchEnergyTab;
};

} // namespace ns3

#endif /* BASIC_NETDEVICE_ENERGY_MODEL_H_ */
