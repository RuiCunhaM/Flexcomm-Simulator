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

#ifndef MIB_H
#define MIB_H

#include "ns3/object.h"
#include "ns3/output-stream-wrapper.h"
#include "ns3/node.h"
#include "ns3/nstime.h"

namespace ns3 {

class Mib : public Object
{
public:
  static TypeId GetTypeId (void);
  virtual ~Mib ();

  void SetNode (Ptr<Node> node);

  virtual void WriteToFile (Ptr<OutputStreamWrapper> streamWrapper);
  virtual void Initialize (void);

protected:
  Ptr<Node> m_node;
  std::string m_mibName;
};

} // namespace ns3

#endif /* MIB_H */
