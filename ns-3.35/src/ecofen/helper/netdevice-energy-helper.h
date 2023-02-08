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
#ifndef NETDEVICE_ENERGY_HELPER_H
#define NETDEVICE_ENERGY_HELPER_H

#include "ns3/netdevice-energy-model.h"
#include "ns3/net-device.h"
#include "ns3/net-device-container.h"
#include "ns3/node.h"
#include "ns3/node-container.h"

namespace ns3 {

class NetdeviceEnergyModel;

/**
 * \brief Build a set of NetdeviceEnergyModel objects
 *
 */
class NetdeviceEnergyHelper : public Object
{
public:
  /**
   * Create a NetdeviceEnergyHelper to make life easier when creating an
   * energy model for a net device.
   */
  NetdeviceEnergyHelper ();
  virtual ~NetdeviceEnergyHelper ()
  {
  }

  /**
   * \param netdevice Pointer to the net device where NetdeviceEnergyModel will be installed.
   *
   * This function installs a NetdeviceEnergyModel onto a net device.
   */
  void Install (Ptr<NetDevice> netdevice);

  /**
   * \param c List of net devices where NetdeviceEnergyModel will be installed.
   *
   * This function installs a NetdeviceEnergyModel onto a list of netdevices.
   */
  void Install (NetDeviceContainer c);

  /**
   * \param node Pointer to the node where NetdeviceEnergyModel will be installed.
   *
   * This function installs a NetdeviceEnergyModel onto the net devices of a node.
   */
  void Install (Ptr<Node> node);

  /**
   * \param c List of nodes where NetdeviceEnergyModel will be installed.
   *
   * This function installs a NetdeviceEnergyModel onto the netdevices of a list of nodes.
   */
  void Install (NodeContainer c);

  /**
   * \param netdeviceName Name of net device where NetdeviceEnergyModel will be installed.
   *
   * This function installs a NetdeviceEnergyModel onto a net device.
   */
  void Install (std::string netdeviceName);

  /**
   * \brief This function installs a NodeEnergyModel on all net devices in simulation.
   */
  //void InstallAll (void);

private:
  /**
   * \param net device Pointer to node where the net device energy model is to be installed.
   * \returns Pointer to the created NetdeviceEnergyModel.
   *
   * Child classes of NetdeviceEnergyHelper only have to implement this function,
   * to create and aggregate an NetdeviceEnergyModel object onto a single net device. Rest of
   * the installation process (eg. installing NetdeviceEnergyModel on set of net devices) is
   * implemented in the NetdeviceEnergyHelper base class.
   */
  virtual Ptr<NetdeviceEnergyModel> DoInstall (Ptr<NetDevice> netdevice) const = 0;
};

} // namespace ns3

#endif /* NETDEVICE_ENERGY_HELPER_H */
