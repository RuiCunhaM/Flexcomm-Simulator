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

#include "parse-energy.h"
#include "ns3/energy-api.h"
#include "json.hpp"
#include <fstream>

namespace ns3 {

using namespace std;
using json = nlohmann::json;

void
parseEnergy (std::string topoName, std::string estiFile, std::string flexFile)
{
  string flexFilePath = SystemPath::Append (topoName, flexFile);
  string estiFilePath = SystemPath::Append (topoName, estiFile);

  if (SystemPath::Exists (flexFilePath))
    {
      try
        {
          std::ifstream flexFile (flexFilePath);
          json flex = json::parse (flexFile);

          for (const auto &pair : flex.items ())
            EnergyAPI::AddFlexArray (pair.key (), pair.value ());
        }
      catch (const json::parse_error &err)
        {
          NS_ABORT_MSG ("Failed to parse " << flexFilePath);
        }
    }
  else
    std::cout << flexFilePath << " not found. Skipping flexibility values." << std::endl;

  if (SystemPath::Exists (estiFilePath))
    {
      try
        {
          std::ifstream estiFile (estiFilePath);
          json esti = json::parse (estiFile);

          for (const auto &pair : esti.items ())
            EnergyAPI::AddEstimateArray (pair.key (), pair.value ());
        }
      catch (const json::parse_error &err)
        {
          NS_ABORT_MSG ("Failed to parse " << estiFilePath);
        }
    }
  else
    std::cout << estiFilePath << " not found. Skipping estimate values." << std::endl;
}

} // namespace ns3
