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

#ifndef LINEAR_NETDEVICE_ENERGY_MODEL_H_
#define LINEAR_NETDEVICE_ENERGY_MODEL_H_

#include <vector>
#include "ns3/object-vector.h"
#include "netdevice-energy-model.h"
#include "ns3/nstime.h"
#include "ns3/traced-value.h"

namespace ns3 {

/**
 * \ingroup energy
 *
 * \brief A linear net device energy model.
 * 
 * Warning: this energy model can be attached only to PointToPoint and Csma NetDevices.
 *
 */
class LinearNetdeviceEnergyModel : public NetdeviceEnergyModel
{
public:
  /**
   * Get the TypeId of this class.
   */
  static TypeId GetTypeId (void);
  LinearNetdeviceEnergyModel ();
  virtual ~LinearNetdeviceEnergyModel ();

  /**
   * \brief Initialize the internal parameters with the values set by the user.
   */
  void Initialize ();

  /**
   * \brief Set pointer to the net device.
   *
   * \param netdevice Pointer to the net device.
   *
   * Implements LinearNetdeviceEnergyModel::SetNetdevice.
   */
  virtual void SetNetdevice (Ptr<NetDevice> netdevice);

  /**
   * \brief Get pointer to the net device.
   *
   * \returns Pointer to the net device.
   *
   * Implements LinearNetdeviceEnergyModel::GetNetdevice.
   */
  virtual Ptr<NetDevice> GetNetdevice (void) const;

  /**
   * Set the power consumption when idle (static power consumption).
   *
   * \param idleconso Value in Watts of the idle power consumption for the net device.
   */
  void SetIdleConso (double idleconso);

  /**
   * Set the power consumption when off.
   *
   * \param offconso Value in Watts of the power consumption when the net device is off.
   */
  void SetOffConso (double offconso);

  /**
   * This net device energy model assumes a linear dependance between energy consumption
   * and traffic amount in volume (Bytes).
   * Set the dynamic energy consumption parameter.
   *
   * \param byteenergy Parameter for the linear dependance: energy consumption of the net device
   * in nJ per Byte.
   */
  void SetByteEnergy (double byteenergy);

  /**
   * To add a new state for energy-aware functions.
   *
   * \param staticConso Static power consumption of the new state in Watts.
   * \param dynamicEnergy Dynamic energy consumption of the new state in nJ per Byte.
   *
   * \returns Id of this new state.
   */
  uint32_t AddState (double staticConso, double dynamicEnergy);

  /**
   * \returns Total energy consumption of the net device.
   *
   * Implements NetdeviceEnergyModel::GetPowerConsumption.
   */
  virtual double GetPowerConsumption (void);

  /**
   * Update the net device state when there is a switch.
   *
   * \param state New state id.
   * \param energy Energy consumed by the state switch.
   * \param duration Duration of the state switching (Time value).
   */
  virtual void UpdateState (uint32_t state, double energy, Time duration);

private:
  /**
  * \returns Number of Bytes received.
  */
  static void GetRecvBytes (std::vector<double> args, std::string context,
                            Ptr<const Packet> packet);

  double GetNbRecvBytes (void);

  /**
  * \returns Number of Bytes sent.
  */
  static void GetSentBytes (std::vector<double> args, std::string context,
                            Ptr<const Packet> packet);

  double GetNbSentBytes (void);

private:
  // To deal with the states
  std::vector<double> m_stateIdleConso;
  std::vector<double> m_stateByteEnergy;
  double m_idleConso;
  double m_offConso;
  double m_byteEnergy;

  // To deal with the callbacks
  uint32_t m_slastPos;
  uint32_t m_rlastPos;

  Ptr<NetDevice> m_netdevice;

  // To deal with the state changes
  Time m_lastUpdateTime;
  std::vector<uint32_t> m_stateTab;
  std::vector<Time> m_timeTab;
  std::vector<double> m_switchEnergyTab;
  std::vector<uint32_t> m_bytesNbTab;
};

} // namespace ns3

#endif /* LINEAR_NETDEVICE_ENERGY_MODEL_H_ */
