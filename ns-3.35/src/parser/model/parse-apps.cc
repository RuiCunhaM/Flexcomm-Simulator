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

#include "parse-apps.h"
#include "toml.hpp"
#include "ns3/v4ping-helper.h"
#include "ns3/internet-module.h"

namespace ns3 {

using namespace std;

void
parseV4ping (toml::table configs, Ptr<Node> host, Ptr<Node> remoteHost)
{
  V4PingHelper v4helper =
      V4PingHelper (remoteHost->GetObject<Ipv4> ()->GetAddress (1, 0).GetLocal ());
  v4helper.SetAttribute ("Verbose", BooleanValue (configs["verbose"].value_or (true)));
  v4helper.SetAttribute ("Interval", StringValue (configs["interval"].value_or ("1s")));
  v4helper.Install (host);
}

void
parseApps (std::string topoName)
{
  string appsFile = SystemPath::Append (topoName, "applications.toml");

  NS_ABORT_MSG_IF (!SystemPath::Exists (appsFile), "applications.toml not found");

  toml::table tbl;
  try
    {
      tbl = toml::parse_file (appsFile);
    }
  catch (const toml::parse_error &err)
    {
      NS_ABORT_MSG ("Error parsing applications.toml" << err.description ());
    }

  for (auto pair : tbl)
    {
      toml::table configs = *pair.second.as_table ();
      std::string appType = configs["type"].ref<std::string> ();

      Ptr<Node> host = Names::Find<Node> (configs["host"].ref<string> ());
      Ptr<Node> remoteHost = Names::Find<Node> (configs["remote"].ref<string> ());

      if (!appType.compare ("v4ping"))
        {
          parseV4ping (configs, host, remoteHost);
        }
      else
        {
          NS_ABORT_MSG ("Unknown " << appType << " application");
        }
    }
}

} // namespace ns3
