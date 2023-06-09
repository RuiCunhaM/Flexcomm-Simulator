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
 * along with this program.  If not, see <http: //www.gnu.org/licenses/>.
 *
 * Author: Rui Pedro C. Monteiro <rui.p.monteiro@inesctec.pt>
 */

#ifndef MIB_MANAGER_H
#define MIB_MANAGER_H

#include "ns3/node.h"
#include "ns3/object.h"
#include "mib.h"

namespace ns3 {

class MibManager : public Object
{
public:
  static TypeId GetTypeId (void);
  MibManager ();
  ~MibManager ();

  void DumpMibs (Time interval, Time stop, std::string path);

  void InstallMib (Ptr<Mib> mib);

  void SetNode (Ptr<Node> node);
  void SetCommunity (std::string community);

private:
  Ptr<Node> m_node;
  std::vector<Ptr<Mib>> m_mibs;
  std::string m_community;

  void DoDumpMibs (std::string path);
};

} // namespace ns3

#endif /* MIB_MANAGER_H */
