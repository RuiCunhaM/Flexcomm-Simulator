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
 * along with this program.  If not, see <http: //www.gnu.org/licenses/>.
 *
 * Author: Rui Pedro C. Monteiro <rui.p.monteiro@inesctec.pt>
 */

#include "parse-templates.h"
#include "parser.h"

namespace ns3 {

using namespace std;

#define TEMPLATES_FILE_DEFAULT "../topologies/energy-templates.toml"

Ptr<NodeEnergyHelper>
parseLoadBasedModel (toml::table chassis, Ptr<CpuLoadBasedEnergyHelper> helper)
{
  toml::array &percentages = *chassis.get_as<toml::array> ("percentages");
  toml::array &consumptions = *chassis.get_as<toml::array> ("consumptions");
  map<double, double> values = map<double, double> ();

  for (size_t i = 0; i < percentages.size (); i++)
    values[percentages.at (i).ref<double> ()] = consumptions.at (i).ref<double> ();

  helper->SetUsageLvels (values);
  return helper;
}

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
      Ptr<CpuLoadBasedEnergyHelper> helper = CreateObject<CpuLoadBasedEnergyHelper> ();
      return parseLoadBasedModel (chassis, helper);
    }
  else if (!chassisModel.compare ("cpuLoadDiscrete"))
    {
      Ptr<CpuLoadBasedDiscreteEnergyHelper> helper =
          CreateObject<CpuLoadBasedDiscreteEnergyHelper> ();
      return parseLoadBasedModel (chassis, helper);
    }
  else if (!chassisModel.compare ("enabledPorts"))
    {
      Ptr<EnabledPortsEnergyHelper> helper = CreateObject<EnabledPortsEnergyHelper> ();
      helper->SetIdleConsumption (chassis["idleConso"].value_or (0.0));

      toml::array &dataRates = *chassis.get_as<toml::array> ("dataRates");
      toml::array &consumptions = *chassis.get_as<toml::array> ("consumptions");
      map<uint64_t, double> values = map<uint64_t, double> ();

      for (size_t i = 0; i < dataRates.size (); i++)
        values[DataRate (dataRates.at (i).ref<string> ()).GetBitRate ()] =
            consumptions.at (i).ref<double> ();

      helper->SetPortsConsumptions (values);
      return helper;
    }
  else
    {
      NS_ABORT_MSG ("Unknown " << chassisModel << " chassis model");
    }
}

Ptr<NetdeviceEnergyHelper>
parseInterfaceEnergyModel (toml::table interface)
{
  string interfaceModel = interface["model"].ref<string> ();

  if (!interfaceModel.compare ("dataRate"))
    {
      Ptr<DataRateNetdeviceEnergyHelper> helper = CreateObject<DataRateNetdeviceEnergyHelper> ();
      helper->SetUnit (DataRate (interface["unit"].ref<string> ()).GetBitRate ());

      toml::array &dataRates = *interface.get_as<toml::array> ("dataRates");
      toml::array &consumptions = *interface.get_as<toml::array> ("consumptions");
      map<uint64_t, double> values = map<uint64_t, double> ();

      for (size_t i = 0; i < dataRates.size (); i++)
        values[DataRate (dataRates.at (i).ref<string> ()).GetBitRate ()] =
            consumptions.at (i).ref<double> ();

      helper->SetConsumptionValues (values);
      return helper;
    }
  else
    NS_ABORT_MSG ("Unknown " << interfaceModel << " interface model");
}

void
parseChassisTemplates (toml::table tbl)
{
  if (tbl.contains ("chassis"))
    {
      toml::table chassis = *tbl["chassis"].as_table ();

      for (auto pair : chassis)
        {
          toml::table configs = *pair.second.as_table ();
          Parser::m_chassisTemplates[pair.first.data ()] = parseChassisEnergyModel (configs);
        }
    }
}

void
parseInterfacesTemplates (toml::table tbl)
{
  if (tbl.contains ("interfaces"))
    {
      toml::table interfaces = *tbl["interfaces"].as_table ();

      for (auto pair : interfaces)
        {
          toml::table configs = *pair.second.as_table ();
          Parser::m_interfaceTemplates[pair.first.data ()] = parseInterfaceEnergyModel (configs);
        }
    }
}

void
parseTemplates (std::string topoName)
{

  toml::table tbl;

  // Default templates
  try
    {
      tbl = toml::parse_file (TEMPLATES_FILE_DEFAULT);
      parseChassisTemplates (tbl);
      parseInterfacesTemplates (tbl);
    }
  catch (const toml::parse_error &err)
    {
      cout << "Default templates file not found" << endl;
    }

  // Topology templates
  try
    {
      tbl = toml::parse_file (SystemPath::Append (topoName, "energy-templates.toml"));
      parseChassisTemplates (tbl);
      parseInterfacesTemplates (tbl);
    }
  catch (const toml::parse_error &err)
    {
      cout << "Topology templates file not found" << endl;
    }
}

} // namespace ns3
