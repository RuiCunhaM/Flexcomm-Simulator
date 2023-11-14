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

  CommandLine cmd;
  cmd.AddValue ("topo", "Topology to load", topo);
  cmd.AddValue ("ctrl", "Controller to use", ctrl);
  cmd.AddValue ("checksum", "Calculate checksums", checksum);
  cmd.AddValue ("estifile", "Estimate file", estiFile);
  cmd.AddValue ("flexfile", "Flexibility file", flexFile);
  cmd.Parse (argc, argv);

  if (ctrl == "External")
    {
      GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));
      checksum = true; // Override checksum option
    }

  GlobalValue::Bind ("ControllerType", StringValue (ctrl));
  GlobalValue::Bind ("ChecksumEnabled", BooleanValue (checksum));

  SystemWallClockMs clock;
  uint64_t endTime;
  clock.Start ();

  Parser::ParseTopology (topo, estiFile, flexFile);

  endTime = clock.End ();
  uint64_t milli = endTime % 1000;
  uint64_t scds = (endTime / 1000) % 60;

  std::cout << "--------------------------------------------------------------" << std::endl;
  std::cout << "Parsing Time: " << scds << "s " << milli << "ms" << std::endl;

  FlowMonitorHelper flowHelper;
  if (FlowMonitor::IsEnabled ())
    flowHelper.InstallAll ();

  TimeValue stopTime;
  GlobalValue::GetValueByName ("SimStopTime", stopTime);
  Simulator::Stop (stopTime.Get ());

  clock.Start ();

  Simulator::Run ();

  endTime = clock.End ();
  scds = (endTime / 1000) % 60;
  uint64_t mins = ((endTime / (1000 * 60)) % 60);
  uint64_t hrs = ((endTime / (1000 * 60 * 60)) % 24);
  uint64_t days = ((endTime / (1000 * 60 * 60 * 24)));

  std::cout << "Nº Events: " << Simulator::GetEventCount () << std::endl;
  std::cout << "Execution Time: " << days << "d " << hrs << "h " << mins << "m " << scds << "s"
            << std::endl;
  std::cout << "--------------------------------------------------------------" << std::endl;

  flowHelper.SerializeToXmlFile (SystemPath::Append (topo, "flow-monitor.xml"), true, true);

  Simulator::Destroy ();
}
