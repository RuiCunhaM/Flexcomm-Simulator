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

#ifndef ENERGY_API_H
#define ENERGY_API_H

#include "ns3/core-module.h"

namespace ns3 {

using namespace std;

class EnergyAPI : public Object
{

public:
  static TypeId GetTypeId (void);
  EnergyAPI ();
  ~EnergyAPI ();

  static vector<float> GetFlexArray (string id);
  static vector<float> GetEstimateArray (string id);

  static void AddFlexArray (string id, vector<float> arr);
  static void AddEstimateArray (string id, vector<float> arr);

  static void StartExternalServer (string topoName, string estiFile, string flexFile);
  static void StopExternalServer ();

private:
  static map<string, vector<float>> m_flexes;
  static map<string, vector<float>> m_estimates;
  static pid_t m_pid;
};

} // namespace ns3

#endif /* ENERGY_API_H */
