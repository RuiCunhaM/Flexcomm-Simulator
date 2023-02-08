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
#ifndef ON_OFF_NETDEVICE_HELPER_H
#define ON_OFF_NETDEVICE_HELPER_H

#include "ns3/on-off-netdevice.h"
#include "ns3/net-device.h"
#include "ns3/net-device-container.h"
#include "ns3/node.h"
#include "ns3/node-container.h"

namespace ns3 {

class OnOffNetdevice;

/**
 * \brief Build a set of NetdeviceEnergyModel objects
 *
 */
class OnOffNetdeviceHelper : public Object
{
public:
  /**
   * Create an OnOffNetdeviceHelper to make life easier when wanting
   * to switch on/off a net device.
   */
  OnOffNetdeviceHelper ();
  ~OnOffNetdeviceHelper ();

  /**
   * \param netdevice Pointer to the net device where OnOffNetdevice will be installed.
   *
   * This function installs an OnOffNetdevice onto a net device.
   * The OnOffNetdevice is aggregated to the NetdeviceEnergyModel.
   */
  void Install (Ptr<NetDevice> netdevice);

  /**
   * \param c List of net devices where OnOffNetdevice will be installed.
   *
   * This function installs an OnOffNetdeviceEnergy onto a list of netdevices.
   */
  void Install (NetDeviceContainer c);

  /**
   * \param node Pointer to the node where OnOffNetdevice will be installed.
   *
   * This function installs an OnOffNetdevice onto the net devices of a node.
   */
  void Install (Ptr<Node> node);

  /**
   * \param c List of nodes where OnOffNetdevice will be installed.
   *
   * This function installs an OnOffNetdevice onto the netdevices of a list of nodes.
   */
  void Install (NodeContainer c);

  /**
   * \param netdeviceName Name of net device where OnOffNetdevice will be installed.
   *
   * This function installs an OnOffNetdevice onto a net device.
   */
  void Install (std::string netdeviceName);

  /**
   * To set the parameters values of this class.
   */
  void Set (std::string name, const AttributeValue &v);

  /**
   * \param link NetDeviceContainer containing two linked net devices to switch off.
   * \param date Time of the switch on.
   *
   * This function switches off two linked net devices.
   */
  void LinkSwitchOff (NetDeviceContainer link, Time date);

  /**
   * \param link NetDeviceContainer containing two linked net devices to switch on.
   * \param date Time of the switch on.
   *
   * This function switches on two linked net devices.
   */
  void LinkSwitchOn (NetDeviceContainer link, Time date);

  /**
   * \param netdev NetDevice to switch off.
   * \param date Time of the switch off.
   *
   * This function switches off a net device.
   */
  void NetDeviceSwitchOff (Ptr<NetDevice> netdev, Time date);

  /**
   * \param netdevc NetDeviceContainer containing the net devices to switch off.
   * \param date Time of the switch off.
   *
   * This function switches off a set of net devices.
   */
  void NetDeviceSwitchOff (NetDeviceContainer netdevc, Time date);

  /**
   * \param netdev NetDevice to switch on. 
   * \param date Time of the switch on.
   *
   * This function switches on a net device.
   */
  void NetDeviceSwitchOn (Ptr<NetDevice> netdev, Time date);

  /**
   * \param netdevc NetDeviceContainer containing the net devices to switch on.
   * \param date Time of the switch on.
   *
   * This function switches on a set of net devices.
   */
  void NetDeviceSwitchOn (NetDeviceContainer netdevc, Time date);

  /**
   * \param nd NetDevice
   *
   * This function provides the duration of a switch off plus a switch on (in s.).
   */
  double GetDuration (Ptr<NetDevice> nd);

  /**
   * \param nd NetDevice
   *
   * This function provides the duration of a switch on (in s.).
   */
  double GetSwitchOn (Ptr<NetDevice> nd);

  /**
   * \param nd NetDevice
   *
   * This function provides the duration of a switch off (in s.).
   */
  double GetSwitchOff (Ptr<NetDevice> nd);

private:
  ObjectFactory m_onOffNetdevice;
};

} // namespace ns3

#endif /* ON_OFF_NETDEVICE_H */
