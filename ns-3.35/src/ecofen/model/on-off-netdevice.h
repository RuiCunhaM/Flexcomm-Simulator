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

#ifndef ONOFF_NETDEVICE_H
#define ONOFF_NETDEVICE_H

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/type-id.h"
#include "ns3/net-device.h"
#include "ns3/nstime.h"
#include "netdevice-energy-model.h"

namespace ns3 {

/**
 * \ingroup energy
 * \brief Base class for on/off functions on net devices.
 * Warning: this class works at the condition that the user is using IPv4 or IPv6 on the concerned nodes.
 */
class OnOffNetdevice : public Object
{
public:
  /**
   * Get the TypeId of this class.
   */
  static TypeId GetTypeId (void);
  OnOffNetdevice ();
  virtual ~OnOffNetdevice ();

  /**
   * \brief Sets pointer to net device containing this NetdeviceEnergyModel.
   *
   * \param netdevice Pointer to the net device containing this NetdeviceEnergyModel.
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
   *
   * To switch off a netdevice.
   *
   * \param interface Id of the interface to switch on the node.
   * \param netdevice Pointer to the netdevice to switch off
   */
  void SwitchOff (uint32_t interface, Ptr<NetDevice> netdevice);

  /**
   *
   * To switch on a netdevice.
   *
   * \param interface Id of the interface to switch on the node.
   * \param netdevice Pointer to the netdevice to switch on.
   */
  void SwitchOn (uint32_t interface, Ptr<NetDevice> netdevice);

private:
  void DelayedPrintOn (uint32_t interface, uint32_t node);
  /**
   * Pointer to netdevice containing this OnOffNetdevice. Used by helper class
   * to make sure onoff models are installed onto the corresponding net device.
   */
  Ptr<NetDevice> m_netdevice;

  double m_switchOnEnergy;
  double m_switchOffEnergy;
  double m_switchOnDuration;
  double m_switchOffDuration;
  Time m_lastSwitchEnd;
};

} // namespace ns3

#endif /* ONOFF_NETDEVICE_H */
