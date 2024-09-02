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

#ifndef NETDEVICE_ENERGY_MODEL_H
#define NETDEVICE_ENERGY_MODEL_H

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/type-id.h"
#include "ns3/net-device.h"
#include "ns3/simulator.h"

namespace ns3 {

/**
 * \ingroup energy
 * \brief Base class for netdevice energy models.
 *
 */
class NetdeviceEnergyModel : public Object
{
public:
  /**
  * Get the TypeId of this class.
  */
  static TypeId GetTypeId (void);
  NetdeviceEnergyModel ();
  virtual ~NetdeviceEnergyModel ();

  /**
  * Getter for current net device state.
  * 0 = off, 1 = on, 2 = switching, other states can be added in subclasses by energy-aware functions.
  *
  * \return Id of current net device state.
  */
  uint32_t GetNetdeviceState (void) const;

  /**
  * Setter for current net device state.
  * 0 = off, 1 = on, 2 = switching, other states can be added in subclasses by energy-aware functions.
  *
  * \param netdeviceState Id of desired net device state.
  */

  void SetNetdeviceState (uint32_t netdeviceState);

  /**
  * Getter for current net device On state.
  * A net device can have several On states.
  * This state is used by the OnOffNetdevice::SwitchOn function.
  *
  * \return Id of the current net device on state.
  */
  uint32_t GetNetdeviceOnState (void) const;

  /**
  * Setter for current net device On state.
  * A net device can have several On states.
  * This state is used by the OnOffNetdevice::SwitchOn function.
  *
  * \param state Id of the desired net device on state.
  */

  void SetNetdeviceOnState (uint32_t state);

  /**
  * Getter for current net device Off state.
  * A net device can have several Off states.
  * This state is used by the OnOffNetdevice::SwitchOff function.
  *
  * \return Id of the current net device off state.
  */
  uint32_t GetNetdeviceOffState (void) const;

  /**
  * Setter for current net device Off state.
  * A net device can have several Off states.
  * This state is used by the OnOffNetdevice::SwitchOff function.
  *
  * \param state Id of the desired off state.
  */

  void SetNetdeviceOffState (uint32_t state);

  /**
   * \brief Sets pointer to net device containing this NetdeviceEnergyModel.
   *
   * \param netdevice Pointer to net device containing this NetdeviceEnergyModel.
   */
  void SetNetdevice (Ptr<NetDevice> netdevice);

  /**
   * \returns Pointer to net device containing this NetdeviceEnergyModel.
   *
   * When a subclass needs to get access to the underlying net device base class to
   * print the netDeviceId for example, it can invoke this method.
   */
  Ptr<NetDevice> GetNetdevice (void) const;

  /**
   * \returns energy consumption of the net device.
   *
   * To implement in each child class.
   */
  virtual double GetPowerConsumption (void);
  double GetPowerDrawn (void);
  double GetCurrentPowerConsumption (void);

  void GetConso (Time interval, Time stop);

  /**
   * Update the net device state when there is a switch.
   * Called by OnOffNetdevice, AlrNetdevice and LpiNetdevice.
   * To implement in each child class.
   *
   * \param state New state.
   * \param energy Energy consumed by the state switch.
   * \param duration Duration of the state switch.
   */
  virtual void UpdateState (uint32_t state, double energy, Time duration);

  void UpdateEnergy (void);

private:
  uint32_t m_netdeviceState; // State for this net device
  uint32_t m_netdeviceOnState; // Current On state for this net device
  uint32_t m_netdeviceOffState; // Current Off state for this net device

  double m_powerDrawn;
  double m_lastConso;
  Time m_lastUpdate;

  /**
   * Pointer to netdevice containing this NetdeviceEnergyModel. Used by helper class
   * to make sure energy models are installed onto the corresponding net device.
   */
  Ptr<NetDevice> m_netdevice;
};

} // namespace ns3

#endif /* NETDEVICE_ENERGY_MODEL_H */
