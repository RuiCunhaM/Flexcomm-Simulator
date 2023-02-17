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
#include "ns3/ecofen-module.h"

namespace ns3 {

using namespace std;

void
parseSimConfigs (toml::table simConfigs)
{
  GlobalValue::Bind ("SimStopTime", StringValue (simConfigs["stopTime"].value_or ("60s")));
}

void
parseEcofenConfigs (toml::table ecofenConfigs, string outPath)
{
  ConsumptionLogger consoLogger;

  if (ecofenConfigs["logFile"].value_or (false))
    consoLogger.EnableLogFile (SystemPath::Append (outPath, "ecofen-trace"));

  TimeValue stopTime;
  GlobalValue::GetValueByName ("SimStopTime", stopTime);
  consoLogger.NodeConsoAll (Time (ecofenConfigs["interval"].value_or ("5s")), stopTime.Get ());
}

void
parseFlowMonitorConfigs (toml::table flowMonitorConfigs)
{
  GlobalValue::Bind ("FlowMonitorEnabled",
                     BooleanValue (flowMonitorConfigs["enable"].value_or (false)));
}

void
parseConfigs (std::string topoName, std::string outPath)
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

  parseSimConfigs (*tbl["simulator"].as_table ());
  parseEcofenConfigs (*tbl["ecofen"].as_table (), outPath);
  parseFlowMonitorConfigs (*tbl["flowMonitor"].as_table ());
}

} // namespace ns3
