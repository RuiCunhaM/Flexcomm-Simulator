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

#include "parser.h"
#include "parse-nodes.h"
#include "parse-links.h"
#include "parse-apps.h"
#include "parse-configs.h"

namespace ns3 {

using namespace std;

void
Parser::ParseTopology (string topoName)
{
  string outPath = topoName;
  string topoPath = SystemPath::Append ("../topologies", topoName);

  NS_ABORT_MSG_IF (!SystemPath::Exists (topoPath), "Topology " << topoName << " not found");

  parseNodes (topoPath);
  parseLinks (topoPath, outPath);
  parseApps (topoPath);
  parseConfigs (topoPath, outPath);
}

} // namespace ns3
