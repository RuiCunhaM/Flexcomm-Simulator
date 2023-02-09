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

#include "parse-configs.h"
#include "toml.hpp"

namespace ns3 {

using namespace std;

void
parseConfigs (std::string topoName)
{
  string configsFile = SystemPath::Append (topoName, "configs.toml");

  NS_ABORT_MSG_IF (!SystemPath::Exists (configsFile), "applications.toml not found");

  toml::table tbl;
  try
    {
      tbl = toml::parse_file (configsFile);
    }
  catch (const toml::parse_error &err)
    {
      NS_ABORT_MSG ("Error parsing configs.toml" << err.description ());
    }

  toml::table simConfigs = *tbl["simulator"].as_table ();
  Simulator::Stop (Time (simConfigs["stopTime"].ref<string> ()));
}

} // namespace ns3
