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

#include "parse-links.h"
#include "parser.h"
#include "toml.hpp"
#include "ns3/ipv4-address-helper.h"
#include "ns3/point-to-point-ethernet-helper.h"
#include "ns3/ofswitch13-module.h"

namespace ns3 {

using namespace std;

void
installLinks (toml::table tbl, string outPath)
{
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  for (auto pair : tbl)
    {
      toml::table configs = *pair.second.as_table ();
      string node0 = configs["edges"][0].ref<string> ();
      string node1 = configs["edges"][1].ref<string> ();
      Ptr<Node> n0 = Names::Find<Node> (node0);
      Ptr<Node> n1 = Names::Find<Node> (node1);

      PointToPointEthernetHelper p2pHelper;
      p2pHelper.SetChannelAttribute ("Delay", StringValue (configs["delay"].value_or ("1ns")));
      p2pHelper.SetDeviceAttribute ("DataRate",
                                    StringValue (configs["dataRate"].value_or ("1000Gbps")));

      NetDeviceContainer p2pDevices = p2pHelper.Install (NodeContainer (n0, n1));
      Names::Add (pair.first.data (), p2pDevices.Get (0)->GetChannel ());

      if (configs["pcap"].value_or (false))
        p2pHelper.EnablePcap (SystemPath::Append (outPath, "capture"), p2pDevices, true);

      if (n0->IsHost ())
        address.Assign (p2pDevices.Get (0));

      if (n1->IsHost ())
        address.Assign (p2pDevices.Get (1));
    }
}

void
installController ()
{
  // We assume a single controller at the moment
  Ptr<Node> controllerNode = CreateObject<Node> ();
  Names::Add ("controller", controllerNode);

  Ptr<OFSwitch13InternalHelper> of13Helper = CreateObject<OFSwitch13InternalHelper> ();
  of13Helper->InstallController (controllerNode);

  NodeContainer switches = NodeContainer::GetGlobalSwitches ();
  for (NodeContainer::Iterator n = switches.Begin (); n != switches.End (); n++)
    {
      NetDeviceContainer switchPorts;
      for (uint32_t i = 0; i < (*n)->GetNDevices (); i++)
        {
          switchPorts.Add ((*n)->GetDevice (i));
        }
      of13Helper->InstallSwitch (*n, switchPorts);
    }
  of13Helper->CreateOpenFlowChannels ();
}

void
parseLinks (string topoName, string outPath)
{
  string linksFile = SystemPath::Append (topoName, "links.toml");

  NS_ABORT_MSG_IF (!SystemPath::Exists (linksFile), "links.toml not found");

  toml::table tbl;
  try
    {
      tbl = toml::parse_file (linksFile);
    }
  catch (const toml::parse_error &err)
    {
      NS_ABORT_MSG ("Error parsing links.toml" << err.description ());
    }

  installLinks (tbl, outPath);
  installController ();
}

} // namespace ns3
