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
#include "parse-templates.h"
#include "ns3/snmp-module.h"

namespace ns3 {

using namespace std;

void
parseEnergyModels (toml::table configs, Ptr<Node> sw)
{
  if (!configs.contains ("chassis"))
    return;

  toml::table chassis = *configs["chassis"].as_table ();
  Ptr<NodeEnergyHelper> model = NULL;

  if (chassis.contains ("template"))
    model = Parser::m_templates[chassis["template"].ref<std::string> ()];
  else
    model = parseChassisEnergyModel (chassis);

  model->Install (sw);

  // TODO: Add interface models
}

void
parseMibs (toml::table configs, Ptr<Node> node)
{
  toml::array &mibsToml = *configs.get_as<toml::array> ("mibs");
  std::vector<std::string> mibs;
  for (size_t i = 0; i < mibsToml.size (); i++)
    node->AddMib (mibsToml.at (i).ref<std::string> ());
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
          node->SetAttribute ("CpuCapacity",
                              StringValue (configs["cpuCapacity"].value_or ("100Gbps")));
          parseEnergyModels (configs, node);
          Topology::AddSwitch (node);

          if (MibLogger::IsEnabled () && configs.contains ("mibs"))
            parseMibs (configs, node);
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
