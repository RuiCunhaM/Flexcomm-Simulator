/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * The GPLv2 License (GPLv2)
 *
 * Copyright (c) 2024 Rui Pedro C. Monteiro
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http: //www.gnu.org/licenses/>.
 *
 * Author: Rui Pedro C. Monteiro <rui.p.monteiro@inesctec.pt>
 */

#ifndef ENABLED_PORTS_ENERGY_HELPER_H_
#define ENABLED_PORTS_ENERGY_HELPER_H_

#include "node-energy-helper.h"

namespace ns3 {

class EnabledPortsEnergyHelper : public NodeEnergyHelper
{
public:
  EnabledPortsEnergyHelper ();
  ~EnabledPortsEnergyHelper ();

  void SetIdleConsumption (double consumption);
  void SetPortsConsumptions (std::map<uint64_t, double> values);

protected:
  virtual Ptr<NodeEnergyModel> DoInstall (Ptr<Node> node) const;
  ObjectFactory m_enabledPortsEnergyModel;
  double m_idle;
  std::map<uint64_t, double> m_values;
};

} // namespace ns3

#endif
