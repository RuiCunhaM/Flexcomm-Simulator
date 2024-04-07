/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * The GPLv2 License (GPLv2)
 *
 * Copyright (c) 2023 Rui Pedro C. Monteiro
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Rui Pedro C. Monteiro <rui.p.monteiro@inesctec.pt>
 */

#ifndef PARSE_TEMPLATES_H
#define PARSE_TEMPLATES_H

#include "ns3/core-module.h"
#include "ns3/ecofen-module.h"
#include "toml.hpp"

namespace ns3 {

Ptr<NodeEnergyHelper> parseChassisEnergyModel (toml::table chassis);
Ptr<NetdeviceEnergyHelper> parseInterfaceEnergyModel (toml::table interface);

void parseTemplates (std::string topoName);

} // namespace ns3

#endif /* PARSE_TEMPLATES_H */
