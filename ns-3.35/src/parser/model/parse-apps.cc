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
#include "ns3/applications-module.h"
#include "boost/algorithm/string.hpp"

namespace ns3 {

using namespace std;

#define getAddress(h) (h->GetObject<Ipv4> ()->GetAddress (1, 0).GetLocal ())

uint64_t portCounter = 10000;

std::string
protocol2factory (std::string proto)
{
  if (!proto.compare ("UDP"))
    return "ns3::UdpSocketFactory";
  else if (!proto.compare ("TCP"))
    return "ns3::TcpSocketFactory";
  else
    NS_ABORT_MSG ("Unknown " << proto << " protocol");
}

ApplicationContainer
installSinker (Ptr<Node> host, string protocol, uint64_t port)
{
  PacketSinkHelper sinkHelper (protocol, InetSocketAddress (Ipv4Address::GetAny (), port));
  return sinkHelper.Install (host);
}

ApplicationContainer
parseV4ping (toml::table configs, Ptr<Node> host, Ptr<Node> remoteHost)
{
  V4PingHelper v4helper = V4PingHelper (getAddress (remoteHost));
  v4helper.SetAttribute ("Verbose", BooleanValue (configs["verbose"].value_or (true)));
  v4helper.SetAttribute ("Interval", StringValue (configs["interval"].value_or ("1s")));
  return v4helper.Install (host);
}

ApplicationContainer
parseBulkSend (toml::table configs, Ptr<Node> host, Ptr<Node> remoteHost)
{
  Ipv4Address remoteAddress = getAddress (remoteHost);
  uint64_t port = configs["port"].value_or (portCounter++);
  string protocol = protocol2factory (configs["protocol"].value_or ("TCP"));

  BulkSendHelper bulkHelper = BulkSendHelper (protocol, InetSocketAddress (remoteAddress, port));
  bulkHelper.SetAttribute ("SendSize", UintegerValue (configs["sendSize"].value_or (512)));
  bulkHelper.SetAttribute ("MaxBytes", UintegerValue (configs["maxBytes"].value_or (0)));
  ApplicationContainer apps = bulkHelper.Install (host);

  apps.Add (installSinker (remoteHost, protocol, port));
  return apps;
}

ApplicationContainer
parseConstSend (toml::table configs, Ptr<Node> host, Ptr<Node> remoteHost)
{
  Ipv4Address remoteAddress = getAddress (remoteHost);
  uint64_t port = configs["port"].value_or (portCounter++);
  string protocol = protocol2factory (configs["protocol"].value_or ("UDP"));

  OnOffHelper onOffHelper = OnOffHelper (protocol, InetSocketAddress (remoteAddress, port));
  onOffHelper.SetAttribute ("PacketSize", UintegerValue (configs["packetSize"].value_or (512)));
  onOffHelper.SetAttribute ("MaxBytes", UintegerValue (configs["maxBytes"].value_or (0)));
  onOffHelper.SetConstantRate (DataRate (configs["dataRate"].value_or ("500kb/s")));
  ApplicationContainer apps = onOffHelper.Install (host);

  apps.Add (installSinker (remoteHost, protocol, port));
  return apps;
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

      ApplicationContainer apps;
      if (!appType.compare ("v4ping"))
        {
          apps = parseV4ping (configs, host, remoteHost);
        }
      else if (!appType.compare ("bulkSend"))
        {
          apps = parseBulkSend (configs, host, remoteHost);
        }
      else if (!appType.compare ("constSend"))
        {
          apps = parseConstSend (configs, host, remoteHost);
        }
      else
        {
          NS_ABORT_MSG ("Unknown " << appType << " application");
        }

      apps.Start (Time (configs["startTime"].value_or ("1s")));
      if (configs.contains ("stopTime"))
        apps.Stop (Time (configs["stopTime"].ref<string> ()));
    }
}

} // namespace ns3
