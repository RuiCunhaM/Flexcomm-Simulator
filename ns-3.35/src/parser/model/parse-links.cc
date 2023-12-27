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
#include "ns3/point-to-point-ethernet-channel.h"
#include "toml.hpp"
#include "ns3/ipv4-address-helper.h"
#include "ns3/point-to-point-ethernet-helper.h"
#include "ns3/ofswitch13-module.h"
#include "ns3/topology-module.h"
#include "ns3/tap-bridge-module.h"

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
      Ptr<Channel> channel = p2pDevices.Get (0)->GetChannel ();
      Names::Add (pair.first.data (), channel);

      if (configs["pcap"].value_or (false))
        p2pHelper.EnablePcap (SystemPath::Append (outPath, "capture"), p2pDevices, true);

      if (n0->IsHost ())
        {
          Ipv4InterfaceContainer ips = address.Assign (p2pDevices.Get (0));
          Topology::AddHost (n0, ips.GetAddress (0));
        }

      if (n1->IsHost ())
        {
          Ipv4InterfaceContainer ips = address.Assign (p2pDevices.Get (1));
          Topology::AddHost (n1, ips.GetAddress (0));
        }

      Topology::AddLink (n0, n1, channel);
    }
}

void
installController (std::string outPath)
{
  // Create controller node
  Ptr<Node> controllerNode = CreateObject<Node> ();
  Names::Add ("controller", controllerNode);
  Ptr<OFSwitch13Helper> of13Helper;

  // Create controller
  StringValue controllerType;
  GlobalValue::GetValueByName ("ControllerType", controllerType);

  if (controllerType.Get () == "External")
    {
      of13Helper = CreateObject<OFSwitch13ExternalHelper> ();
      Ptr<NetDevice> ctrlDev = DynamicCast<OFSwitch13ExternalHelper> (of13Helper)
                                   ->InstallExternalController (controllerNode);

      // Install tapBridge
      TapBridgeHelper tapBridge;
      tapBridge.SetAttribute ("Mode", StringValue ("ConfigureLocal"));
      tapBridge.SetAttribute ("DeviceName", StringValue ("ctrl"));
      tapBridge.Install (controllerNode, ctrlDev);
    }
  else
    {
      ObjectFactory factory;
      of13Helper = CreateObject<OFSwitch13InternalHelper> ();
      of13Helper->SetChannelType (OFSwitch13Helper::DEDICATEDP2PETHERNET);
      factory.SetTypeId (controllerType.Get ());
      Ptr<OFSwitch13Controller> controller = factory.Create<OFSwitch13Controller> ();
      DynamicCast<OFSwitch13InternalHelper> (of13Helper)
          ->InstallController (controllerNode, controller);
    }

  NodeContainer switches = NodeContainer::GetGlobalSwitches ();
  for (NodeContainer::Iterator n = switches.Begin (); n != switches.End (); n++)
    {
      NetDeviceContainer switchPorts;
      for (uint32_t i = 0; i < (*n)->GetNDevices (); i++)
        {
          switchPorts.Add ((*n)->GetDevice (i));
        }
      of13Helper->SetDeviceAttribute ("CpuCapacity", DataRateValue ((*n)->GetCpuCapacity ()));
      of13Helper->InstallSwitch (*n, switchPorts);
      of13Helper->SetDeviceAttribute ("CpuCapacity", StringValue ("100Gbps"));
    }
  of13Helper->CreateOpenFlowChannels ();
  of13Helper->EnableOpenFlowPcap (SystemPath::Append (outPath, "ofchannel"));
}

void
parseLinkFailures (string topoName, string linkFailuresFile)
{
  if (!linkFailuresFile.compare ("NONE"))
    return;

  string failures = SystemPath::Append (topoName, linkFailuresFile);

  if (!SystemPath::Exists (failures))
    NS_ABORT_MSG ("Link Failures file not found " << failures);

  ifstream infile (failures);
  string link, state, time;

  while (infile >> time >> link >> state)
    {
      Ptr<PointToPointEthernetChannel> channel = Names::Find<PointToPointEthernetChannel> (link);

      if (!channel)
        NS_ABORT_MSG ("Unknown link when parsing link-failures: " << link);

      NetDeviceContainer devices = channel->GetDevices ();
      Time t = Time (time);

      // NOTE: Perhaps we should use a toggle?
      if (!state.compare ("down"))
        {
          Simulator::Schedule (t, &PointToPointEthernetNetDevice::SetLinkDown,
                               DynamicCast<PointToPointEthernetNetDevice> (devices.Get (0)));
          Simulator::Schedule (t, &PointToPointEthernetNetDevice::SetLinkDown,
                               DynamicCast<PointToPointEthernetNetDevice> (devices.Get (1)));
        }
      else
        {
          Simulator::Schedule (t, &PointToPointEthernetNetDevice::SetLinkUp,
                               DynamicCast<PointToPointEthernetNetDevice> (devices.Get (0)));
          Simulator::Schedule (t, &PointToPointEthernetNetDevice::SetLinkUp,
                               DynamicCast<PointToPointEthernetNetDevice> (devices.Get (1)));
        }
    }
}

void
parseLinks (string topoName, string outPath, string linkFailuresFile)
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
  installController (outPath);
  parseLinkFailures (topoName, linkFailuresFile);
}

} // namespace ns3
