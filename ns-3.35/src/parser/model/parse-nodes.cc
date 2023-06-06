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
#include "ns3/internet-stack-helper.h"
#include "toml.hpp"
#include "ns3/node.h"
#include "ns3/ecofen-module.h"
#include "ns3/topology-module.h"

namespace ns3 {

using namespace std;

Ptr<NodeEnergyHelper>
parseChassisEnergyModel (toml::table chassis)
{
  string chassisModel = chassis["model"].ref<string> ();

  if (!chassisModel.compare ("basic"))
    {
      Ptr<BasicNodeEnergyHelper> helper = CreateObject<BasicNodeEnergyHelper> ();
      helper->Set ("OnConso", DoubleValue (chassis["onConso"].value_or (0.0)));
      helper->Set ("OffConso", DoubleValue (chassis["offConso"].value_or (0.0)));
      return helper;
    }
  else if (!chassisModel.compare ("cpuLoad"))
    {
      toml::array &percentages = *chassis.get_as<toml::array> ("percentages");
      toml::array &consumptions = *chassis.get_as<toml::array> ("consumptions");
      map<double, double> values = map<double, double> ();
      Ptr<CpuLoadBaseEnergyHelper> helper = CreateObject<CpuLoadBaseEnergyHelper> ();

      for (size_t i = 0; i < percentages.size (); i++)
        {
          values[percentages.at (i).ref<double> ()] = consumptions.at (i).ref<double> ();
        }
      helper->SetUsageLvels (values);
      return helper;
    }
  else
    {
      NS_ABORT_MSG ("Unknown " << chassisModel << " chassis model");
    }
}

void
parseEnergyModels (toml::table configs, Ptr<Node> sw)
{
  if (!configs.contains ("chassis"))
    return;

  toml::table chassis = *configs["chassis"].as_table ();
  Ptr<NodeEnergyHelper> chassisHelper = parseChassisEnergyModel (chassis);
  chassisHelper->Install (sw);

  // TODO: Add interface models
}

void
parseNodes (string topoName)
{
  string nodesFile = SystemPath::Append (topoName, "nodes.toml");

  NS_ABORT_MSG_IF (!SystemPath::Exists (nodesFile), "nodes.toml not found");

  toml::table tbl;
  try
    {
      tbl = toml::parse_file (nodesFile);
    }
  catch (const toml::parse_error &err)
    {
      NS_ABORT_MSG ("Error parsing nodes.toml" << err.description ());
    }

  InternetStackHelper stackHelper;
  for (auto pair : tbl)
    {
      string nodeName = pair.first.data ();
      toml::table configs = *pair.second.as_table ();

      Ptr<Node> node = CreateObject<Node> ();
      Names::Add (nodeName, node);

      string nodeType = configs["type"].value_or ("switch");

      if (!nodeType.compare ("switch"))
        {
          node->SetAttribute ("NodeType", StringValue ("Switch"));
          parseEnergyModels (configs, node);
          Topology::AddSwitch (node);
        }
      else if (!nodeType.compare ("host"))
        {
          node->SetAttribute ("NodeType", StringValue ("Host"));
          stackHelper.Install (node);
        }
      else
        {
          NS_ABORT_MSG ("Unknown " << nodeType << " node type");
        }
    }
}

} // namespace ns3
