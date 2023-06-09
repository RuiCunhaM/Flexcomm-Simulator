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

#include "ns3/core-module.h"
#include "ns3/parser-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/system-wall-clock-ms.h"
#include "ns3/snmp-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Flexcomm");

int
main (int argc, char *argv[])
{

  std::string topo;
  std::string ctrl;
  std::string estiFile;
  std::string flexFile;
  bool checksum;
  bool snmp;

  CommandLine cmd;
  cmd.AddValue ("topo", "Topology to load", topo);
  cmd.AddValue ("ctrl", "Controller to use", ctrl);
  cmd.AddValue ("checksum", "Calculate checksums", checksum);
  cmd.AddValue ("estifile", "Estimate file", estiFile);
  cmd.AddValue ("flexfile", "Flexibility file", flexFile);
  cmd.AddValue ("snmp", "Enable Mib Logger", snmp);
  cmd.Parse (argc, argv);

  if (ctrl == "External")
    {
      GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));
      checksum = true; // Override checksum option

      if (snmp)
        GlobalValue::Bind ("MibLoggerEnabled", BooleanValue (true));
    }

  GlobalValue::Bind ("ControllerType", StringValue (ctrl));
  GlobalValue::Bind ("ChecksumEnabled", BooleanValue (checksum));

  SystemWallClockMs clock;
  uint64_t endTime;
  clock.Start ();

  Parser::ParseTopology (topo, estiFile, flexFile);

  endTime = clock.End ();
  uint64_t milliseconds = endTime % 1000;
  uint64_t seconds = (endTime / 1000) % 60;
  std::cout << "Parsing Time: " << seconds << "s " << milliseconds << "ms" << std::endl;

  FlowMonitorHelper flowHelper;
  if (FlowMonitor::IsEnabled ())
    flowHelper.InstallAll ();

  TimeValue stopTime;
  GlobalValue::GetValueByName ("SimStopTime", stopTime);
  Simulator::Stop (stopTime.Get ());

  if (MibLogger::IsEnabled ())
    {
      // TODO: This is a proof of concept. We need to make this
      // more generic to support more mibs
      NodeContainer switches = NodeContainer::GetGlobalSwitches ();
      MibManagerHelper managerHelper;
      managerHelper.Install (switches);

      for (NodeContainer::Iterator it = switches.Begin (); it != switches.End (); it++)
        {
          for (std::string mib : (*it)->GetMibs ())
            {
              if (mib == "energy")
                {
                  EnergyMibHelper energyHelper;
                  energyHelper.Install ((*it));
                }
            }
        }
      MibLogger logger (SystemPath::Append (topo, "snmp"));
      // TODO: Allow setting custom interval
      logger.EnableDumpAll (Time ("5s"), stopTime.Get ());
    }

  clock.Start ();

  Simulator::Run ();

  endTime = clock.End ();
  seconds = (endTime / 1000) % 60;
  uint64_t minutes = ((endTime / (1000 * 60)) % 60);
  uint64_t hours = ((endTime / (1000 * 60 * 60)) % 24);

  std::cout << "Execution Time: " << hours << "h " << minutes << "m " << seconds << "s"
            << std::endl;

  flowHelper.SerializeToXmlFile (SystemPath::Append (topo, "flow-monitor.xml"), true, true);

  Simulator::Destroy ();
}
