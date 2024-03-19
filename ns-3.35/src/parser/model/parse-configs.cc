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

#include "toml.hpp"
#include "parse-configs.h"
#include "ns3/switch-stats-module.h"
#include "ns3/ecofen-module.h"
#include "ns3/link-stats-module.h"

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
  TimeValue stopTime;
  GlobalValue::GetValueByName ("SimStopTime", stopTime);

  consoLogger.NodeConsoAll (Time (ecofenConfigs["interval"].value_or ("5s")), stopTime.Get ());

  if (ecofenConfigs["logFile"].value_or (false))
    consoLogger.NodeConsoAllLog (Time (ecofenConfigs["logInterval"].value_or ("5s")),
                                 stopTime.Get (), SystemPath::Append (outPath, "ecofen-trace.csv"));
}

void
parseSwitchStatsConfigs (toml::table switchConfigs, string outPath)
{
  if (switchConfigs["enable"].value_or (false))
    {
      TimeValue stopTime;
      GlobalValue::GetValueByName ("SimStopTime", stopTime);

      SwitchStatsHelper statsHelper;
      statsHelper.InstallAll ();

      SwitchStatsLogger statsLogger (SystemPath::Append (outPath, "switch-stats.csv"));
      statsLogger.LogStatsAll (Time (switchConfigs["interval"].value_or ("5s")), stopTime.Get ());
    }
}

void
parseLinkStatsConfigs (toml::table linkConfigs, string outPath)
{
  LinkStatsHelper statsHelper;
  statsHelper.InstallAll ();

  LinkStatsLogger statsLogger;
  TimeValue stopTime;
  GlobalValue::GetValueByName ("SimStopTime", stopTime);

  statsLogger.ComputeStatsAll (Time (linkConfigs["interval"].value_or ("5s")), stopTime.Get ());

  if (linkConfigs["logFile"].value_or (false))
    statsLogger.ComputeStatsAllLog (Time (linkConfigs["logInterval"].value_or ("5s")),
                                    stopTime.Get (),
                                    SystemPath::Append (outPath, "link-stats.csv"));
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
  parseSwitchStatsConfigs (*tbl["switchStats"].as_table (), outPath);
  parseLinkStatsConfigs (*tbl["linkStats"].as_table (), outPath);
  parseFlowMonitorConfigs (*tbl["flowMonitor"].as_table ());
}

} // namespace ns3
