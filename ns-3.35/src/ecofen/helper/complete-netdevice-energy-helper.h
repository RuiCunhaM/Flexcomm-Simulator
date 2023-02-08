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

#ifndef COMPLETE_NETDEVICE_ENERGY_HELPER_H
#define COMPLETE_NETDEVICE_ENERGY_HELPER_H

#include "netdevice-energy-helper.h"
#include "ns3/net-device.h"
#include "ns3/object-factory.h"

namespace ns3 {

/**
 * \ingroup energy
 * \brief Creates a CompleteNetdeviceEnergyModel object.
 *
 */
class CompleteNetdeviceEnergyHelper : public NetdeviceEnergyHelper
{
public:
  CompleteNetdeviceEnergyHelper ();
  ~CompleteNetdeviceEnergyHelper ();

  /**
   * To set the parameters values of this class.
   */

  void Set (std::string name, const AttributeValue &v);

private:
  virtual Ptr<NetdeviceEnergyModel> DoInstall (Ptr<NetDevice> netdevice) const;

private:
  ObjectFactory m_completeNetdeviceEnergyModel;
};

} // namespace ns3

#endif /* COMPLETE_NETDEVICE_ENERGY_HELPER_H */
